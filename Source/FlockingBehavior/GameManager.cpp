// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"

// Sets default values
AGameManager::AGameManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = true;

	// RootComp = CreateDefaultSubobject<USceneComponent>("Root");
	// RootComponent = RootComp;

	// SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	// SpringArm->SetupAttachment(RootComponent);

	// Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	// Camera->SetupAttachment(SpringArm);

	Flock = CreateDefaultSubobject<UFlock>("Flock");
	AddInstanceComponent(Flock);
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	//this needs to be in BeginPlay because values set from editor are initliazed after the constructor is called.
	Flock->Initialize(this, _boidBPClass);
}

void AGameManager::Tick(float DeltaTime)
{
	if (Flock != nullptr)
	{
		Flock->TickComponent(DeltaTime);
	}
}