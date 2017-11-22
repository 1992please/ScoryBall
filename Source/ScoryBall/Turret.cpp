// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "ScoryBall.h"
#include "BallPlayer.h"

const FName TraceTag("MyTraceTag");

// Sets default values
ATurret::ATurret()
{
	bPlayerDetected = false;
	m_ShotDamage = 5.0f;
	m_LaserSocketName = "head_socket";
	m_ActivationAngle = 60.0f;
	m_ShootingCoolDown = 0.2f;
	m_LaserRange = 1000.0f;
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	RootComponent = TurretMesh;
	TurretMesh->SetSimulatePhysics(true);
	TurretMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	SetActorScale3D(FVector(4.0f));

	LaserTarget = CreateDefaultSubobject<USceneComponent>(TEXT("LaserTarget"));
	LaserTarget->SetupAttachment(TurretMesh);

	LaserBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserBeam"));
	LaserBeam->SetupAttachment(TurretMesh, m_LaserSocketName);
	LaserBeam->SetWorldScale3D(FVector(1));
	//LaserBeam->SetRelativeLocation(LaserBeam->GetSocketLocation("head_socket"));

	m_CurrentState = ETurretState::InActive;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	AnimateLaserTarget(false);
	m_Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LaserScan();
	UpdateAI(DeltaTime);
}

// Called to bind functionality to input
void ATurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurret::Shoot(float DeltaTime, bool bPlayerTakesDamage)
{
	m_LastShotTime += DeltaTime;
	if (m_LastShotTime > m_ShootingCoolDown)
	{
		if (bPlayerTakesDamage)
			m_Player->TakeDamage(m_ShotDamage, FDamageEvent(), NULL, this);
		// Adding Shooting FX here

		m_LastShotTime = 0;
	}
}

void ATurret::UpdatePlayerVariables()
{
	// Reset the variable to false
	bPlayerInSight = false;

	// Get Player Direction with respect to the socket of the torret
	const FVector PlayerLocation = m_Player->GetActorLocation();
	FTransform SockTrans = TurretMesh->GetSocketTransform(m_LaserSocketName);
	m_PlayerDirection = SockTrans.InverseTransformPosition(PlayerLocation);

	// Get the angle of the direction
	FRotator PlayerAngle = m_PlayerDirection.Rotation();
	float Angle = FMath::Sqrt(FMath::Square(PlayerAngle.Pitch) + FMath::Square(PlayerAngle.Yaw));

	if (Angle < m_ActivationAngle)
	{
		FHitResult HitOut;
		if (Trace(SockTrans.GetLocation(), PlayerLocation, HitOut))
		{
			bPlayerInSight = true;
		}
	}

}

void ATurret::LaserScan()
{
	// Start Point
	FVector StartPoint = TurretMesh->GetSocketLocation(m_LaserSocketName);
	// Laser Direction
	FVector LaserDirection;
	if (bPlayerDetected)
	{
		LaserDirection = m_Player->GetActorLocation() - StartPoint;
	}
	else
	{
		LaserDirection = LaserTarget->GetComponentLocation() - StartPoint;
	}
	// End Point
	FVector EndPoint = StartPoint + LaserDirection * m_LaserRange;
	// first set the location of the laser target point

	FHitResult HitOut;
	if (Trace(StartPoint, EndPoint, HitOut))
	{
		// Player Detected
		bPlayerDetected = true;
	}

	if (HitOut.bBlockingHit)
	{
		LaserBeam->SetBeamTargetPoint(0, HitOut.ImpactPoint, 0);
		m_DetectedActor = HitOut.GetActor();
	}
	else
	{
		LaserBeam->SetBeamTargetPoint(0, EndPoint, 0);
		m_DetectedActor = NULL;
	}
}

void ATurret::SetNewState(ETurretState NewState)
{
	if (NewState == m_CurrentState)
		return;

	switch (NewState)
	{
		case ETurretState::InActive:
		{
			bPlayerDetected = false;
			AnimateLaserTarget(false);
			m_CurrentState = NewState;
		}
		break;
		case ETurretState::Searching:
		{
			bPlayerDetected = false;
			AnimateLaserTarget(true);
			m_CurrentState = NewState;
		}
		break;
		case ETurretState::Shooting:
		{
			AnimateLaserTarget(false);
			m_LastShotTime = 0;
			m_CurrentState = NewState;

		}
		break;
		default:
			break;
	}
}

void ATurret::UpdateAI(float DeltaTime)
{
	UpdatePlayerVariables();

	switch (m_CurrentState)
	{
		case ETurretState::InActive:
		{
			if (bPlayerInSight)
			{
				SetNewState(ETurretState::Searching);
			}
		}
		break;
		case ETurretState::Searching:
		{
			if (!bPlayerInSight)
			{
				SetNewState(ETurretState::InActive);
			}
			else if (bPlayerDetected)
			{
				SetNewState(ETurretState::Shooting);
			}
		}
		break;
		case ETurretState::Shooting:
		{
			if (!bPlayerInSight)
			{
				SetNewState(ETurretState::InActive);
			}
			else
			{
				Shoot(DeltaTime, true);
			}
		}
		break;
	}
}

bool ATurret::Trace(FVector Start, FVector End, FHitResult& HitOut)
{
	FCollisionQueryParams TraceParams;
	TraceParams.TraceTag = TraceTag;
	TraceParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(
		HitOut,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	))
	{
		ABallPlayer* Player = Cast<ABallPlayer>(HitOut.GetActor());

		if (Player)
		{
			return true;
		}
	}
	return false;
}
