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

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	if (ABoid::_speed == -1.0f)
	{
		_speed = FMath::RandRange(100.0f, MaxSpeed);
	}

	_world = GetWorld();
	//code to detect overlap
	FScriptDelegate scriptDelegate;
	scriptDelegate.BindUFunction(this, FName("OnActorOverlap"));
	OnActorBeginOverlap.Add(scriptDelegate);

	_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	_directionVector.Normalize();

	_velocity = _speed * _directionVector;

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
	_actorBounds /= 2.0f;
}

// Called every frame
void ABoid::Tick(float DeltaTime, float alignmentAngle, FVector cohesion, FVector separation)
{
	//Super::Tick(DeltaTime);

	// DirectionAngle = alignmentAngle;// -DirectionAngle;

	_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	_directionVector.Normalize();

	//the three vectors - alignment, cohesion and sepration will affect the velocity here

	_velocity = _speed * _directionVector;
	FVector displacement = _velocity * DeltaTime;

	FVector location = GetActorLocation();
	location += displacement;

	float headingAngle = _directionVector.HeadingAngle();
	//only in z because we want to rotate the boid in yaw, in xy plane 
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

	FVector forwardVector = GetActorForwardVector();
}

void ABoid::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//TArray<UStaticMeshComponent> StaticComps;
	//OtherActor->GetComponents< UStaticMeshComponent>(StaticComps);
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));

	FVector origin;
	FVector boidBounds;
	OtherActor->GetActorBounds(true, origin, boidBounds);
	UE_LOG(FlockingBehaviorLogs, Warning, TEXT("The bounds of %s are w: %f, h: %f, d: %f"), *(OtherActor->GetFullName()), boidBounds.X, boidBounds.Y, boidBounds.Z);
}