// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadTreeNode.h"

/**
 * 
 */
class FLOCKINGBEHAVIOR_API QuadTree
{
public:
	QuadTree(FBox region, TArray<ABoid*>& boids, int boidCapacity);
	~QuadTree();

	void BuildTree();

	QuadTreeNode* root;

	//void GetNeighbors(const ABoid& boid, const QuadTreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius);
	void GetNeighbors(FVector boidPosition, const QuadTreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius);
	QuadTreeNode* GetTreeNode(FVector boidPosition);// ABoid& boid);
	void UpdateBoidPositionInTree(ABoid* boid, QuadTreeNode* const node);
	//TArray<ABoid*> boids;


};
