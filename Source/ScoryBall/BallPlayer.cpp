// Fill out your copyright notice in the Description page of Project Settings.

#include "BallPlayer.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "ScoryBall.h"
// Sets default values
ABallPlayer::ABallPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;
	BallMesh->SetSimulatePhysics(true);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 500.0f;
	CameraSpringArm->SetRelativeRotation(FRotator(320.0f, 0.0f, 0.0f));
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bInheritPitch = false;
	CameraSpringArm->bInheritYaw = true;
	CameraSpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ABallPlayer::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogBall, Warning, TEXT("Hello: %s"), *GetNameSafe(this));



}

// Called every frame
void ABallPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("MoveForward", this, &ABallPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABallPlayer::MoveRight);
}

void ABallPlayer::MoveForward(float Value)
{
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("MoveForward Value: %s"), *FString::SanitizeFloat(Value));
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, DebugMsg);
	}
}

void ABallPlayer::MoveRight(float Value)
{
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("MoveForward Value: %s"), *FString::SanitizeFloat(Value));
		GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Green, DebugMsg);
	}
}

