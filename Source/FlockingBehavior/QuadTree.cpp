// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTree.h"

QuadTree::QuadTree(FBox region, TArray<ABoid*>& boids)
{
	root = new QuadTreeNode(region, nullptr, boids);
}

void QuadTree::BuildTree()
{
	root->BuildTree();
}

void QuadTree::GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors)
{
	if (root != nullptr)
	{
		root->GetNeighbors(boid, currentNode, neighbors);
	}
}

QuadTreeNode* QuadTree::GetTreeNode(ABoid& boid)
{
	FVector boidPosition = boid.GetTransform().GetLocation();

	//if (root == nullptr || !root->region.IsInside(boidPosition))
	if (root == nullptr || !root->IsInside(root->region, boidPosition))
	{
		return nullptr;
	}

	auto current = root;

	while (current != nullptr && current->child.Num() > 0)
	{
		bool isBoidInsideChildNode = false;
		for (int i = 0; i < current->child.Num(); ++i)
		{
			if (current->child[i]->IsInside(current->child[i]->region, boidPosition))
			{
				isBoidInsideChildNode = true;
				current = current->child[i];
				break;
			}
		}
		if (!isBoidInsideChildNode) break;
	}

	return current;
}

void QuadTree::UpdateBoidPositionInTree(ABoid& boid, QuadTreeNode& node)
{
	auto previous = node.parent;
	auto current = node.parent;
	FVector boidPosition = boid.GetTransform().GetLocation();

	//go up in the tree and find the node which encloses the boid
	while (current != nullptr && !current->IsInside(current->region, boidPosition))
	{
		previous = current;
		current = current->parent;
	}
	if (current == nullptr && previous == root) current = root;
	//now as we have the node which encloses the boid go down the tree from there to find the actual smallest node which can
	//enclose the boid

	while (current != nullptr && current->child.Num() > 0)
	{
		for (int i = 0; i < current->child.Num(); ++i)
		{
			if (current->child[i]->IsInside(current->child[i]->region, boidPosition))
			{
				previous = current;
				current = current->child[i];
				break;
			}
		}
	}

	node.boids.Remove(&boid);
	current->boids.Add(&boid);
}

QuadTree::~QuadTree()
{
}
