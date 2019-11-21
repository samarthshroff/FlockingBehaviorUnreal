// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Boid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(FlockingBehaviorLogs, Log, All)

UCLASS()
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boid")
	FVector DirectionVector;

	//current speed by which each boid moves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float Speed;

	//All other boids within this radius are the neighbors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float NeighborRadius;

	//The Speed variable will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSpeed;

	//Steering will never cross this limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	float MaxSteeringForce;

private:

	//speed * direction vector
	FVector _velocity;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
