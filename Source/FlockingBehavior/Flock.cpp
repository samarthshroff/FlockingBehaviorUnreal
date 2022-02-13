// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
<<<<<<< HEAD
=======
#include "Kismet/GameplayStatics.h"

//DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

#define AWARENESS_RADIUS 250.0f
#define SEPARATION_RADIUS 150.0f
>>>>>>> origin/simple-basic-algorithm

// Sets default values for this component's properties
UFlock::UFlock()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
<<<<<<< HEAD

	// ...
}


=======
}

>>>>>>> origin/simple-basic-algorithm
// Called when the game starts
void UFlock::BeginPlay()
{
	Super::BeginPlay();
<<<<<<< HEAD

	// ...

=======
>>>>>>> origin/simple-basic-algorithm
}

void UFlock::Initialize(class AActor* boidsOwner, TSubclassOf<class ABoid> boidBPClass)
{
	_boidOwner = boidsOwner;
	_boidBPClass = boidBPClass;

<<<<<<< HEAD
	_width = _height = _depth = _gridSize = 1;

=======
>>>>>>> origin/simple-basic-algorithm
	UWorld* world = GetWorld();

	if (world)
	{
<<<<<<< HEAD
		//this entire hard coding will be optimized
		FString name = "Boid";
		name.AppendInt(_boids.Num());

		FActorSpawnParameters params;
		params.Owner = _boidOwner;
		params.Name = FName(*name);

		FTransform transform = FTransform(FVector(-3160.0f, 0.0f, 250.0f));
		auto boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		boid->SetActorLabel(name);

		boid->DirectionAngle = 0.0f;

		AddBoidToArray(boid);

		name = "Boid";
		name.AppendInt(_boids.Num());
		params.Owner = _boidOwner;
		params.Name = FName(*name);

		transform = FTransform(FVector(-3160.0f, 400.0f, 250.0f));
		boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		boid->SetActorLabel(name);
		boid->DirectionAngle = -15.0f;

		AddBoidToArray(boid);

		name = "Boid";
		name.AppendInt(_boids.Num());
		params.Owner = _boidOwner;
		params.Name = FName(*name);

		transform = FTransform(FVector(-3160.0f, 200.0f, 250.0f));
		boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		boid->SetActorLabel(name);
		boid->DirectionAngle = 1.0f;

		AddBoidToArray(boid);

		name = "Boid";
		name.AppendInt(_boids.Num());
		params.Owner = _boidOwner;
		params.Name = FName(*name);

		transform = FTransform(FVector(-3160.0f, -200.0f, 250.0f));
		boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		boid->SetActorLabel(name);
		boid->DirectionAngle = -2.0f;

		AddBoidToArray(boid);

		name = "Boid";
		name.AppendInt(_boids.Num());
		params.Owner = _boidOwner;
		params.Name = FName(*name);

		transform = FTransform(FVector(-3280.0f, -200.0f, 250.0f));
		boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		boid->SetActorLabel(name);
		boid->DirectionAngle = 10.0f;

		AddBoidToArray(boid);
	}
=======
		// Create boids
		for (int i = 0; i < TotalBoids; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);

			FTransform transform = FTransform(FVector(FMath::FRandRange(-1000.0f, 1000.0f), FMath::FRandRange(-1000.0f, 1000.0f), FMath::FRandRange(100.0f, 1800.0f)));
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
>>>>>>> origin/simple-basic-algorithm
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
<<<<<<< HEAD
	if (_boids.Num() > 0)
	{
		//sort the boids here
		SortBoids();

		for(auto& boid : _boids)
		for (int i = 0; i < _boids.Num(); i++)
		{
			auto boid = _boids[i];
			//get the neigbors to a given boid and send a data structure of there transofrm, velocity and direction to the 
			//boid in focus
			FVector indices;
			Get3DIndicies(i, indices);

			float alignmentAngle = 0.0f;
			FVector separation = FVector::ZeroVector;
			FVector cohesion = FVector::ZeroVector;
			int neighborCount = 0;
			int separationCount = 0;

			//TArray<FNeighborData> neighborsData;

			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					for (int z = -1; z <= 1; z++)
					{
						//this is the location of boid whose neighbors we are finding
						if (x == y == z == 0) continue;

						FVector neighborIndices;

						neighborIndices.X = indices.X + x;
						neighborIndices.Y = indices.Y + y;
						neighborIndices.Z = indices.Z + z;

						int neighborIndex;
						GetFlatIndex(neighborIndices, neighborIndex);

						if (_boids.IsValidIndex(neighborIndex))
						{
							//check if this object is in vicinity and in vision
							auto neighbor = _boids[neighborIndex];

							//distance is sqrt((x2-x2)^2 + (y2-y1)^2 + (z2-z1)^2)
							float distance = FVector::Distance(boid->GetTransform().GetLocation(), neighbor->GetTransform().GetLocation());
							if (distance <= NeighborhoodRadius)
							{
								FVector boidDirectionVector = boid->GetDirectionVector();

								FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();

								//this is a valid neighbor
								if ( FVector::DotProduct(boidDirectionVector, subVector) >= FMath::Cos(FMath::DegreesToRadians(BoidFOV / 2.0f)))
								{

								}

							/*	//check if the neighbor is visible by the boid

								//FVector boidDirectionVector = boid->GetDirectionVector();

								//FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();

								//FVector normalizedSubVector = subVector.GetSafeNormal();

								////subVector.ToDirectionAndLength(normalizedSubVector, magnitude);

								//float angle = FMath::Acos(FVector::DotProduct(boidDirectionVector, normalizedSubVector));

								////this is a valid neighbor
								//if (angle <= BoidFOV / 2.0f)

								FVector boidDirectionVector = boid->GetDirectionVector();

								FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();

								//this is a valid neighbor
								if (FVector::DotProduct(boidDirectionVector, subVector) >= FMath::Cos(FMath::DegreesToRadians(BoidFOV / 2.0f)))
								{
									//Alignment
									alignmentAngle += neighbor->DirectionAngle;
									neighborCount++;

									//Cohesion
									cohesion += neighbor->GetTransform().GetLocation();

									//Separation
									if( distance > 0 && distance <= SeparationRadius)
									{
										FVector subtractionVector = boid->GetTransform().GetLocation() - neighbor->GetTransform().GetLocation();
										subtractionVector.Normalize();
										FVector individualSteeringForce = subtractionVector / distance;

										separation += individualSteeringForce;

										separationCount++;
									}


									// //NeighborData nd;
									// //nd.DirectionAngle = neighbor->DirectionAngle;
									// //nd.Distance = distance;
									// //nd.Location = neighbor->GetTransform().GetLocation();

									// //neighborsData.Add(nd);

									// neighborCount++;

									// //Alignment
									// alignmentAngle += neighbor->DirectionAngle;

									// cohesion += neighbor->GetTransform().GetLocation();

									// //Separation
									// if (distance <= SeparationRadius)
									// {
									// 	separationCount++;
									// 	//subVector = boid->GetTransform().GetLocation() - neighbor->GetTransform().GetLocation();
									// 	//subVector /= distance>2.0f?distance:2.0f;
									// 	//subVector.Normalize();
									// 	separation += subVector;
									// }
								}*/
							}
						}
					}
				}
			}

			// //
			/* if (neighborCount > 0)
			 {
			 	alignmentAngle /= neighborCount;

			 	cohesion /= neighborCount;
			 	cohesion = cohesion - boid->GetTransform().GetLocation();
			// 	if (cohesion.Size() >= 2.0f) cohesion.Normalize();// /= cohesion.Size();
			 }
			if (separationCount > 0)
			{				
				separation /= separationCount;
				// separation *= -1.0f;
				// separation.Normalize();
				//if(separation.Size() >= 2.0f) separation.Normalize();// /= separation.Size();
			}*/

			boid->Tick(DeltaTime, alignmentAngle, cohesion, separation);
		}
	}
}

