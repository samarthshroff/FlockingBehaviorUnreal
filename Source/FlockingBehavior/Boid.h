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

	FVector GetVelocity();

private:

	static float _speed;

	//speed * direction vector
	UPROPERTY()
	FVector _velocity;

	UPROPERTY()
	FVector _actorBounds;

	UPROPERTY()
	UWorld* _world;

	UPROPERTY()
	FVector _directionVector;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	void Tick(float DeltaTime, float alignmentAngle, FVector alignment, FVector cohesion, FVector separation);

  UFUNCTION()
     void OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
};

//USTRUCT()
//struct FNeighborData
//{
//	float DirectionAngle;
//	FVector Location;
//	float Distance;
//};
