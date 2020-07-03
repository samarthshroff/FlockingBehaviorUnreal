// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OctreeNode.h"

/**
 * 
 */
class FLOCKINGBEHAVIOR_API Octree
{
public:
	Octree(FBox region, TArray<ABoid*>& boids, int boidCapacity);
	~Octree();

	void BuildTree();

	OctreeNode* root;

	void GetNeighbors(FVector boidPosition, const OctreeNode* currentNode, TArray<ABoid*>& neighbors, float awarenessRadius);
	OctreeNode* GetTreeNode(FVector boidPosition);
	void UpdateBoidPositionInTree(ABoid* boid, OctreeNode* const node);
};
