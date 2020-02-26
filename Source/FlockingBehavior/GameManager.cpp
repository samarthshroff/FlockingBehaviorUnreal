// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = true;
	
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	//RootComp = CreateDefaultSubobject<USceneComponent>("Root");
	//RootComponent = RootComp;

	//SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	//SpringArm->SetupAttachment(RootComponent);
	//SpringArm->bUsePawnControlRotation = true;

	//Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	//Camera->SetupAttachment(SpringArm);
	//Camera->bUsePawnControlRotation = false;

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

//void AGameManager::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
//{
//	check(PlayerInputComponent);
//
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	PlayerInputComponent->BindAxis("MoveForward", this, &AGameManager::MoveForward);
//	PlayerInputComponent->BindAxis("MoveRight", this, &AGameManager::MoveRight);
//}

//void AGameManager::MoveForward(float value)
//{
//	if ((Controller != NULL) && (value != 0.0f))
//	{
//		// find out which way is forward
//		const FRotator Rotation = Controller->GetControlRotation();
//		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
//		// get forward vector
//		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
//		AddMovementInput(Direction, value,true);
//		UE_LOG(FlockingBehaviorLogs, Warning, TEXT("MoveForward Direction.x: %d and value: %f"), Direction.X, value);
//	}
//}
//
//void AGameManager::MoveRight(float value)
//{
//
//}

void AGameManager::Tick(float DeltaTime)
{
	if(Flock != nullptr)
	{
		Flock->TickComponent(DeltaTime);
	}
}