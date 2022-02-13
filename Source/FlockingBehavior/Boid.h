// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Boid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(FlockingBehaviorLogs, Log, All)

UCLASS(Blueprintable)
class FLOCKINGBEHAVIOR_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

	UPROPERTY(VisibleAnywhere, BlueprintreadOnly)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintreadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock", meta = (Tooltip = "Length of Ray cast (in cms) for checking forward collision with World Static objects, so that the boid can turn accordingly."))
	float FarSightness;

	//The Speed variable will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSpeed;

	//Steering will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSteeringForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float alignmentWeight = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float cohesionWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float separationWeight = 1.0f;


	FVector GetVelocity();

private:
	
	float _maxSteeringForceSquared;

	FVector _acceleration;

	UPROPERTY()
	FVector _velocity;

	UPROPERTY()
	FVector _actorBounds;

	UPROPERTY()
	UWorld* _world;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	void Tick(float DeltaTime, FVector alignmentSteering = FVector::ZeroVector, FVector cohesionSteering = FVector::ZeroVector, FVector separationSteering = FVector::ZeroVector);
};