// Fill out your copyright notice in the Description page of Project Settings.

#include "Flock.h"
#include "Kismet/GameplayStatics.h"

// DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

#define AWARENESS_RADIUS 200.0f
#define SEPARATION_RADIUS 150.0f

// Sets default values for this component's properties
UFlock::UFlock()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UFlock::BeginPlay()
{
	Super::BeginPlay();
}

void UFlock::Initialize(class AActor *boidsOwner, TSubclassOf<class ABoid> boidBPClass)
{
	_boidOwner = boidsOwner;
	_boidBPClass = boidBPClass;

	UWorld *world = GetWorld();

	if (world)
	{
		// Create boids
		for (int i = 0; i < TotalBoids; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);

			FTransform transform = FTransform(FVector(FMath::FRandRange(-1900.0f, 1900.0f), FMath::FRandRange(-1900.0f, 1900.0f), FMath::FRandRange(100.0f, 1800.0f)));
			// need to defer the spawning as we need to set the direction angle before boids BeginPlay is called
			auto boid = world->SpawnActorDeferred<ABoid>(_boidBPClass, transform, _boidOwner);
			boid->SetActorLabel(name);
			// boid->DirectionAngle = FMath::RandRange(-40.0f, 40.0f);// (-1, 1)* (i + 1) * 10.0f;

			UGameplayStatics::FinishSpawningActor(boid, transform);

			_boids.Add(boid);
		}
	}

	// Calculation of constants for future use in the game loop.
	_awarenessRadiusSquared = AWARENESS_RADIUS * AWARENESS_RADIUS;
	_separationRadiusSquared = SEPARATION_RADIUS * SEPARATION_RADIUS;
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
	for (ABoid *boid : _boids)
	{
		auto boidPosition = boid->GetTransform().GetLocation();

		FVector alignment = FVector::ZeroVector;
		FVector separation = FVector::ZeroVector;
		FVector cohesion = FVector::ZeroVector;
		int neighborCount = 0;
		int separationCount = 0;

		for (ABoid *neighbor : _boids)
		{
			if (boid == neighbor)
				continue;

			auto neighborPosition = neighbor->GetTransform().GetLocation();

			float distSquared = FVector::DistSquared(boidPosition, neighborPosition);

			if (distSquared < _awarenessRadiusSquared)
			{
				// Get the heading or forward vector of the neighbor
				alignment += neighbor->GetActorForwardVector();

				// Cohesion is to adjust the position of this boid wrt neighbors.
				// That is averaging to the neighbors positions.
				cohesion += neighborPosition;
				neighborCount++;

				if (distSquared < _separationRadiusSquared)
				{
					// subtraction vector from neighbor to boid / distance = value by which boid has to separate from this neighbor
					separation -= (neighborPosition - boidPosition) / distSquared;
					separationCount++;
				}
			}
		}

		if (neighborCount > 0)
		{
			alignment /= neighborCount;
			cohesion /= neighborCount;
		}
		if (separationCount > 0)
		{
			separation /= separationCount;
		}

		boid->Tick(DeltaTime, alignment, cohesion, separation);
	}
}
