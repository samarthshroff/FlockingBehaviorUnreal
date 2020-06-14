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
	QuadTree(FBox region, TArray<ABoid*>& boids);
	~QuadTree();

	void BuildTree();

	QuadTreeNode* root;

	void GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors);
	QuadTreeNode* GetTreeNode(ABoid& boid);
	void UpdateBoidPositionInTree(ABoid& boid, QuadTreeNode& node);
	//TArray<ABoid*> boids;


};
