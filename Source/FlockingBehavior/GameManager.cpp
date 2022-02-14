// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = true;

	_startAI = false;

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
	if (_startAI && Flock != nullptr)
	{
		Flock->TickComponent(DeltaTime);
	}
}
void AGameManager::StartAI ()
{
	_startAI = true;
}

void AGameManager::SetupPlayerInputComponent(class UInputComponent *InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("StartAI", IE_Pressed, this, &AGameManager::StartAI);
}