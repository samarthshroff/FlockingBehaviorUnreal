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

	_velocity = FMath::RandRange(100.0f,MaxSpeed) * _directionVector;

	FVector origin;
	GetActorBounds(true, origin, _actorBounds);
	_actorBounds /= 2.0f;
}

// Called every frame
void ABoid::Tick(float DeltaTime, float alignmentAngle, FVector cohesion, FVector separation)
{

}

/*void ABoid::Tick(float DeltaTime, float alignmentAngle, FVector cohesion, FVector separation)
{
	//Super::Tick(DeltaTime);

	 DirectionAngle = alignmentAngle;// -DirectionAngle;

	_directionVector.X = FMath::Cos(FMath::DegreesToRadians(DirectionAngle));
	_directionVector.Y = FMath::Sin(FMath::DegreesToRadians(DirectionAngle));

	FVector headingVector = _directionVector;
	headingVector.Normalize(MaxSpeed);

	// _directionVector.Normalize();// /= _directionVector.Size();
	// _directionVector *= MaxSpeed;

	//_directionVector = _directionVector - _velocity;
	//if (_directionVector.Size() > MaxSteeringForce)
	//{
	//	_directionVector.Normalize(MaxSteeringForce);
	//}

	if (cohesion != FVector::ZeroVector)
	{
		//considering the mass of boid is 1 in digital world in F=ma
		//we get F=a, thus acceleration=cohesion steering force
		//thus using s = ut + 0.5*a*t where s = cohesion (as it is the displacement from boid position to the avg position of neighbors)
		//thus a = (2*(s - u*t))/t^2 where a is acceleration which is the steering force (F)
		//thus if we don't use the constant value 2 and time in the formula then we get a = s - v 
		//i.e. sterring force = disaplcement - velocity

		cohesion = cohesion - _velocity;

		// cohesion *= MaxSpeed;
		// cohesion = cohesion - _velocity;
		// if (cohesion.Size() > MaxSteeringForce)
		// {
		// 	cohesion.Normalize();// /= cohesion.Size();
		// 	cohesion *= MaxSteeringForce;
		// }
	}

	if (separation != FVector::ZeroVector)
	{
		//considering the mass of boid is 1 in digital world in F=ma
		//we get F=a, thus acceleration=separation steering force i.e. separation
		// v = at + u
		//separation = (separation * DeltaTime) + _velocity;

		separation = 

		// separation *= MaxSpeed;
		// separation = separation - _velocity;
		// if (separation.Size() > MaxSteeringForce)
		// {
		// 	separation.Normalize();// /= separation.Size();
		// 	separation *= MaxSteeringForce;
		// 	//separation.Normalize(MaxSteeringForce);
		// }
	}

	FVector acceleration = separation;	//_directionVector + cohesion + separation;

	_velocity = acceleration * DeltaTime + _velocity;

	if (_velocity.Size() > MaxSpeed)
	{
		_velocity.Normalize(MaxSpeed);
	}

	FVector displacement = _velocity * DeltaTime;

	FVector location = GetActorLocation();
	location += displacement;

	float headingAngle = headingVector.HeadingAngle();
	//only in z because we want to rotate the boid in yaw, in xy plane 
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);

	FVector forwardVector = GetActorForwardVector();

	//code for detecting world static obstacles
	FVector newVector = FVector(_actorBounds.X * forwardVector.X, _actorBounds.Y * forwardVector.Y, _actorBounds.Z * forwardVector.Z);
	FVector start = GetActorLocation()+newVector;
	FVector end = forwardVector * FarSightness + start;
	FHitResult hitResult;
	FCollisionQueryParams collisionParams;

	//DrawDebugLine(GetWorld(),start,end,FColor::Blue,true,-1.0f,0,2.0f);
	//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("start %s and end is %s "),*(start.ToString()),*(end.ToString()));

	if(_world->LineTraceSingleByObjectType(hitResult,start,end,ECC_WorldStatic,collisionParams))
	{
		//UE_LOG(FlockingBehaviorLogs,Warning,TEXT("found something"));
		//calculate the angle by which the boid will turn or rotate. For now just consider that the boid will make a kind of u-turn
		//We will add the moving around a static object afterwards
		//TO-DO add logic to move around a world static object like pillars
	}	
}*/

	// void ABoid::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
	// {
	// 	UE_LOG(FlockingBehaviorLogs,Warning,TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));
	// }