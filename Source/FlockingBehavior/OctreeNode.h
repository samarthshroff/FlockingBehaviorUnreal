// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boid.h"

#define CHILDREN_CAPACITY 8
/**
 * 
 */
class FLOCKINGBEHAVIOR_API OctreeNode
{
public:
	OctreeNode(FBox region, OctreeNode* parent, TArray<ABoid*>& boids, int boidsCapacity);
	OctreeNode();
	~OctreeNode();

	void BuildTree();

	//TArray<OctreeNode*> child;
	OctreeNode* child[CHILDREN_CAPACITY];

	TArray<ABoid*> boids;
	OctreeNode* parent;

	//FVector origin;
	//FVector bounds;

	FBox region;

	int boidsCapacity;
	//non empty children; these are to be 
	BYTE childrenWithBoids;

	void GetNeighborsFromChildren(TArray<ABoid*>& neighbors) const;

	//bool IsInside(const FBox& region, const FVector& vector) const;
};
