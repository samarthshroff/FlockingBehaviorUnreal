// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Kismet/GameplayStatics.h"

//DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

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
		/*	FlockingBehaviorLogs: Warning: The boid at: 0 is at location x: -2171.822266, y:  2120.749512, z: 250.000000
			FlockingBehaviorLogs: Warning: The boid at: 1 is at location x: -2259.324951, y:  2282.875977, z: 250.000000
			FlockingBehaviorLogs: Warning: The boid at: 2 is at location x:  1757.423828, y: -1307.333496, z: 250.000000
			FlockingBehaviorLogs: Warning: The boid at: 3 is at location x: -189.852539, y:  -2804.278809, z: 250.000000
			FlockingBehaviorLogs: Warning: The boid at: 4 is at location x:  2473.338867, y: -3065.211914, z: 250.000000*/

		auto x = new float[5] { -2171.822266, -2259.324951,  1757.423828, -189.852539,   2473.338867};
		auto y = new float[5] {  2120.749512,  2282.875977, -1307.333496, -2804.278809, -3065.211914 };

		for (int i = 0; i < 5; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);
			//
			FTransform transform = FTransform(FVector(x[i], y[i], 250.0f));
			//FTransform transform = FTransform(FVector(FMath::FRandRange(-3100.0f, 2500.0f), FMath::FRandRange(-3800.0f, 3800.0f), 250.0f));
			//FTransform transform = FTransform(FVector(FMath::FRandRange(-3160.0f, -2160.0f), FMath::FRandRange(-400.0f,350.0f), 250.0f));
			//need to defer the spawning as we need to set the direction angle before boids BeginPlay is called
			auto boid = world->SpawnActorDeferred<ABoid>(_boidBPClass, transform, _boidOwner);
			boid->SetActorLabel(name);
			//boid->DirectionAngle = FMath::RandRange(-40.0f, 40.0f);// (-1, 1)* (i + 1) * 10.0f;

			UGameplayStatics::FinishSpawningActor(boid, transform);

			AddBoidToArray(boid);

			UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);
		}
		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_width %d _height %d _depth %d"), _width, _height, _depth);

		SortBoids();

		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("After sort"));
		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_width %d _height %d _depth %d"), _width, _height, _depth);

		for (int i = 0; i < 5; i++)
		{
			auto boid = _boids[i];

			UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);
		}
	}

	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_width %d _height %d _depth %d"), _width, _height, _depth);
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
	int numberOfBoids = _boids.Num();
	if (numberOfBoids > 0)
	{
		if (numberOfBoids == 1)
		{
			_boids[0]->Tick(DeltaTime);
		}
		else
		{
			//sort the boids here
			SortBoids();

			_width = _ySet.Num();
			_depth = _xSet.Num();
			_height = _zSet.Num();

			//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_width %d, _height %d, _depth %d"), _width, _height, _depth);

			_xSet.Empty();
			_ySet.Empty();
			_zSet.Empty();

			//for(auto& boid : _boids)
			for (int i = 0; i < numberOfBoids; i++)
			{
				auto boid = _boids[i];

				//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);

				//get the neigbors to a given boid and send a data structure of there transofrm, velocity and direction to the 
				//boid in focus
				FVector indices;
				Get3DIndicies(i, indices);

				//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("indices.X %f, indices.Y %f, indices.Z %f and i %d"), indices.X, indices.Y, indices.Z, i);

				FVector alignment = FVector::ZeroVector;
				FVector separation = FVector::ZeroVector;
				FVector cohesion = FVector::ZeroVector;
				int alignmentCount = 0;
				int cohesionCount = 0;
				int separationCount = 0;

				int maxLimit = 4;
				int minLimit = -1* maxLimit;

				for (int x = minLimit; x <= maxLimit; x++)
				{
					for (int y = minLimit; y <= maxLimit; y++)
					{
						for (int z = minLimit; z <= maxLimit; z++)
						{
							if (indices.Z == 0)
							{
								if (_height == 1 && z != 0) continue;
								if (_height > 1 && z < 0) continue;
							}
							else if (indices.Z == _height - 1)
							{
								if (z >= 1) continue;
							}

							if (indices.Y == 0)
							{
								if (_width == 1 && y != 0) continue;
								if (_width > 1 && y < 0) continue;
							}
							else if (indices.Y == _width - 1)
							{
								if (y >= 1) continue;
							}

							if (indices.X == 0)
							{
								if (_depth == 1 && x != 0) continue;
								if (_depth > 1 && x < 0) continue;
							}
							else if (indices.X == _depth - 1)
							{
								if (x >= 1) continue;
							}

							FVector neighborIndices;

							neighborIndices.X = indices.X + x;
							neighborIndices.Y = indices.Y + y;
							neighborIndices.Z = indices.Z + z;

							if (neighborIndices.X < 0.0f || neighborIndices.Y < 0.0f || neighborIndices.Z < 0.0f) continue;

							int neighborIndex;
							GetFlatIndex(neighborIndices, neighborIndex);

							//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("neighborIndices.X %f, neighborIndices.Y %f, neighborIndices.Z %f and neighborIndex %d"), neighborIndices.X, neighborIndices.Y, neighborIndices.Z, neighborIndex);

							if (_boids.IsValidIndex(neighborIndex) && neighborIndex != i)
							{
								//check if this object is in vicinity and in vision
								auto neighbor = _boids[neighborIndex];

								//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);

								//distance is sqrt((x2-x2)^2 + (y2-y1)^2 + (z2-z1)^2)
								float distance = FVector::Distance(boid->GetTransform().GetLocation(), neighbor->GetTransform().GetLocation());

								//check if the neighbor is visible by the boid

								FVector boidDirectionVector = boid->GetActorForwardVector();

								FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();
								subVector.Normalize();

								float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(boidDirectionVector, subVector)));

								//this is a valid neighbor
								//if (FVector::DotProduct(boidDirectionVector, subVector) >= FMath::Cos(FMath::DegreesToRadians(BoidFOV/2.0f)))
								if (angle <= BoidFOV)
								{
									//if (i == numberOfBoids / 2)
									//{
									//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("neighborIndices.X %f, neighborIndices.Y %f, neighborIndices.Z %f and neighborIndex %d"), neighborIndices.X, neighborIndices.Y, neighborIndices.Z, neighborIndex);
									//}
									if (distance <= 200.0f)
									{
										alignment += neighbor->GetVelocity();
										alignmentCount++;
									}

									if (distance <= 200.0f)
									{
										cohesion += neighbor->GetTransform().GetLocation();
										cohesionCount++;
									}

									//separation logic will happen here
									if (distance <= 150.0f)
									{
										FVector separationSubVector = boid->GetTransform().GetLocation() - neighbor->GetTransform().GetLocation();
										separationSubVector.Normalize();
										separationSubVector /= distance;
										separation += separationSubVector;
										separationCount++;
									}
								}
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
			}
		}
	}
}

void UFlock::AddBoidToArray(class ABoid* boid)
{
	if (_boids.Num() > 0)
	{
		FVector boidLocation = boid->GetTransform().GetLocation();

		//with this the grid can have different width depth and height values
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
	int depthIndex = flatIndex / (_width * _height);

	flatIndex -= (depthIndex * _width * _height);

	int heightIndex = flatIndex / _width;

	int widthIndex = flatIndex % _width;

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

	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("After X or depth wise sort"));
	for (int i = 0; i < 5; i++)
	{
		auto boid = _boids[i];

		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);
	}

	////sort one depth at a time w.r.t height so we will have each layer sorted w.r.t Z
	//_dimensionToCompare = Compare::Height;
	//for (int i = 0; i < _depth; ++i)
	//{
	//	if (!_boids.IsValidIndex((i * _width * _height))) break;
	//	QuickSort((i * _width * _height), ((i + 1) * _width * _height) - 1);
	//}

	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("After Z or height wise sort"));
	//for (int i = 0; i < 5; i++)
	//{
	//	auto boid = _boids[i];

	//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);
	//}

	////now that we have sorted the array w.r.t. X and Z sort each row which represents width (Y) 
	//_dimensionToCompare = Compare::Width;
	//for (int i = 0; i < (_depth * _height); ++i)
	//{
	//	if (!_boids.IsValidIndex((i * _width))) break;
	//	QuickSort((i * _width), ((i + 1) * _width) - 1);
	//}

	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("After Y or width wise sort"));

	//for (int i = 0; i < 5; i++)
	//{
	//	auto boid = _boids[i];

	//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The boid at: %d is at location x: %f, y: %f, z: %f"), i, boid->GetTransform().GetLocation().X, boid->GetTransform().GetLocation().Y, boid->GetTransform().GetLocation().Z);
	//}
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

	if (_dimensionToCompare == Compare::Depth)
	{
		_xSet.Add(pivotVector.X);
	}
	else if (_dimensionToCompare == Compare::Width)
	{
		_ySet.Add(pivotVector.Y);
	}
	else if (_dimensionToCompare == Compare::Height)
	{
		_zSet.Add(pivotVector.Z);
	}

	int i = low - 1;
	for (int j = low; j < high; ++j)
	{
		FVector lhsBoidVector = _boids[j]->GetTransform().GetLocation();

		if (_dimensionToCompare == Compare::Depth)
		{
			_xSet.Add(lhsBoidVector.X);
		}
		else if (_dimensionToCompare == Compare::Width)
		{
			_ySet.Add(lhsBoidVector.Y);
		}
		else if (_dimensionToCompare == Compare::Height)
		{
			_zSet.Add(lhsBoidVector.Z);
		}

		//>= descending
		//<= ascending
		if (lhsBoidVector.X >= pivotVector.X)
		{
			if (lhsBoidVector.X > pivotVector.X)
			{
				++i;
				_boids.SwapMemory(i, j);
			}
			else if (lhsBoidVector.X == pivotVector.X)
			{
				if (lhsBoidVector.Y <= pivotVector.Y)
				{
					if (lhsBoidVector.Y < pivotVector.Y)
					{
						++i;
						_boids.SwapMemory(i, j);
					}
					else if (lhsBoidVector.Y == pivotVector.Y)
					{
						if (lhsBoidVector.Z >= pivotVector.Z)
						{
							++i;
							_boids.SwapMemory(i, j);
						}
					}
				}
			}
		}


		//if ((_dimensionToCompare == Compare::Depth && lhsBoidVector.X >= pivotVector.X) ||
		//	(_dimensionToCompare == Compare::Width && lhsBoidVector.Y <= pivotVector.Y) ||
		//	(_dimensionToCompare == Compare::Height && lhsBoidVector.Z >= pivotVector.Z))
		//{
		//	++i;
		//	_boids.SwapMemory(i, j);
		//}
	}

	_boids.SwapMemory(i + 1, high);
	return (i + 1);
}