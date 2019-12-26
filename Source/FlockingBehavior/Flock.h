// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Boid.h"
#include "Flock.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLOCKINGBEHAVIOR_API UFlock : public UActorComponent
{
	GENERATED_BODY()

public:
	//All other boids within this radius are the neighbors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float NeighborhoodRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock",meta=(Tooltip="Other boids which fall inside this FOV are neighbors. FOV is in degrees."))
	float BoidFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float SeparationRadius;

private:

	//Taking 3 different variables so that it is easy to understand even though all 3 are going to increase uniformly
	//initially it will start with 1 then 2 then 3 and so on. So the actual array size (TArray.Num) will be 1 then 8 then 27 and so on
	int _width, _height, _depth;

	//this is nothing but _dimensionLength^3
	int _gridSize;

	UPROPERTY()
		AActor* _boidOwner;

	UPROPERTY()
		TArray<ABoid*> _boids;

	UPROPERTY()
		TSubclassOf<class ABoid> _boidBPClass;

	enum class Compare
	{
		Depth,
		Width,
		Height
	};

	Compare _dimensionToCompare;

public:	
	// Sets default values for this component's properties
	UFlock();

	UFUNCTION()
	void Initialize(class AActor* boidsOwner, TSubclassOf<class ABoid> boidBPClass);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	void TickComponent(float DeltaTime);

private:
	void AddBoidToArray(class ABoid* boid);

	void Get3DIndicies(int flatIndex, FVector& indices);

	void GetFlatIndex(FVector& indices, int& flatIndex);

	void SortBoids();

	void QuickSort(int low, int high);

	int Partition(int low, int high);
};
