// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"

#define CHILDREN_CAPACITY 4
/**
 * 
 */
class FLOCKINGBEHAVIOR_API QuadTreeNode
{
public:
	QuadTreeNode(FBox region, QuadTreeNode* parent, TArray<ABoid*>& boids, int boidsCapacity);
	QuadTreeNode();
	~QuadTreeNode();

	void BuildTree();

	//TArray<QuadTreeNode*> child;
	QuadTreeNode* child[4];

	TArray<ABoid*> boids;
	QuadTreeNode* parent;

	//FVector origin;
	//FVector bounds;

	FBox region;

	int boidsCapacity;
	//non empty children; these are to be 
	BYTE childrenWithBoids;

	//void GetNeighbors(const ABoid& boid, const QuadTreeNode& currentNode, TArray<ABoid*>& neighbors, float awarenessRadius);

	void GetNeighborsFromChildren(TArray<ABoid*>& neighbors) const;

	bool IsInside(const FBox& region, const FVector& vector) const;
};
