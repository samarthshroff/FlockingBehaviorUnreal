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

	_width = _height = _depth = _gridSize = 1;

	UWorld* world = GetWorld();

	if (world)
	{
		FActorSpawnParameters params;

		for (int i = 0; i < 5; i++)
		{
			FString name = "Boid";
			name.AppendInt(i);
			params.Owner = _boidOwner;
			params.Name = FName(*name);

			FTransform transform = FTransform(FVector(-3160.0f, 0.0f, 250.0f));
			auto boid = world->SpawnActor<ABoid>(_boidBPClass, transform, params);
			boid->SetActorLabel(name);

			boid->DirectionAngle = 0.0f;

			AddBoidToArray(boid);
		}

		////this entire hard coding will be optimized
		//FString name = "Boid";
		//name.AppendInt(_boids.Num());		
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
}

// Called every frame
void UFlock::TickComponent(float DeltaTime)
{
	if (_boids.Num() > 0)
	{
		//sort the boids here
		SortBoids();

		for (auto& boid : _boids)
		{
			boid->Tick(DeltaTime, _boids);
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