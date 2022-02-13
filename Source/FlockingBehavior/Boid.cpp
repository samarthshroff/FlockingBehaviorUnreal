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

FVector ABoid::GetVelocity()
{
	return _velocity;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	_world = GetWorld();
	////code to detect overlap
	//FScriptDelegate scriptDelegate;
	//scriptDelegate.BindUFunction(this, FName("OnActorOverlap"));
	//OnActorBeginOverlap.Add(scriptDelegate);

	float angle = FMath::RandRange(-40.0f, 40.0f);

	_velocity.X = FMath::Cos(FMath::DegreesToRadians(angle));
	_velocity.Y = FMath::Sin(FMath::DegreesToRadians(angle));

	_velocity.Normalize();
	_velocity *= MaxSpeed;

	_maxSteeringForceSquared = MaxSteeringForce * MaxSteeringForce;

	GetRootComponent()->SetWorldRotation(_velocity.ToOrientationRotator());

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
}

// Called every frame
void ABoid::Tick(float DeltaTime, FVector alignmentSteering, FVector cohesionSteering, FVector separationSteering)
{
	if (!alignmentSteering.IsZero())
	{
		// Steering logic for alignment.
		alignmentSteering.Normalize();
		alignmentSteering *= MaxSpeed;
		alignmentSteering = alignmentSteering - _velocity;

		if (alignmentSteering.SizeSquared() > _maxSteeringForceSquared)
		{
			alignmentSteering.Normalize();
			alignmentSteering *= MaxSteeringForce;
		}

		alignmentSteering *= alignmentWeight;
	}

	if (!cohesionSteering.IsZero())
	{
		// Steering logic for cohesion
		// Cohesion is to find the average position of this boid wrt neighbors,
		// thus substract this boid's position from the calculated average neighbors' positions.
		cohesionSteering = cohesionSteering - this->GetActorLocation();
		cohesionSteering.Normalize();
		cohesionSteering *= MaxSpeed;
		cohesionSteering = cohesionSteering - _velocity;

		if (cohesionSteering.SizeSquared() > _maxSteeringForceSquared)
		{

			cohesionSteering.Normalize();
			cohesionSteering *= MaxSteeringForce;
		}
		cohesionSteering *= cohesionWeight;
	}

	if (!separationSteering.IsZero())
	{
		// Steering logic for separation. How much distance to keep 
		// between 2 boids.
		separationSteering.Normalize();
		separationSteering *= MaxSpeed;
		separationSteering = separationSteering - _velocity;

		if (separationSteering.SizeSquared() > _maxSteeringForceSquared)
		{
			separationSteering.Normalize();
			separationSteering *= MaxSteeringForce;
		}
		separationSteering *= separationWeight;
	}

	_acceleration = alignmentSteering + cohesionSteering + separationSteering;

	_velocity = _velocity + _acceleration;
	if (_velocity.Size() > MaxSpeed)
	{
		_velocity /= _velocity.Size();
		_velocity *= MaxSpeed;
	}

	FVector location = GetActorLocation();
	location += _velocity;

	//SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);
	SetActorLocation(location, false, nullptr, ETeleportType::None);
	GetRootComponent()->SetWorldRotation(_velocity.ToOrientationRotator());

	_acceleration = FVector::ZeroVector;

	//code for detecting world static obstacles
	FVector forwardVector = GetActorForwardVector();
	FVector newVector = FVector(_actorBounds.X * forwardVector.X, _actorBounds.Y * forwardVector.Y, _actorBounds.Z * forwardVector.Z);
	FVector start = GetActorLocation() + newVector;
	FVector end = forwardVector * FarSightness + start;
	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	//DrawDebugLine(GetWorld(),start,end,FColor::Blue,true,-1.0f,0,2.0f);
	//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("start %s and end is %s "),*(start.ToString()),*(end.ToString()));

	if (_world->LineTraceSingleByObjectType(hitResult, start, end, ECC_WorldStatic, collisionParams))
	{
		//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("found something*************  "));

		_velocity = _velocity - 2 * (_velocity * hitResult.Normal) * hitResult.Normal;

		//calculate the angle by which the boid will turn or rotate. For now just consider that the boid will make a kind of u-turn
		//We will add the moving around a static object afterwards
		//TO-DO add logic to move around a world static object like pillars
	}
}

//void ABoid::OnActorOverlap(AActor *SelfActor, AActor *OtherActor, FVector NormalImpulse, const FHitResult &Hit)
//{
//	////TArray<UStaticMeshComponent> StaticComps;
//	////OtherActor->GetComponents< UStaticMeshComponent>(StaticComps);
//	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));
//
//	//FVector origin;
//	//FVector boidBounds;
//	//OtherActor->GetActorBounds(true, origin, boidBounds);
//	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The bounds of %s are w: %f, h: %f, d: %f"), *(OtherActor->GetFullName()), boidBounds.X, boidBounds.Y, boidBounds.Z);
//}
