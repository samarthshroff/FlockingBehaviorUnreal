// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Kismet/GameplayStatics.h"

//DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

#define AWARENESS_RADIUS	200.0f
#define ALIGNMENT_RADIUS	200.0f
#define COHESION_RADIUS		200.0f
#define SEPARATION_RADIUS	150.0f

// Sets default values for this component's properties
UFlock::UFlock()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFlock::BeginPlay()
{
	Super::BeginPlay();
}

void UFlock::Initialize(class AActor* boidsOwner, TSubclassOf<class ABoid> boidBPClass)
{
	_boidOwner = boidsOwner;
	_boidBPClass = boidBPClass;

	_width = _height = _depth = _gridSize = 1;

	UWorld* world = GetWorld();

	if (world)
	{
		//get the boid bounds and destroy this actor. Separation radius should not be less than the boid's bounds.
		auto tempBoid = world->SpawnActor<ABoid>(_boidBPClass, FTransform(FVector::ZeroVector));

		FVector origin;
		FVector boidBounds;
		tempBoid->GetActorBounds(true, origin, boidBounds);
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("boidBounds is: X %f Y %f Z %f"), boidBounds.X, boidBounds.Y, boidBounds.Z);

		tempBoid->Destroy();
		auto maxComponent = boidBounds.GetMax();
		SeparationRadius = FMath::Max(SeparationRadius, maxComponent * 1.5f);

		for (int i = 0; i < 500; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);

			FTransform transform = FTransform(FVector(FMath::FRandRange(-1900.0f, 1900.0f), FMath::FRandRange(-1900.0f, 1900.0f), FMath::FRandRange(100.0f, 1800.0f)));
			//need to defer the spawning as we need to set the direction angle before boids BeginPlay is called
			auto boid = world->SpawnActorDeferred<ABoid>(_boidBPClass, transform, _boidOwner);
			boid->SetActorLabel(name);
			//boid->DirectionAngle = FMath::RandRange(-40.0f, 40.0f);// (-1, 1)* (i + 1) * 10.0f;

			UGameplayStatics::FinishSpawningActor(boid, transform);

			_boids.Add(boid);
		}

		tree = new Octree(FBox(FVector(-2000.0f, -2000.0f, 0.0f), FVector(2000.0f, 2000.0f, 2000.0f)), _boids, 200);
		tree->BuildTree();
	}

	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_width %d _height %d _depth %d"), _width, _height, _depth);
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
	//int numberOfBoids = _boids.Num();

	//if (numberOfBoids >= 1)
	//{
		for (ABoid* boid : _boids)
		{
			//ABoid& boidRef = *boid;
			auto boidPosition = boid->GetTransform().GetLocation();
			auto name = boid->GetName();
			TArray<ABoid*> neighbors;
			auto currentNode = tree->GetTreeNode(boidPosition);// boidRef);
			//if(currentNode == nullptr)
			//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after GetTreeNode for boid:: %s. Current Node is NULL *******************"), *(name));
			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after GetTreeNode for boid:: %s"), *(name));

			tree->GetNeighbors(boidPosition, currentNode, neighbors, AWARENESS_RADIUS);

			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after get neighbors for boid:: %s"), *(name));

			FVector alignment = FVector::ZeroVector;
			FVector separation = FVector::ZeroVector;
			FVector cohesion = FVector::ZeroVector;
			int alignmentCount = 0;
			int cohesionCount = 0;
			int separationCount = 0;

			for (auto neighbor : neighbors)// int neighborIndex = 0; neighborIndex < neighbors.Num(); ++neighborIndex)
			{
				if (boid != neighbor)
				{
					//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);

					auto neighborPosition = neighbor->GetTransform().GetLocation();

					float distance = FVector::Distance(boidPosition, neighborPosition);

					//check if the neighbor is visible by the boid
					float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(boid->GetActorForwardVector(), (neighborPosition - boidPosition).GetSafeNormal())));

					//this is a valid neighbor
					//if (FVector::DotProduct(boidDirectionVector, subVector) >= FMath::Cos(FMath::DegreesToRadians(BoidFOV/2.0f)))
					if (angle <= BoidFOV && distance <= AWARENESS_RADIUS)
					{
						if (distance <= ALIGNMENT_RADIUS)
						{
							//alignment
							alignment += neighbor->GetVelocity();
							alignmentCount++;
						}

						if (distance <= COHESION_RADIUS)
						{
							//cohesion
							cohesion += neighborPosition;
							cohesionCount++;
						}

						if (distance <= SEPARATION_RADIUS)
						{
							//separation
							//subtraction vector from boid to neighbor / distance = value by which boid has to separate from this neighbor
							FVector separationSubVector = boidPosition - neighborPosition;
							separationSubVector.Normalize();
							separationSubVector /= distance;
							separation += separationSubVector;
							separationCount++;
						}
					}
				}
			}

			if (alignmentCount > 0)
			{
				alignment /= alignmentCount;
			}
			if (cohesionCount > 0)
			{
				cohesion /= cohesionCount;
			}
			if (separationCount > 0)
			{
				separation /= separationCount;
			}

			boid->Tick(DeltaTime, alignment, cohesion, separation);

			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after tick of boid:: %s"), *(name));
			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("before UpdateBoidPositionInTree for boid:: %s"), *(name));
			tree->UpdateBoidPositionInTree(boid, currentNode);
			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("after UpdateBoidPositionInTree for boid:: %s"), *(name));
		}
	//}
	//else if (numberOfBoids == 1)
	//{
	//	_boids[0]->Tick(DeltaTime);
	//}
}