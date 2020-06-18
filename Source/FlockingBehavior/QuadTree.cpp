// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree.h"

QuadTree::QuadTree(FBox region, TArray<ABoid*>& boids, int boidCapacity)
{
	root = new QuadTreeNode(region, nullptr, boids, boidCapacity);
}

void QuadTree::BuildTree()
{
	root->BuildTree();
}

//void QuadTree::GetNeighbors(const ABoid& boid, const QuadTreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius)
void QuadTree::GetNeighbors(FVector boidPosition, const QuadTreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius)
{
	if (root == nullptr) return;

	//FVector boidPosition = boid.GetTransform().GetLocation();

	//traverse up until we reach a node that completely contains the radius of awareness of given boid
	const QuadTreeNode* current = currentNode;
	const QuadTreeNode* previous = nullptr;

	while (previous != current)
	{
		if (current == nullptr) return;

		FVector extent = current->region.GetExtent();
		FVector center = current->region.GetCenter();

		//if boid position is inside the box
		if (!current->IsInside(current->region, boidPosition)) break;

		//if (current->IsInside(current->region, boidPosition))
		//{
		if ((boidPosition.X - awarenessRadius >= (center.X - extent.X)) &&
			(boidPosition.X + awarenessRadius <= (center.X + extent.X)) &&
			(boidPosition.Y - awarenessRadius >= (center.Y - extent.Y)) &&
			(boidPosition.Y + awarenessRadius <= (center.Y + extent.Y)))
		{
			break;
		}

		previous = current;
		current = current->parent;

		////then check if the circle formed by boidpoisition+awareness_radius is inside the box
		////if outside then this tree node does not confine the entire circle. goto it's parent
		//if ((boidPosition.X - awarenessRadius < (center.X - extent.X)) ||
		//	(boidPosition.X + awarenessRadius > (center.X + extent.X)) ||
		//	(boidPosition.Y - awarenessRadius < (center.Y - extent.Y)) ||
		//	(boidPosition.Y + awarenessRadius > (center.Y + extent.Y)))
		//{
		//	current = current->parent;
		//}
		//else
		//	break;
	//}
	}
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("before calling GetNeighborsFromChildren"));
	//we now have the node which confines (boidPosition+awarenessRadius). All of its boid and boids from its children are neigbors
	current->GetNeighborsFromChildren(neighbors);
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after calling GetNeighborsFromChildren"));

}

QuadTreeNode* QuadTree::GetTreeNode(FVector boidPosition)
{
	if (root == nullptr || !root->IsInside(root->region, boidPosition))
	{
		return nullptr;
	}

	auto current = root;
	QuadTreeNode* previous = nullptr;

	while (current != nullptr && current != previous)
	{
		previous = current;
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			if (current->child[i] == nullptr) continue;

			if (!current->child[i]->IsInside(current->child[i]->region, boidPosition))
			{
				continue;
			}
			current = current->child[i];
			break;
		}
	}

	return current;
}

void QuadTree::UpdateBoidPositionInTree(ABoid* boid, QuadTreeNode* const node)
{
	auto previous = node->parent;
	auto current = node;
	FVector boidPosition = boid->GetTransform().GetLocation();

	//go up in the tree and find the node which encloses the boid
	while (current != nullptr && !current->IsInside(current->region, boidPosition))
	{
		previous = current;
		current = current->parent;
	}
	if (current == nullptr && previous == root) current = root;
	//now as we have the node which encloses the boid go down the tree from there to find the actual smallest node which can
	//enclose the boid

	while (current != nullptr && current != previous)// && current->child.Num() > 0)
	{
		previous = current;
		//for (int i = 0; i < current->child.Num(); ++i)
		for (int i = 0; i < CHILDREN_CAPACITY; ++i)
		{
			if (current->child[i] == nullptr) continue;
			if (!current->child[i]->IsInside(current->child[i]->region, boidPosition)) continue;
			//{
				//previous = current;
			current = current->child[i];
			break;
			//}
		}
	}
	if (node != current)
	{
		node->boids.Remove(boid);
		current->boids.Add(boid);
		current->BuildTree();
	}
}

QuadTree::~QuadTree()
{
}
