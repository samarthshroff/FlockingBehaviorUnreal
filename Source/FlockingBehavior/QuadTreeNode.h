// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"

/**
 * 
 */
class FLOCKINGBEHAVIOR_API QuadTreeNode
{
public:
	QuadTreeNode(FBox region, QuadTreeNode* parent, TArray<ABoid*>& boids);
	QuadTreeNode();
	~QuadTreeNode();

	void BuildTree();

	TArray<QuadTreeNode*> child;
	TArray<ABoid*> boids;
	QuadTreeNode* parent;

	//FVector origin;
	//FVector bounds;

	FBox region;

	//non empty children; these are to be 
	BYTE childrenWithBoids;

	void GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors);

	void GetNeighborsFromChildren(const ABoid& boid, TArray<ABoid*>& neighbors) const;

	bool IsInside(const FBox& region, const FVector& vector);
};
