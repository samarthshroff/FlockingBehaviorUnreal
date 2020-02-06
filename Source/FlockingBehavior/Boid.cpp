// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Mesh->SetupAttachment(RootComponent);
}

FVector ABoid::GetDirectionVector()
{
	return _directionVector;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	_world = GetWorld();
	//code to detect overlap
	// FScriptDelegate scriptDelegate;
	// scriptDelegate.BindUFunction(this,FName("OnActorOverlap"));
	// OnActorBeginOverlap.Add(scriptDelegate);
	
	//float magnitude;
	//DirectionVector.ToDirectionAndLength(DirectionVector, magnitude);

	//_velocity = Speed * DirectionVector;

	_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	_directionVector.Normalize();

	//FMath::RandRange(100.0f,MaxSpeed)
	_velocity = 85.0f * _directionVector;

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
	_actorBounds /= 2.0f;
}
void ABoid::Tick(float DeltaTime, TArray<ABoid*> boids)
{
	auto separate = Separate(boids);
	auto align = Align(boids);
	auto cohesion = Cohesion(boids);

	_velocity += align + separate + cohesion;
	//_velocity.Normalize(MaxSpeed);

	FVector displacement = _velocity * DeltaTime;

	FVector location = GetActorLocation();
	location += displacement;

	float headingAngle = _directionVector.HeadingAngle();
	//only in z because we want to rotate the boid in yaw, in xy plane 
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

	FVector forwardVector = GetActorForwardVector();

	//code for detecting world static obstacles
	FVector newVector = FVector(_actorBounds.X * forwardVector.X, _actorBounds.Y * forwardVector.Y, _actorBounds.Z * forwardVector.Z);
	FVector start = GetActorLocation() + newVector;
	FVector end = forwardVector * FarSightness + start;
	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	//DrawDebugLine(GetWorld(),start,end,FColor::Blue,true,-1.0f,0,2.0f);
	//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("start %s and end is %s "),*(start.ToString()),*(end.ToString()));

	if (_world->LineTraceSingleByObjectType(hitResult, start, end, ECC_WorldStatic, collisionParams))
	{
		//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("found something"));
		//calculate the angle by which the boid will turn or rotate. For now just consider that the boid will make a kind of u-turn
		//We will add the moving around a static object afterwards
		//TO-DO add logic to move around a world static object like pillars
	}
}

FVector ABoid::Separate(TArray<ABoid*> boids)
{
	FVector steer = FVector::ZeroVector;
	int count = 0;

	for (auto& boid : boids)
	{
		float distance = FVector::Distance(GetTransform().GetLocation(), boid->GetTransform().GetLocation());

		if (distance > 0.0f && distance < 250.0f)
		{
			FVector distanceVector = boid->GetTransform().GetLocation() - GetTransform().GetLocation();

			if (FVector::DotProduct(GetDirectionVector(), distanceVector) >= FMath::Cos(FMath::DegreesToRadians(120.0f / 2.0f)))
			{
				FVector diff = GetTransform().GetLocation() - boid->GetTransform().GetLocation();
				diff.Normalize();
				diff /= distance;
				steer += diff;
				count++;
			}
		}
	}

	if (count > 0)
	{
		steer /= count;
	}
	if (steer.Size() > 0)
	{
		steer.Normalize();
		steer *= MaxSpeed;
		steer = steer - _velocity;

		if (steer.Size() > MaxSteeringForce)
		{
			steer.Normalize();
			steer *= MaxSteeringForce;
		}
	}
	return steer;
}

FVector ABoid::Align(TArray<ABoid*> boids)
{
	FVector directionVector = FVector::ZeroVector;
	int count = 0;
	float alignmentAngle = 0.0f;

	for (auto& boid : boids)
	{
		float distance = FVector::Distance(GetTransform().GetLocation(), boid->GetTransform().GetLocation());
			   
		if (distance > 0.0f && distance < 350.0f)
		{
			FVector distanceVector = boid->GetTransform().GetLocation() - GetTransform().GetLocation();

			if (FVector::DotProduct(GetDirectionVector(), distanceVector) >= FMath::Cos(FMath::DegreesToRadians(120.0f / 2.0f)))
			{
				alignmentAngle += boid->DirectionAngle;
				count++;
			}
		}
	}
	if (count > 0)
	{
		alignmentAngle += count;
		DirectionAngle = alignmentAngle;

		directionVector.X = FMath::Cos(FMath::DegreesToRadians(alignmentAngle));
		directionVector.Y = FMath::Sin(FMath::DegreesToRadians(alignmentAngle));

		directionVector.Normalize();// /= _directionVector.Size();
		directionVector *= MaxSpeed;
	}
	_directionVector = directionVector;
	return directionVector;
}

