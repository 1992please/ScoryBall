// Fill out your copyright notice in the Description page of Project Settings.

#include "BallPlayer.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "ScoryBall.h"
#include "Battery.h"

// Sets default values
ABallPlayer::ABallPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;
	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetLinearDamping(0.5f);
	BallMesh->SetAngularDamping(0.1f);
	BallMesh->SetWorldScale3D(FVector(0.25f));
	BallMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BallMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BallMesh->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	//SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Trigger"));
	//SphereTrigger->SetupAttachment(BallMesh);
	//SphereTrigger->SetCollisionProfileName("OverlapAll");
	//SphereTrigger->SetSphereRadius(28.0f);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 300.0f;
	CameraSpringArm->SetRelativeRotation(FRotator(320.0f, 0.0f, 0.0f));
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bInheritPitch = false;
	CameraSpringArm->bInheritYaw = true;
	CameraSpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	m_TorquePower = 13000.0f;
	m_JumpPower = 4000.0f;
	m_CurrentHealthPoints =	m_MaxHealthPoints = 100.0f;
	m_EnergyCounter = 0;
}

// Called when the game starts or when spawned
void ABallPlayer::BeginPlay()
{
	Super::BeginPlay();

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

float ABallPlayer::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (m_CurrentHealthPoints > 0)
	{
		m_CurrentHealthPoints -= DamageAmount;
		if (m_CurrentHealthPoints < 0)
		{
			m_CurrentHealthPoints = 0;
			return DamageAmount + m_CurrentHealthPoints;
		}
		return DamageAmount;
	}
	return 0.0;
}

void ABallPlayer::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ABattery* Battery = Cast<ABattery>(OtherActor);
	if (Battery)
	{
		Battery->PickUp();
		m_EnergyCounter++;
	}
}

void ABallPlayer::MoveForward(float Value)
{
	m_MoveForwardAxisValue = Value;
}

void ABallPlayer::MoveRight(float Value)
{
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

	return (zVelocity > -5.0f && zVelocity < 5.0f);
}

