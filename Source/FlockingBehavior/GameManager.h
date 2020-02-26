// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Flock.h"
#include "GameManager.generated.h"

UCLASS()
class FLOCKINGBEHAVIOR_API AGameManager : public ADefaultPawn
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AGameManager();

	UPROPERTY(VisibleAnywhere, BlueprintreadOnly)
	UFlock* Flock;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABoid> _boidBPClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
