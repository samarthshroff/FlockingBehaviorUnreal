// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeNode.h"

QuadTreeNode::QuadTreeNode()
{
	this->parent = nullptr;
	childrenWithBoids = 0;
}

QuadTreeNode::QuadTreeNode(FBox region, QuadTreeNode* parent, TArray<ABoid*>& boids)
{
	this->region = region;
	this->parent = parent;
	this->boids = boids;
	childrenWithBoids = 0;
}

void QuadTreeNode::BuildTree()
{
	FVector dimensions = region.GetSize();

	//200.0 is the neighboring radius for boids
	//if true then this is the leaf node which will hold the boids wihtin it's region
	if (dimensions.X <= 1000.0f && dimensions.Y <= 1000.0f)// && dimensions.Z <= 200.0f)
	{
		//no more recursion
		return;
	}

	FVector center = region.GetCenter();

	//if the dimensions of this node is greater then the min required then this is not a leaf node; subdivide
	//FBox octant[8];
	//octant[0] = FBox(region.Min, center);
	//octant[1] = FBox(FVector(center.X, region.Min.Y, region.Min.Z), FVector(region.Max.X, center.Y, center.Z));
	//octant[2] = FBox(FVector(center.X, region.Min.Y, center.Z), FVector(region.Max.X, center.Y, region.Max.Z));
	//octant[3] = FBox(FVector(region.Min.X, region.Min.Y, center.Z), FVector(center.X, center.Y, region.Max.Z));
	//octant[4] = FBox(FVector(region.Min.X, center.Y, region.Min.Z), FVector(center.X, region.Max.Y, center.Z));
	//octant[5] = FBox(FVector(center.X, center.Y, region.Min.Z), FVector(region.Max.X, region.Max.Y, center.Z));
	//octant[6] = FBox(center, region.Max);
	//octant[7] = FBox(FVector(region.Min.X, center.Y, center.Z), FVector(center.X, region.Max.Y, region.Max.Z));

	FBox quad[4];
	quad[0] = FBox( FVector(region.Min.X,region.Min.Y,250.0f), FVector(center.X, center.Y,250.0f));
	quad[1] = FBox(FVector(center.X,region.Min.Y,250.0f), FVector(region.Max.X,center.Y,250.0f));
	quad[2] = FBox(FVector(center.X,center.Y,250.0f), FVector(region.Max.X,region.Max.Y,250.0f));
	quad[3] = FBox(FVector(region.Min.X, center.Y,250.0f), FVector(center.X,region.Max.Y,250.0f));

	//list of boids distributed amongst the 8 children
	//TArray<ABoid*> boidDistribution[8];
	TArray<ABoid*> boidDistribution[4];
	//contains boids moved down to children and which can thus be safely removed fro this node
	TArray<ABoid*> boidsMovedToChildren;

	//distrubute the boids
	for (auto& boid : boids)
	{		
		FVector boidPosition = boid->GetTransform().GetLocation();

		//for (int i = 0; i < 8; ++i)
		for (int i = 0; i < 4; ++i)
		{
			//if (octant[i].IsInside(boidPosition))
			if (IsInside(quad[i],boidPosition))
			{
				boidDistribution[i].Add(boid);
				boidsMovedToChildren.Add(boid);
			}
		}
	}

	//remove boidsMovedToChildren from this node
	for (auto& boid : boidsMovedToChildren)
		boids.Remove(boid);

	//distribute the boids amongst children
	if (child.Num() == 0)
	{
		//for (int i = 0; i < 8; ++i)
		for (int i = 0; i < 4; ++i)
		{
			//child.Add(new OctreeNode(octant[i], this, boidDistribution[i]));
			child.Add(new QuadTreeNode(quad[i], this, boidDistribution[i]));
			childrenWithBoids |= (1 << i);
			child[i]->BuildTree();
		}
	}
}

void QuadTreeNode::GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors)
{
	FVector boidPosition = boid.GetTransform().GetLocation();
	
	////add all other boids from current node
	//if (boids.Num() > 1)
	//{
	//	for (auto& neighbor : boids)
	//	{
	//		if(&boid != neighbor)//boid.GetTransform().GetLocation() != neighbor->GetTransform().GetLocation())
	//			neighbors.Add(neighbor);
	//	}
	//}

	//add all boids from sibling nodes
	auto parent = currentNode.parent;

	for (int i = 0; i < parent->child.Num(); ++i)
	{
		auto sibling = parent->child[i];

		if (&currentNode != sibling)
		{
			for (auto& neighbor : sibling->boids)
			{
				neighbors.Add(neighbor);
			}
		}
	}

	//traverse up till root and add boids from all parents (potential neighbors)
	while (parent != nullptr)
	{
		for (auto& neighbor : parent->boids)
		{
			neighbors.Add(neighbor);
		}
		parent = parent->parent;
	}

	//traverse down and add boids as neighbors as boids of children can be potential neighbors too
	currentNode.GetNeighborsFromChildren(boid, neighbors);

	////if (region.IsInside(boidPosition))
	//if (IsInside(region, boidPosition))
	//{
	//	if (boids.Num() > 0)
	//	{
	//		for (auto& neighbor : boids)
	//		{
	//			if(&boid != neighbor )//boid.GetTransform().GetLocation() != neighbor->GetTransform().GetLocation())
	//				neighbors.Add(neighbor);
	//		}
	//	}
	//	//check if any of the children enclose the given boid. the one which does enclose call it's
	//	//get neighbors if it has any other boids.
	//	if (child.Num() > 0)
	//	{
	//		for (int i = 0; i < child.Num(); ++i)
	//		{
	//			//if (child[i]->boids.Num() > 0 && child[i]->region.IsInside(boidPosition))
	//			if (IsInside(child[i]->region, boidPosition))
	//			{
	//				child[i]->GetNeighborsAndCurrentNode(boid, neighbors, currentNode);
	//			}
	//		}
	//	}
	//}
	//if (&currentNode == this)
	//{
	//	//if this is the leaf
	//}
}

void QuadTreeNode::GetNeighborsFromChildren(const ABoid& boid, TArray<ABoid*>& neighbors) const
{
	for( auto& child : child)
	{
		child->GetNeighborsFromChildren(boid, neighbors);
	}

	for (auto& neighbor : boids)
	{
		if (&boid != neighbor) neighbors.Add(neighbor);
	}
}

bool QuadTreeNode::IsInside(const FBox& region, const FVector& vector)
{
	return ((vector.X >= region.Min.X) && (vector.X <= region.Max.X) && (vector.Y >= region.Min.Y) && (vector.Y <= region.Max.Y));
}

QuadTreeNode::~QuadTreeNode()
{
}
