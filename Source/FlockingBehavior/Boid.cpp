// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

DEFINE_LOG_CATEGORY(FlockingBehaviorLogs)

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate scriptDelegate;
	scriptDelegate.BindUFunction(this,FName("OnActorOverlap"));
	OnActorBeginOverlap.Add(scriptDelegate);
	
	float magnitude;
	DirectionVector.ToDirectionAndLength(DirectionVector, magnitude);

	_velocity = Speed * DirectionVector;

	//float headingAngle = DirectionVector.HeadingAngle();
	//UE_LOG(FlockingBehaviorLogs, Log, TEXT("in begin play heading angle : %f"), headingAngle);

	//FVector forward = Mesh->GetForwardVector();
	//UE_LOG(FlockingBehaviorLogs, Log, TEXT("in begin play forward vector: %s"), *(forward.ToString()));
	//FVector rotatedVector = forward.RotateAngleAxis(headingAngle, FVector(0, 0, 1));
	//UE_LOG(FlockingBehaviorLogs, Log, TEXT("in begin play rotated vector: %s"), *(rotatedVector.ToString()));

	//FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);
	//SetActorLocationAndRotation(GetActorLocation(), quat, false, nullptr, ETeleportType::None);
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector displacement = _velocity * DeltaTime;

	FVector location = GetActorLocation();	 
	location += displacement;

	float headingAngle = DirectionVector.HeadingAngle();
	FQuat quat = FQuat(FVector(0, 0, 1), headingAngle);

	SetActorLocationAndRotation(location, quat, false, nullptr, ETeleportType::None);
	//SetActorTransform(transform);

	//FVector forward = GetActorForwardVector();

	//SetActorForwa

	//float headingAngle = DirectionVector.HeadingAngle();
	//FRotator actualRot = Mesh->GetRelativeRotationCache();

}

void ABoid::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(FlockingBehaviorLogs,Warning,TEXT("self actor %s hit with: %s"), *(SelfActor->GetFullName()), *(OtherActor->GetFullName()));
}