void UFlock::AddBoidToArray(class ABoid* boid)
{
	if (_boids.Num() > 0)
	{
		FVector boidLocation = boid->GetTransform().GetLocation();

		if (!_boids.ContainsByPredicate([&](const ABoid* b)
			{    return b->GetTransform().GetLocation().X == boidLocation.X;   }))
		{
			_depth++;
		}

		if (!_boids.ContainsByPredicate([&](const ABoid* b)
			{    return b->GetTransform().GetLocation().Y == boidLocation.Y;   }))
		{
			_width++;
		}

		if (!_boids.ContainsByPredicate([&](const ABoid* b)
			{    return b->GetTransform().GetLocation().Z == boidLocation.Z;   }))
		{
			_height++;
		}

		_gridSize = _depth * _width * _height;//in the form of [x,y,z]
	}

	_boids.Add(boid);
}

void UFlock::Get3DIndicies(int flatIndex, FVector& indices)
{
	int depthIndex = flatIndex % _depth;

	//dividing with depth gives us on layer of the 3d array like getting one face/layer/2dmatrix from a cube
	//then find index as we find for a 2d array

	int heightIndex = (flatIndex / _depth) / _height;

	int widthIndex = (flatIndex / _depth) % _height;

	indices.X = depthIndex;
	indices.Y = widthIndex;
	indices.Z = heightIndex;
}

