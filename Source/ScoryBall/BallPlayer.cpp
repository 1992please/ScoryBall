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
	BallMesh->SetLinearDamping(1.0f);
	BallMesh->SetAngularDamping(0.1f);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 1000.0f;
	CameraSpringArm->SetRelativeRotation(FRotator(320.0f, 0.0f, 0.0f));
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bInheritPitch = false;
	CameraSpringArm->bInheritYaw = true;
	CameraSpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	m_TorquePower = 1300000.0f;
	m_JumpPower = 70000.0f;
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
	UpdateBallMovement();
}

// Called to bind functionality to input
void ABallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("MoveForward", this, &ABallPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABallPlayer::MoveRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABallPlayer::Jump);
}

void ABallPlayer::MoveForward(float Value)
{

	m_MoveForwardAxisValue = Value;
}

void ABallPlayer::MoveRight(float Value)
{
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("MoveRight Value: %s"), *FString::SanitizeFloat(Value));
		GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Green, DebugMsg);
	}
	m_MoveRightAxisValue = Value;
}

void ABallPlayer::Jump()
{
	if (BallMesh && IsGrounded())
	{
		BallMesh->AddImpulse(FVector(0.0f, 0.0f, m_JumpPower));
	}
}

void ABallPlayer::UpdateBallMovement()
{
	const FRotator lYawRotation(0.0, this->GetControlRotation().Yaw, 0.0);

	FVector lForward = FRotationMatrix(lYawRotation).GetUnitAxis(EAxis::Y) * m_MoveForwardAxisValue;
	FVector lRight = -FRotationMatrix(lYawRotation).GetUnitAxis(EAxis::X) * m_MoveRightAxisValue;

	if (BallMesh)
	{
		BallMesh->AddTorque((lForward + lRight) * m_TorquePower);
	}
}

bool ABallPlayer::IsGrounded()
{
	float zVelocity = GetVelocity().Z;
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Forward Value: %s"), *FString::SanitizeFloat(zVelocity));
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, DebugMsg);
	}

	return (zVelocity > -5.0f && zVelocity < 5.0f);
}