FVector ABoid::Cohesion(TArray<ABoid*> boids)
{
	FVector cohesion = FVector::ZeroVector;
	int count = 0;

	for (auto& boid : boids)
	{
		float distance = FVector::Distance(GetTransform().GetLocation(), boid->GetTransform().GetLocation());

		if (distance > 0.0f && distance < 350.0f)
		{
			FVector distanceVector = boid->GetTransform().GetLocation() - GetTransform().GetLocation();

			if (FVector::DotProduct(GetDirectionVector(), distanceVector) >= FMath::Cos(FMath::DegreesToRadians(120.0f / 2.0f)))
			{
				cohesion += boid->GetTransform().GetLocation();
				count++;
			}
		}
	}
	if (count > 0)
	{
		cohesion /= count;
		//vector pointing from the avg cohesion vector to this boids position
		cohesion -= GetTransform().GetLocation();

		//scale it to maximum speed
		cohesion.Normalize();
		cohesion *= MaxSpeed;

		//steer
		cohesion -= _velocity;
		//limit to max steer
		if (cohesion.Size() > MaxSteeringForce)
		{
			cohesion.Normalize();
			cohesion *= MaxSteeringForce;
		}
	}
	return cohesion;
}

// Called every frame
void ABoid::Tick(float DeltaTime, float alignmentAngle, FVector cohesion, FVector separation)
{
	////Super::Tick(DeltaTime);

	// DirectionAngle = alignmentAngle;// -DirectionAngle;

	//_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	//_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	//_directionVector.Normalize();// /= _directionVector.Size();
	//_directionVector *= MaxSpeed;

	////_directionVector = _directionVector - _velocity;
	////if (_directionVector.Size() > MaxSteeringForce)
	////{
	////	_directionVector.Normalize(MaxSteeringForce);
	////}

	//if (cohesion != FVector::ZeroVector)
	//{
	//	cohesion *= MaxSpeed;
	//	cohesion = cohesion - _velocity;
	//	if (cohesion.Size() > MaxSteeringForce)
	//	{
	//		cohesion.Normalize();// /= cohesion.Size();
	//		cohesion *= MaxSteeringForce;
	//	}
	//}

	//if (separation != FVector::ZeroVector)
	//{
	//	separation *= MaxSpeed;
	//	separation = separation - _velocity;
	//	if (separation.Size() > MaxSteeringForce)
	//	{
	//		separation.Normalize();// /= separation.Size();
	//		separation *= MaxSteeringForce;
	//		//separation.Normalize(MaxSteeringForce);
	//	}
	//}
	//
	//_velocity = _directionVector;// +cohesion + separation;// 
	//	 
	//FVector displacement = _velocity * DeltaTime;

	//FVector location = GetActorLocation();
	//location += displacement;

	//float headingAngle = _directionVector.HeadingAngle();
	////only in z because we want to rotate the boid in yaw, in xy plane 
	//FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	//SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

	//FVector forwardVector = GetActorForwardVector();

	////code for detecting world static obstacles
	//FVector newVector = FVector(_actorBounds.X * forwardVector.X, _actorBounds.Y * forwardVector.Y, _actorBounds.Z * forwardVector.Z);
	//FVector start = GetActorLocation()+newVector;
	//FVector end = forwardVector * FarSightness + start;
	//FHitResult hitResult;
	//FCollisionQueryParams collisionParams;

	////DrawDebugLine(GetWorld(),start,end,FColor::Blue,true,-1.0f,0,2.0f);
	////UE_LOG(FlockingBehaviorLogs,Warning,TEXT("start %s and end is %s "),*(start.ToString()),*(end.ToString()));

	//if(_world->LineTraceSingleByObjectType(hitResult,start,end,ECC_WorldStatic,collisionParams))
	//{
	//	//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("found something"));
	//	//calculate the angle by which the boid will turn or rotate. For now just consider that the boid will make a kind of u-turn
	//	//We will add the moving around a static object afterwards
	//	//TO-DO add logic to move around a world static object like pillars
	//}	
}

// void ABoid::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
// {
// 	UE_LOG(FlockingBehaviorLogs,Warning,TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));
// }