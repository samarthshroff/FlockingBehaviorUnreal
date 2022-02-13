// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Boid.h"

#include "Flock.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLOCKINGBEHAVIOR_API UFlock : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock", meta = (Tooltip = "Total number of boids."))
		int TotalBoids = 300;

private:

	float _awarenessRadiusSquared;
	float _separationRadiusSquared;

	UPROPERTY()
		AActor* _boidOwner;

	UPROPERTY()
		TArray<ABoid*> _boids;

	UPROPERTY()
		TSubclassOf<class ABoid> _boidBPClass;

public:
	// Sets default values for this component's properties
	UFlock();

	UFUNCTION()
		void Initialize(class AActor* boidsOwner, TSubclassOf<class ABoid> boidBPClass);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	void TickComponent(float DeltaTime);
};