void UFlock::GetFlatIndex(FVector& indices, int& flatIndex)
{
	//this is derived from the 2d flat index formula which is => widthIndex + ( width * heightIndex)
	flatIndex = indices.Y + (_width * (indices.Z + (_height * indices.X)));
}

void UFlock::SortBoids()
{
	//sort entire array. At the end we will have an array sorted w.r.t. depth (X)
	_dimensionToCompare = Compare::Depth;
	QuickSort(0, _gridSize - 1);

	//sort one depth at a time w.r.t height so we will have each layer sorted w.r.t Z
	_dimensionToCompare = Compare::Height;
	for (int i = 0; i < _depth; ++i)
	{
		if (!_boids.IsValidIndex((i * _width * _height))) break;
		QuickSort((i * _width * _height), ((i + 1) * _width * _height) - 1);
	}

	//now that we have sorted the array w.r.t. X and Z sort each row which represents width (Y) 
	_dimensionToCompare = Compare::Width;
	for (int i = 0; i < (_depth * _height); ++i)
	{
		if (!_boids.IsValidIndex((i * _width))) break;
		QuickSort((i * _width), ((i + 1) * _width) - 1);
	}
}

void UFlock::QuickSort(int low, int high)
{
	//There can be a case where gridsize is more than total elements in array
	if (high > (_boids.Num() - 1)) high = _boids.Num() - 1;

	if (low < high)
	{
		int mid = Partition(low, high);

		QuickSort(low, mid - 1);
		QuickSort(mid + 1, high);
	}
}

int UFlock::Partition(int low, int high)
{
	ABoid* pivot = _boids[high];
	FVector pivotVector = pivot->GetTransform().GetLocation();

	int i = low - 1;
	for (int j = low; j < high; ++j)
	{
		FVector lhsBoidVector = _boids[j]->GetTransform().GetLocation();

		if ((_dimensionToCompare == Compare::Depth && lhsBoidVector.X >= pivotVector.X) ||
			(_dimensionToCompare == Compare::Width && lhsBoidVector.Y <= pivotVector.Y) ||
			(_dimensionToCompare == Compare::Height && lhsBoidVector.Z >= pivotVector.Z))
		{
			++i;
			_boids.SwapMemory(i, j);
		}
	}

	_boids.SwapMemory(i + 1, high);
	return (i + 1);
}
=======
	for (ABoid* boid : _boids)
	{
		auto boidPosition = boid->GetTransform().GetLocation();

		FVector alignment = FVector::ZeroVector;
		FVector separation = FVector::ZeroVector;
		FVector cohesion = FVector::ZeroVector;
		int neighborCount = 0;
		int separationCount = 0;

		for (ABoid* neighbor : _boids)
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
>>>>>>> origin/simple-basic-algorithm
