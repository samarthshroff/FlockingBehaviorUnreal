// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeNode.h"
#include <stack>

QuadTreeNode::QuadTreeNode()
{
	this->parent = nullptr;
	childrenWithBoids = 0;
}

QuadTreeNode::QuadTreeNode(FBox region, QuadTreeNode* parent, TArray<ABoid*>& boids, int boidsCapacity)
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

void QuadTreeNode::BuildTree()
{
	//if true then this is the leaf node which will hold not more than 100 boids in it's space
	if (boids.Num() <= boidsCapacity)
	{
		return;
	}

	//FVector dimensions = region.GetSize();

	////200.0 is the neighboring radius for boids
	////if true then this is the leaf node which will hold the boids wihtin it's region
	//if (dimensions.X <= 1000.0f && dimensions.Y <= 1000.0f)// && dimensions.Z <= 200.0f)
	//{
	//	//no more recursion
	//	return;
	//}

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

	FBox quad[CHILDREN_CAPACITY];
	quad[0] = FBox( FVector(region.Min.X,region.Min.Y,250.0f), FVector(center.X, center.Y,250.0f));
	quad[1] = FBox(FVector(center.X,region.Min.Y,250.0f), FVector(region.Max.X,center.Y,250.0f));
	quad[2] = FBox(FVector(center.X,center.Y,250.0f), FVector(region.Max.X,region.Max.Y,250.0f));
	quad[3] = FBox(FVector(region.Min.X, center.Y,250.0f), FVector(center.X,region.Max.Y,250.0f));

	//list of boids distributed amongst the 8 children
	//TArray<ABoid*> boidDistribution[8];
	TArray<ABoid*> boidDistribution[CHILDREN_CAPACITY];
	//contains boids moved down to children and which can thus be safely removed fro this node
	TArray<ABoid*> boidsMovedToChildren;

	//distrubute the boids
	for (auto boid : boids)
	{		
		FVector boidPosition = boid->GetTransform().GetLocation();

		//for (int i = 0; i < 8; ++i)
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
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
	for (auto boid : boidsMovedToChildren)
		boids.Remove(boid);

	//distribute the boids amongst children
	//if (child.Num() == 0)
	{
		//for (int i = 0; i < 8; ++i)
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			//child.Add(new OctreeNode(octant[i], this, boidDistribution[i]));
			if (child[i] == nullptr)
			{
				child[i] = new QuadTreeNode(quad[i], this, boidDistribution[i], boidsCapacity);
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
}

//void QuadTreeNode::GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors, float awarenessRadius)
//{
//	FVector boidPosition = boid.GetTransform().GetLocation();
//
//	//traverse up until we reach a node that completely contains the radius of awareness of given boid
//	const QuadTreeNode* current = &currentNode;
//	const QuadTreeNode* previous = &currentNode;
//
//	while (true)
//	{
//		if (current == nullptr) break;
//		
//		FVector extent = current->region.GetExtent();
//		FVector center = current->region.GetCenter();
//		
//		//if boid position is inside the box
//		if (current->IsInside(current->region, boidPosition))
//		{
//			//then check if the circle formed by boidpoisition+awareness_radius is inside the box
//			//if outside then this tree node does not confine the entire circle. goto it's parent
//			if ((boidPosition.X - awarenessRadius < (center.X - extent.X)) ||
//				(boidPosition.X + awarenessRadius > (center.X + extent.X)) ||
//				(boidPosition.Y - awarenessRadius < (center.Y - extent.Y)) ||
//				(boidPosition.Y + awarenessRadius > (center.Y + extent.Y)))
//			{
//				previous = current;
//				current = current->parent;
//			}
//			else
//				break;
//		}
//	}
//	if (current == nullptr) current = previous;
//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("before calling GetNeighborsFromChildren"));
//	//we now have the node which confines (boidPosition+awarenessRadius). All of its boid and boids from its children are neigbors
//	current->GetNeighborsFromChildren(boid, neighbors);
//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after calling GetNeighborsFromChildren"));
//	
//	////add all other boids from current node
//	//if (boids.Num() > 1)
//	//{
//	//	for (auto& neighbor : boids)
//	//	{
//	//		if(&boid != neighbor)//boid.GetTransform().GetLocation() != neighbor->GetTransform().GetLocation())
//	//			neighbors.Add(neighbor);
//	//	}
//	//}
//
//	//add all boids from sibling nodes
//	//auto parent = currentNode.parent;
//
//	//for (int i = 0; i < parent->child.Num(); ++i)
//	//{
//	//	auto sibling = parent->child[i];
//
//	//	if (&currentNode != sibling)
//	//	{
//	//		for (auto& neighbor : sibling->boids)
//	//		{
//	//			neighbors.Add(neighbor);
//	//		}
//	//	}
//	//}
//
//	////traverse up till root and add boids from all parents (potential neighbors)
//	//while (parent != nullptr)
//	//{
//	//	for (auto& neighbor : parent->boids)
//	//	{
//	//		neighbors.Add(neighbor);
//	//	}
//	//	parent = parent->parent;
//	//}
//
//	////traverse down and add boids as neighbors as boids of children can be potential neighbors too
//	//currentNode.GetNeighborsFromChildren(boid, neighbors);
//
//	////if (region.IsInside(boidPosition))
//	//if (IsInside(region, boidPosition))
//	//{
//	//	if (boids.Num() > 0)
//	//	{
//	//		for (auto& neighbor : boids)
//	//		{
//	//			if(&boid != neighbor )//boid.GetTransform().GetLocation() != neighbor->GetTransform().GetLocation())
//	//				neighbors.Add(neighbor);
//	//		}
//	//	}
//	//	//check if any of the children enclose the given boid. the one which does enclose call it's
//	//	//get neighbors if it has any other boids.
//	//	if (child.Num() > 0)
//	//	{
//	//		for (int i = 0; i < child.Num(); ++i)
//	//		{
//	//			//if (child[i]->boids.Num() > 0 && child[i]->region.IsInside(boidPosition))
//	//			if (IsInside(child[i]->region, boidPosition))
//	//			{
//	//				child[i]->GetNeighborsAndCurrentNode(boid, neighbors, currentNode);
//	//			}
//	//		}
//	//	}
//	//}
//	//if (&currentNode == this)
//	//{
//	//	//if this is the leaf
//	//}
//}

void QuadTreeNode::GetNeighborsFromChildren(TArray<ABoid*>& neighbors) const
{
	std::stack<const QuadTreeNode*> dfsStack;

	dfsStack.push(this);

	while (!dfsStack.empty())
	{
		auto visited = dfsStack.top();
		dfsStack.pop();

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

bool QuadTreeNode::IsInside(const FBox& region, const FVector& vector) const
{
	return ((vector.X >= region.Min.X) && (vector.X <= region.Max.X) && (vector.Y >= region.Min.Y) && (vector.Y <= region.Max.Y));
}

QuadTreeNode::~QuadTreeNode()
{
}
