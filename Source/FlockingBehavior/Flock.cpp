// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"

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

	// ...

}

void UFlock::Initialize(class AActor* boidsOwner, TSubclassOf<class ABoid> boidBPClass)
{
	_boidOwner = boidsOwner;
	_boidBPClass = boidBPClass;

	//FVector origin;
	// GetActorBounds(true, origin, _actorBounds);
	//_actorBounds /= 2.0f;

	_width = _height = _depth = _gridSize = 1;

	UWorld* world = GetWorld();

	if (world)
	{
		//get the boid bounds and destroy this actor. Separation radius should not be less than the boid's bounds.
		auto tempBoid = world->SpawnActor<ABoid>(_boidBPClass, FTransform(FVector::ZeroVector));

		FVector origin;
		FVector boidBounds;
		tempBoid->GetActorBounds(true, origin, boidBounds);

		tempBoid->Destroy();
		auto maxComponent = boidBounds.GetMax();
		SeparationRadius = FMath::Max(SeparationRadius, maxComponent * 1.5f);

		FActorSpawnParameters params;

		for (int i = 0; i < 1; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);
			params.Owner = _boidOwner;
			params.Name = FName(*name);

			FTransform transform = FTransform(FVector(-3160.0f, 0.0f, 250.0f));
			auto boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
			boid->SetActorLabel(name);

			boid->DirectionAngle = 1.0f;// FMath::RandRange(-180.0f, 180.0f);

			AddBoidToArray(boid);
		}

		////this entire hard coding will be optimized
		//FString name = "Boid";
		//name.AppendInt(_boids.Num());

		//FActorSpawnParameters params;
		//params.Owner = _boidOwner;
		//params.Name = FName(*name);

		//FTransform transform = FTransform(FVector(-3160.0f, 0.0f, 250.0f));
		//auto boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		//boid->SetActorLabel(name);

		//boid->DirectionAngle = 0.0f;

		//AddBoidToArray(boid);

		//name = "Boid";
		//name.AppendInt(_boids.Num());
		//params.Owner = _boidOwner;
		//params.Name = FName(*name);

		//transform = FTransform(FVector(-3160.0f, 400.0f, 250.0f));
		//boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		//boid->SetActorLabel(name);
		//boid->DirectionAngle = -15.0f;

		//AddBoidToArray(boid);

		//name = "Boid";
		//name.AppendInt(_boids.Num());
		//params.Owner = _boidOwner;
		//params.Name = FName(*name);

		//transform = FTransform(FVector(-3160.0f, 200.0f, 250.0f));
		//boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		//boid->SetActorLabel(name);
		//boid->DirectionAngle = 1.0f;

		//AddBoidToArray(boid);

		//name = "Boid";
		//name.AppendInt(_boids.Num());
		//params.Owner = _boidOwner;
		//params.Name = FName(*name);

		//transform = FTransform(FVector(-3160.0f, -200.0f, 250.0f));
		//boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		//boid->SetActorLabel(name);
		//boid->DirectionAngle = -2.0f;

		//AddBoidToArray(boid);

		//name = "Boid";
		//name.AppendInt(_boids.Num());
		//params.Owner = _boidOwner;
		//params.Name = FName(*name);

		//transform = FTransform(FVector(-3280.0f, -200.0f, 250.0f));
		//boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
		//boid->SetActorLabel(name);
		//boid->DirectionAngle = 10.0f;

		//AddBoidToArray(boid);
	}

	bool index0 = _boids.IsValidIndex(0);
	bool index3 = _boids.IsValidIndex(3);
	bool index4 = _boids.IsValidIndex(4);

	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("width is:: %d, height %d, and depth:: %d "), _width,_height,_depth);
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
	_boids[0]->Tick(DeltaTime, 0.0f, FVector::ZeroVector, FVector::ZeroVector);
	//if (_boids.Num() > 0)
	//{
	//	//sort the boids here
	//	SortBoids();

	//	//for(auto& boid : _boids)
	//	for (int i = 0; i < _boids.Num(); i++)//
	//	{
	//		auto boid = _boids[i];
	//		//get the neigbors to a given boid and send a data structure of there transofrm, velocity and direction to the 
	//		//boid in focus
	//		FVector indices;
	//		Get3DIndicies(i, indices);

	//		//if (boid->GetActorLabel().Equals(TEXT("Boid3")))
	//		//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("i:: %d, indices.X:: %f indices.Y:: %f, indices.Z:: %f"), i, indices.X, indices.Y, indices.Z);

	//		float alignmentAngle = 0.0f;
	//		FVector separation = FVector::ZeroVector;
	//		FVector cohesion = FVector::ZeroVector;
	//		int neighborCount = 0;
	//		int separationCount = 0;

	//		//TArray<FNeighborData> neighborsData;

	//		/*for (int x = -1; x <= 1; x++)
	//		{
	//			for (int y = -1; y <= 1; y++)
	//			{
	//				for (int z = -1; z <= 1; z++)
	//				{
	//					//this is the location of boid whose neighbors we are finding
	//					//if (x == 0 && y == 0 && z == 0) continue;

	//					//if (boid->GetActorLabel().Equals(TEXT("Boid3")) && (x == 0 && y == 0 && z == 1))
	//					//{
	//					//	int g = 0;
	//					//}

	//					if (indices.Z == 0)
	//					{
	//						if (_height == 1 && z != 0) continue;
	//						if (_height > 1 && z < 0) continue;
	//					}
	//					else if (indices.Z == _height-1)
	//					{
	//						if (z == 1) continue;
	//					}

	//					if (indices.Y == 0)
	//					{
	//						if (_width == 1 && y != 0) continue;
	//						if (_width > 1 && y < 0) continue;
	//					}
	//					else if (indices.Y == _width - 1)
	//					{
	//						if (y == 1) continue;
	//					}

	//					if (indices.X == 0)
	//					{
	//						if (_depth == 1 && x != 0) continue;
	//						if (_depth > 1 && x < 0) continue;
	//					}
	//					else if (indices.X == _depth - 1)
	//					{
	//						if (x == 1) continue;
	//					}

	//					FVector neighborIndices;

	//					neighborIndices.X = indices.X + x;
	//					neighborIndices.Y = indices.Y + y;
	//					neighborIndices.Z = indices.Z + z;

	//					//if (boid->GetActorLabel().Equals(TEXT("Boid3")))
	//					//{
	//					//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("(x:: %d, y:: %d, z:: %d) && (neighborIndices.X:: %f, neighborIndices.Y:: %f, neighborIndices.Z:: %f)"), x,y,z, neighborIndices.X, neighborIndices.Y, neighborIndices.Z);
	//					//}

	//					if (neighborIndices.X < 0.0f || neighborIndices.Y < 0.0f || neighborIndices.Z < 0.0f) continue;
	//					
	//					int neighborIndex;
	//					GetFlatIndex(neighborIndices, neighborIndex);

	//					//if (boid->GetActorLabel().Equals(TEXT("Boid3")))
	//					//	UE_LOG(FlockingBehaviorLogs, Error, TEXT("neighborIndex:: %d "), neighborIndex);

	//					if (_boids.IsValidIndex(neighborIndex) && neighborIndex != i)
	//					{
	//						//check if this object is in vicinity and in vision
	//						auto neighbor = _boids[neighborIndex];

	//						//distance is sqrt((x2-x2)^2 + (y2-y1)^2 + (z2-z1)^2)
	//						float distance = FVector::Distance(boid->GetTransform().GetLocation(), neighbor->GetTransform().GetLocation());
	//						if (distance <= NeighborhoodRadius)
	//						{
	//							//check if the neighbor is visible by the boid

	//							//FVector boidDirectionVector = boid->GetDirectionVector();

	//							//FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();

	//							//FVector normalizedSubVector = subVector.GetSafeNormal();

	//							////subVector.ToDirectionAndLength(normalizedSubVector, magnitude);

	//							//float angle = FMath::Acos(FVector::DotProduct(boidDirectionVector, normalizedSubVector));

	//							////this is a valid neighbor
	//							//if (angle <= BoidFOV / 2.0f)

	//							FVector boidDirectionVector = boid->GetDirectionVector();

	//							FVector subVector = neighbor->GetTransform().GetLocation() - boid->GetTransform().GetLocation();

	//							//this is a valid neighbor
	//							//if (FVector::DotProduct(boidDirectionVector, subVector) >= FMath::Cos(FMath::DegreesToRadians(BoidFOV / 2.0f)))
	//							//{
	//								UE_LOG(FlockingBehaviorLogs, Warning, TEXT("Boid is:: %s and NEIGHBOR is:: %s "), *(boid->GetActorLabel()), *(neighbor->GetActorLabel()) );

	//								neighborCount++;
	//								alignmentAngle += neighbor->DirectionAngle;

	//								cohesion += neighbor->GetTransform().GetLocation();
	//							//}
	//							//distance > 0.0f &&
	//							if (distance <= SeparationRadius)
	//							{
	//								subVector = boid->GetTransform().GetLocation() - neighbor->GetTransform().GetLocation();
	//								//subVector.Normalize();
	//								//subVector /= distance;
	//								separation += subVector;
	//								separationCount++;
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}*/

	//		//if (neighborCount > 0)
	//		//{
	//		//	alignmentAngle /= neighborCount;

	//		//	cohesion /= neighborCount;
	//		//	cohesion -= boid->GetTransform().GetLocation();
	//		//	//mag can be 0
	//		//	cohesion = cohesion.GetSafeNormal();
	//		//}

	//		//if (separationCount > 0)
	//		//{
	//		//	separation /= separationCount;
	//		//}

	//		boid->Tick(DeltaTime, alignmentAngle, cohesion, separation);

	//		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("******************************************"));
	//	}
	//}
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