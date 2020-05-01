// Fill out your copyright notice in the Description page of Project Settings.

#include "Boid.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

float ABoid::_speed = -1.0f;

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

FVector ABoid::GetVelocity()
{
	return _velocity;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	if (ABoid::_speed == -1.0f)
	{
		_speed = FMath::RandRange(MaxSpeed*0.5f, MaxSpeed);
	}

	_world = GetWorld();
	//code to detect overlap
	FScriptDelegate scriptDelegate;
	scriptDelegate.BindUFunction(this, FName("OnActorOverlap"));
	OnActorBeginOverlap.Add(scriptDelegate);

	_velocity.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	_velocity.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	//_directionVector.Normalize();
	////_velocity = _speed * _directionVector;
	//_velocity = _directionVector;
	_velocity *= _speed;
	if (_velocity.SizeSquared() > FMath::Square(MaxSpeed)) //p5.limit()
	{
		auto mag = _velocity.Size();
		_velocity /= mag;
		_velocity *= MaxSpeed;
	}

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
	_actorBounds /= 2.0f;
}

// Called every frame
void ABoid::Tick(float DeltaTime, float alignmentAngle, FVector alignmentSteering, FVector cohesionSteering, FVector separationSteering)
{
	//the three vectors - alignment, cohesion and sepration will affect the velocity here

	//if (alignmentAngle != DirectionAngle) //!alignment.IsZero())
	//{
	//	DirectionAngle = alignmentAngle;

	//	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("the direction angle is:: %f "), DirectionAngle);

	//	_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	//	_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	//	////_directionVector = alignment;
	//	//always go in the direction of neighbor but with max speed

	//	_directionVector.Normalize();
	//	_directionVector *= MaxSpeed;
	//}

	//alignmentSteering = FVector::ZeroVector;
	//cohesionSteering = FVector::ZeroVector;
	//separationSteering = FVector::ZeroVector;

	if (!alignmentSteering.IsZero())
	{
		//alignmentSteering = alignment;

		//DirectionAngle = FMath::RadiansToDegrees(FMath::Acos(_directionVector.X));

		//these 2 lines are p5.setMag Normalize() should give the same output. need to check 
		alignmentSteering /= alignmentSteering.Size();
		alignmentSteering *= MaxSpeed;

		//steering = desired - velocity
		alignmentSteering = alignmentSteering - _velocity;
		//if (alignmentSteering.SizeSquared() > FMath::Square(MaxSteeringForce)) //p5.limit()
		//{
		//	alignmentSteering /= alignmentSteering.Size();
		//	alignmentSteering *= MaxSteeringForce;
		//}

		
		//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_directionVector.X:: %f _directionVector.Y: %f _directionVector.Z: %f"), _directionVector.X, _directionVector.Y, _directionVector.Z);
	}

	if (!cohesionSteering.IsZero())
	{
		cohesionSteering = cohesionSteering - this->GetActorLocation();

		cohesionSteering /= cohesionSteering.Size();
		cohesionSteering *= MaxSpeed;

		cohesionSteering = cohesionSteering - _velocity;
		//if (cohesionSteering.SizeSquared() > FMath::Square(MaxSteeringForce)) //p5.limit()
		//{
		//	cohesionSteering /= cohesionSteering.Size();
		//	cohesionSteering *= MaxSteeringForce;
		//}
	}

	if (!separationSteering.IsZero())
	{
		separationSteering /= separationSteering.Size();
		separationSteering *= MaxSpeed;

		separationSteering = separationSteering - _velocity;
		//if (separationSteering.SizeSquared() > FMath::Square(MaxSteeringForce)) //p5.limit()
		//{
		//	separationSteering /= separationSteering.Size();
		//	separationSteering *= MaxSteeringForce;
		//}
	}
	
	_acceleration = alignmentSteering + cohesionSteering + separationSteering;

	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT("_acceleration is: X %f Y %f Z %f"), _acceleration.X, _acceleration.Y, _acceleration.Z);
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("velocity before applying to displacement is: X %f Y %f Z %f and DeltaTime %f"), _velocity.X, _velocity.Y, _velocity.Z, DeltaTime);

	FVector displacement = _velocity; //*DeltaTime;
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT(" displacement is: X %f Y %f Z %f "), displacement.X, displacement.Y, displacement.Z);
	//UE_LOG(FlockingBehaviorLogs, Warning, TEXT(" displacement * _velocity.Size() is: X %f Y %f Z %f "), (displacement * _velocity.Size()).X, (displacement * _velocity.Size()).Y, (displacement * _velocity.Size()).Z);
	FVector location = GetActorLocation();
	location += displacement;

	//SetActorLocation(location);

	////this is used to rotation the body of the boid in the direction it is moving
	float headingAngle = _velocity.HeadingAngle();

	//only in z because we want to rotate the boid in yaw, in xy plane
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

	_velocity += _acceleration;

	_acceleration = FVector::ZeroVector;

	if (_velocity.Size() > MaxSpeed) //p5.limit()
	{
		auto mag = _velocity.Size();
		_velocity /= mag;
		_velocity *= MaxSpeed;
	}
}

void ABoid::OnActorOverlap(AActor *SelfActor, AActor *OtherActor, FVector NormalImpulse, const FHitResult &Hit)
{
	//TArray<UStaticMeshComponent> StaticComps;
	//OtherActor->GetComponents< UStaticMeshComponent>(StaticComps);
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));

	FVector origin;
	FVector boidBounds;
	OtherActor->GetActorBounds(true, origin, boidBounds);
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The bounds of %s are w: %f, h: %f, d: %f"), *(OtherActor->GetFullName()), boidBounds.X, boidBounds.Y, boidBounds.Z);
}