// Fill out your copyright notice in the Description page of Project Settings.


#include "OctreeNode.h"
#include <stack>

OctreeNode::OctreeNode()
{
	this->parent = nullptr;
	childrenWithBoids = 0;
}

OctreeNode::OctreeNode(FBox region, OctreeNode* parent, TArray<ABoid*>& boids, int boidsCapacity)
{
	for (int i = 0; i < CHILDREN_CAPACITY; ++i)
	{
		child[i] = nullptr;
	}

	this->region = region;
	this->parent = parent;
	this->boids = boids;
	this->boidsCapacity = boidsCapacity;
	childrenWithBoids = 0;
}

void OctreeNode::BuildTree()
{
	//if true then this is the leaf node which will hold not more than 100 boids in it's space
	if (boids.Num() <= boidsCapacity)
	{
		return;
	}

	FVector center = region.GetCenter();

	FBox octant[CHILDREN_CAPACITY];

	//if the dimensions of this node is greater then the min required then this is not a leaf node; subdivide
	
	octant[0] = FBox(region.Min, center);
	octant[1] = FBox(FVector(center.X, region.Min.Y, region.Min.Z), FVector(region.Max.X, center.Y, center.Z));
	octant[2] = FBox(FVector(center.X, region.Min.Y, center.Z), FVector(region.Max.X, center.Y, region.Max.Z));
	octant[3] = FBox(FVector(region.Min.X, region.Min.Y, center.Z), FVector(center.X, center.Y, region.Max.Z));
	octant[4] = FBox(FVector(region.Min.X, center.Y, region.Min.Z), FVector(center.X, region.Max.Y, center.Z));
	octant[5] = FBox(FVector(center.X, center.Y, region.Min.Z), FVector(region.Max.X, region.Max.Y, center.Z));
	octant[6] = FBox(center, region.Max);
	octant[7] = FBox(FVector(region.Min.X, center.Y, center.Z), FVector(center.X, region.Max.Y, region.Max.Z));

	//list of boids distributed amongst the 8 children
	TArray<ABoid*> boidDistribution[CHILDREN_CAPACITY];
	//contains boids moved down to children and which can thus be safely removed fro this node
	TArray<ABoid*> boidsMovedToChildren;

	//distrubute the boids
	for (auto boid : boids)
	{		
		FVector boidPosition = boid->GetTransform().GetLocation();

		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			if (octant[i].IsInsideOrOn(boidPosition))
			{
				boidDistribution[i].Add(boid);
				boidsMovedToChildren.Add(boid);
			}
		}
	}

	//remove boidsMovedToChildren from this node
	for (auto boid : boidsMovedToChildren)
		boids.Remove(boid);

	//distribute the boids amongst children

	//for (int i = 0; i < 8; ++i)
	for (int i = 0; i < CHILDREN_CAPACITY; ++i)
	{
		//child.Add(new OctreeNode(octant[i], this, boidDistribution[i]));
		if (child[i] == nullptr)
		{
			child[i] = new OctreeNode(octant[i], this, boidDistribution[i], boidsCapacity);
		}
		else
		{
			for (auto boid : boidDistribution[i])
			{
				child[i]->boids.Add(boid);
			}				
		}
		//childrenWithBoids |= (1 << i);
		child[i]->BuildTree();
	}	
}

void OctreeNode::GetNeighborsFromChildren(TArray<ABoid*>& neighbors) const
{
	std::stack<const OctreeNode*> dfsStack;

	dfsStack.push(this);

	while (!dfsStack.empty())
	{
		auto visited = dfsStack.top();
		dfsStack.pop();

		if (visited == nullptr) continue;
		//visit this node and take its boids as neighbors
		for (auto neighbor : visited->boids)
		{
			if(neighbor != nullptr) neighbors.Add(neighbor);
		}

		//push its children onto stack for visiting
		for (auto child : visited->child)
		{
			if(child != nullptr) dfsStack.push(child);
		}
	}
}

//bool OctreeNode::IsInside(const FBox& region, const FVector& vector) const
//{
//	return region.IsInsideOrOn(vector);
//}

OctreeNode::~OctreeNode()
{
}
