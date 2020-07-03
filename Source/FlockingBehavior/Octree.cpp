// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree.h"

Octree::Octree(FBox region, TArray<ABoid*>& boids, int boidCapacity)
{
	root = new OctreeNode(region, nullptr, boids, boidCapacity);
}

void Octree::BuildTree()
{
	root->BuildTree();
}

void Octree::GetNeighbors(FVector boidPosition, const OctreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius)
{
	if (root == nullptr) return;

	//FVector boidPosition = boid.GetTransform().GetLocation();

	//traverse up until we reach a node that completely contains the radius of awareness of given boid
	const OctreeNode* current = currentNode;
	const OctreeNode* previous = nullptr;

	while (previous != current)
	{
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetNeighbors 000000"));
		if (current == nullptr) return;
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetNeighbors 111111"));
		FVector extent = current->region.GetExtent();
		FVector center = current->region.GetCenter();

		//if boid position is inside the box
		if (!current->region.IsInsideOrOn(boidPosition)) break;
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetNeighbors 22222"));

		//if boid is inside the awareness radius then break. we foind the tree node that contains this boid
		if ((boidPosition.X - awarenessRadius >= (center.X - extent.X)) &&
			(boidPosition.X + awarenessRadius <= (center.X + extent.X)) &&
			(boidPosition.Y - awarenessRadius >= (center.Y - extent.Y)) &&
			(boidPosition.Y + awarenessRadius <= (center.Y + extent.Y)) &&
			(boidPosition.Z - awarenessRadius >= (center.Z - extent.Z)) &&
			(boidPosition.Z + awarenessRadius <= (center.Z + extent.Z)))
		{
			break;
		}
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetNeighbors 33333"));
		previous = current;
		current = current->parent;
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("before calling GetNeighborsFromChildren"));
	//we now have the node which confines (boidPosition+awarenessRadius). All of its boid and boids from its children are neigbors
	current->GetNeighborsFromChildren(neighbors);
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after calling GetNeighborsFromChildren"));
}

OctreeNode* Octree::GetTreeNode(FVector boidPosition)
{
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetTreeNode 0000"));
	if (root == nullptr || !root->region.IsInsideOrOn(boidPosition))
	{
		return nullptr;
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetTreeNode 1111"));
	auto current = root;
	OctreeNode* previous = nullptr;
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetTreeNode 2222"));
	while (current != nullptr && current != previous)
	{
		previous = current;
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			if (current->child[i] == nullptr) continue;

			if (!current->child[i]->region.IsInsideOrOn(boidPosition))
			{
				continue;
			}
			current = current->child[i];
			break;
		}
	}
	//if(current == nullptr)
	//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetTreeNode current is null ------"));
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("GetTreeNode 3333"));
	return current;
}

void Octree::UpdateBoidPositionInTree(ABoid* boid, OctreeNode* const node)
{
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 00000"));

	//if(node == nullptr)
	//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree node is null"));

	auto previous = node->parent;
	auto current = node;
	FVector boidPosition = boid->GetTransform().GetLocation();
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 11111"));
	//go up in the tree and find the node which encloses the boid
	while (current != nullptr && !current->region.IsInsideOrOn(boidPosition))
	{
		previous = current;
		current = current->parent;
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 22222"));
	if (current == nullptr && previous == root) current = root;
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 33333"));
	//now as we have the node which encloses the boid go down the tree from there to find the actual smallest node which can
	//enclose the boid

	while (current != nullptr && current != previous)// && current->child.Num() > 0)
	{
		previous = current;
		//for (int i = 0; i < current->child.Num(); ++i)
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			if (current->child[i] == nullptr) continue;
			if (!current->child[i]->region.IsInsideOrOn(boidPosition)) continue;
			//{
				//previous = current;
			current = current->child[i];
			break;
			//}
		}
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 44444"));
	if (node != current)
	{
		node->boids.Remove(boid);
		current->boids.Add(boid);
		current->BuildTree();
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("UpdateBoidPositionInTree 55555"));
}

Octree::~Octree()
{
}
