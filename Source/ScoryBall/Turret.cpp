// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "ScoryBall.h"
#include "BallPlayer.h"

const FName TraceTag("MyTraceTag");

static const FName s_LaserSocketName = "head_socket";
static const FName s_RightGunSocketName = "right_gun_socket";
static const FName s_LeftGunSocketName = "left_gun_socket";

// Sets default values
ATurret::ATurret()
{
	bPlayerDetected = false;
	m_ShotDamage = 5.0f;
	m_ActivationAngle = 60.0f;
	m_ShootingCoolDown = 0.2f;
	m_LaserRange = 1000.0f;
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	RootComponent = m_TurretMesh;
	m_TurretMesh->SetSimulatePhysics(true);
	m_TurretMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	SetActorScale3D(FVector(1.0f));

	m_LaserTarget = CreateDefaultSubobject<USceneComponent>(TEXT("LaserTarget"));
	m_LaserTarget->SetupAttachment(m_TurretMesh);

	m_LaserBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserBeam"));
	m_LaserBeam->SetupAttachment(m_TurretMesh, s_LaserSocketName);
	m_LaserBeam->SetWorldScale3D(FVector(1));

	m_RightGunMuzzle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RightGunMuzzle"));
	m_RightGunMuzzle->SetupAttachment(m_TurretMesh, s_RightGunSocketName);
	m_RightGunMuzzle->SetWorldScale3D(FVector(.1f));
	m_RightGunMuzzle->SetAutoActivate(false);

	m_LeftGunMuzzle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftGunMuzzle"));
	m_LeftGunMuzzle->SetupAttachment(m_TurretMesh, s_LeftGunSocketName);
	m_LeftGunMuzzle->SetWorldScale3D(FVector(.1f));
	m_LeftGunMuzzle->SetAutoActivate(false);

	m_GunSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Gun Sound"));
	m_GunSound->SetupAttachment(m_TurretMesh, s_LaserSocketName);
	m_GunSound->SetAutoActivate(false);

	m_TurretVoice = CreateDefaultSubobject<UAudioComponent>(TEXT("TurretVoice"));
	m_TurretVoice->SetupAttachment(m_TurretMesh, s_LaserSocketName);
	m_TurretVoice->SetAutoActivate(false);

	//LaserBeam->SetRelativeLocation(LaserBeam->GetSocketLocation("head_socket"));

	m_CurrentState = ETurretState::InActive;
	m_ShootingSpeed = 1000.0f;
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

float ATurret::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	return 0.0;
}

void ATurret::Shoot(float DeltaTime, bool bPlayerTakesDamage)
{
	m_LastShotTime += DeltaTime;
	if (m_LastShotTime > m_ShootingCoolDown)
	{
		if (bPlayerTakesDamage)
			m_Player->TakeDamage(m_ShotDamage, FDamageEvent(), NULL, this);
		// Adding Shooting FX here
		m_RightGunMuzzle->ActivateSystem(true);
		//LeftGunMuzzle->ActivateSystem(true);



		SpawnProjectileAtPlayer(m_TurretMesh->GetSocketLocation(s_RightGunSocketName));

		m_LastShotTime = 0;
	}
}

void ATurret::SpawnProjectileAtPlayer(FVector SpawnLocation)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;


	}
}

void ATurret::UpdatePredictedPlayerDirection()
{
	// Todo do you implementation here
	m_PlayerPredictedDirection = m_PlayerDirection;
}

void ATurret::UpdatePlayerVariables()
{
	// Reset the variable to false
	bPlayerInSight = false;

	// Get Player Direction with respect to the socket of the turret
	const FVector PlayerLocation = m_Player->GetActorLocation();
	FTransform SockTrans = m_TurretMesh->GetSocketTransform(s_LaserSocketName);
	m_PlayerDirection = SockTrans.InverseTransformPosition(PlayerLocation).GetSafeNormal();

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
	FVector StartPoint = m_TurretMesh->GetSocketLocation(s_LaserSocketName);
	// Laser Direction
	FVector LaserDirection;
	if (bPlayerDetected)
	{
		LaserDirection = m_Player->GetActorLocation() - StartPoint;
	}
	else
	{
		LaserDirection = m_LaserTarget->GetComponentLocation() - StartPoint;
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
		m_LaserBeam->SetBeamTargetPoint(0, HitOut.ImpactPoint, 0);
		m_DetectedActor = HitOut.GetActor();
	}
	else
	{
		m_LaserBeam->SetBeamTargetPoint(0, EndPoint, 0);
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
			//GunSound->Activate(true);
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
				//GunSound->Deactivate();
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
