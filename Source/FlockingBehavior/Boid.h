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
	
<<<<<<< HEAD
public:	
=======
public:
>>>>>>> origin/simple-basic-algorithm
	// Sets default values for this actor's properties
	ABoid();

	UPROPERTY(VisibleAnywhere, BlueprintreadOnly)
<<<<<<< HEAD
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintreadOnly)
	UStaticMeshComponent* Mesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boid")
	//FVector DirectionVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid", meta=(Tooltip="Angle in degrees."))
	float DirectionAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock", meta = (Tooltip = "Length of Ray cast (in cms) for checking forward collision with World Static objects, so that the boid can turn accordingly."))
	float FarSightness;

	////current speed by which each boid moves
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	//float Speed;

	//The Speed variable will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSpeed;

	//Steering will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSteeringForce;

	FVector GetDirectionVector();

private:

	//speed * direction vector
	UPROPERTY()
	FVector _velocity;

	UPROPERTY()
	FVector _actorBounds;

	UPROPERTY()
	UWorld* _world;

	UPROPERTY()
	FVector _directionVector;
=======
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
>>>>>>> origin/simple-basic-algorithm

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

<<<<<<< HEAD
public:	
	// Called every frame
	void Tick(float DeltaTime, float alignmentAngle, FVector cohesion, FVector separation);

//  UFUNCTION()
//     void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
};

//USTRUCT()
//struct FNeighborData
//{
//	float DirectionAngle;
//	FVector Location;
//	float Distance;
//};
=======
public:
	// Called every frame
	void Tick(float DeltaTime, FVector alignmentSteering = FVector::ZeroVector, FVector cohesionSteering = FVector::ZeroVector, FVector separationSteering = FVector::ZeroVector);
};
>>>>>>> origin/simple-basic-algorithm
