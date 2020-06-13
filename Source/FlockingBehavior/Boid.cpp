// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

//float ABoid::_speed = -1.0f;

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

//FVector ABoid::GetDirectionVector()
//{
//	return _directionVector;
//}

FVector ABoid::GetVelocity()
{
	return _velocity;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	//if (ABoid::_speed == -1.0f)
	//{
	//	_speed = FMath::RandRange(MaxSpeed*0.5f, MaxSpeed);
	//}

	_world = GetWorld();
	//code to detect overlap
	FScriptDelegate scriptDelegate;
	scriptDelegate.BindUFunction(this, FName("OnActorOverlap"));
	OnActorBeginOverlap.Add(scriptDelegate);

	float angle = FMath::RandRange(-40.0f, 40.0f);// (-1, 1)* (i + 1) * 10.0f;

	_velocity.X = FMath::Cos(FMath::DegreesToRadians(angle));
	_velocity.Y = FMath::Sin(FMath::DegreesToRadians(angle));

	_velocity.Normalize();
	_velocity *= MaxSpeed;// FMath::RandRange(MaxSpeed * 0.5f, MaxSpeed);
	//if (_velocity.SizeSquared() > FMath::Square(MaxSpeed)) //p5.limit()
	//{
	//	_velocity /= _velocity.Size();
	//	_velocity *= MaxSpeed;
	//}

	float headingAngle = _velocity.HeadingAngle();
	//only in z because we want to rotate the boid in yaw, in xy plane
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(GetActorLocation(), quat, false, nullptr, ETeleportType::None);

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
}

// Called every frame
void ABoid::Tick(float DeltaTime, FVector alignmentSteering, FVector cohesionSteering, FVector separationSteering)
{
	if (!alignmentSteering.IsZero())
	{
		alignmentSteering.Normalize();
		alignmentSteering *= MaxSpeed;
		alignmentSteering = alignmentSteering - _velocity;

		if (alignmentSteering.SizeSquared() > (MaxSteeringForce * MaxSteeringForce))
		{
			alignmentSteering.Normalize();
			alignmentSteering *= MaxSteeringForce;
		}
	}

	if (!cohesionSteering.IsZero())
	{
		cohesionSteering = cohesionSteering - this->GetActorLocation();
		cohesionSteering.Normalize();
		cohesionSteering *= MaxSpeed;
		cohesionSteering = cohesionSteering - _velocity;

		if (cohesionSteering.SizeSquared() > (MaxSteeringForce * MaxSteeringForce))
		{

			cohesionSteering.Normalize();
			cohesionSteering *= MaxSteeringForce; 
		}
	}

	if (!separationSteering.IsZero())
	{
		separationSteering.Normalize();
		separationSteering *= MaxSpeed;
		separationSteering = separationSteering - _velocity;
		if (separationSteering.SizeSquared() > (MaxSteeringForce * MaxSteeringForce))
		{
			separationSteering.Normalize();
			separationSteering *= MaxSteeringForce;
		}
		separationSteering *= 1.5f;
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
	location.Z = 250.0f;
	//this is used to rotation the body of the boid in the direction it is moving
	float headingAngle = _velocity.HeadingAngle();
	//only in z because we want to rotate the boid in yaw, in xy plane
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

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

void ABoid::OnActorOverlap(AActor *SelfActor, AActor *OtherActor, FVector NormalImpulse, const FHitResult &Hit)
{
	////TArray<UStaticMeshComponent> StaticComps;
	////OtherActor->GetComponents< UStaticMeshComponent>(StaticComps);
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));

	//FVector origin;
	//FVector boidBounds;
	//OtherActor->GetActorBounds(true, origin, boidBounds);
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The bounds of %s are w: %f, h: %f, d: %f"), *(OtherActor->GetFullName()), boidBounds.X, boidBounds.Y, boidBounds.Z);
}

//FBox ABoid::GetBoundingBox()
//{
//	FVector bounds;
//	FVector center;
//	GetActorBounds(true, center, bounds);
//
//	return FBox(FVector(center - bounds), FVector(center + bounds));
//}