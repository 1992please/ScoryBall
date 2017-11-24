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
#include "LaserProjectile.h"
#include <DrawDebugHelpers.h>

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
	m_LaserRange = 10000.0f;
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	RootComponent = TurretMesh;
	TurretMesh->SetSimulatePhysics(true);
	TurretMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	SetActorScale3D(FVector(1.0f));

	LaserTarget = CreateDefaultSubobject<USceneComponent>(TEXT("LaserTarget"));
	LaserTarget->SetupAttachment(TurretMesh);

	LaserBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserBeam"));
	LaserBeam->SetupAttachment(TurretMesh, s_LaserSocketName);
	LaserBeam->SetWorldScale3D(FVector(1));

	RightGunMuzzle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RightGunMuzzle"));
	RightGunMuzzle->SetupAttachment(TurretMesh, s_RightGunSocketName);
	RightGunMuzzle->SetWorldScale3D(FVector(.1f));
	RightGunMuzzle->SetAutoActivate(false);

	LeftGunMuzzle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftGunMuzzle"));
	LeftGunMuzzle->SetupAttachment(TurretMesh, s_LeftGunSocketName);
	LeftGunMuzzle->SetWorldScale3D(FVector(.1f));
	LeftGunMuzzle->SetAutoActivate(false);

	GunSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Gun Sound"));
	GunSound->SetupAttachment(TurretMesh, s_LaserSocketName);
	GunSound->SetAutoActivate(false);

	TurretVoice = CreateDefaultSubobject<UAudioComponent>(TEXT("TurretVoice"));
	TurretVoice->SetupAttachment(TurretMesh, s_LaserSocketName);
	TurretVoice->SetAutoActivate(false);

	//LaserBeam->SetRelativeLocation(LaserBeam->GetSocketLocation("head_socket"));

	m_CurrentState = ETurretState::InActive;
	m_BulletSpeed = 1000.0f;
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

FVector ATurret::GetPlayerPredictedLocalDirection() const
{
	return TurretMesh->GetSocketTransform(s_LaserSocketName).InverseTransformPosition(m_PlayerPredictedLocation).GetSafeNormal();
}

void ATurret::Shoot(float DeltaTime)
{
	m_LastShotTime += DeltaTime;
	if (m_LastShotTime > m_ShootingCoolDown)
	{
		GunSound->Activate(true);
		if (bRightGunTurn)
		{
			RightGunMuzzle->ActivateSystem(true);
			SpawnProjectileAtLocation(TurretMesh->GetSocketLocation(s_RightGunSocketName));
		}
		else
		{
			LeftGunMuzzle->ActivateSystem(true);
			SpawnProjectileAtLocation(TurretMesh->GetSocketLocation(s_LeftGunSocketName));
		}
		// Adding Shooting FX here
		bRightGunTurn = !bRightGunTurn;

		m_LastShotTime = 0;
	}
}

void ATurret::SpawnProjectileAtLocation(FVector SpawnLocation)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		FTransform ProjectileTransform;
		ProjectileTransform.SetLocation(SpawnLocation);
		if (m_ProjectileClass != NULL)
		{
			ALaserProjectile* Projectile = World->SpawnActorDeferred<ALaserProjectile>(
				m_ProjectileClass,
				ProjectileTransform,
				this,
				Instigator,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Projectile)
			{
				const FVector VelocityDirection = (m_PlayerPredictedLocation - SpawnLocation).GetSafeNormal();
				Projectile->SetProjectileInitialVelocity(m_BulletSpeed * VelocityDirection);
				Projectile->SetDamage(m_ShotDamage);
				Projectile->FinishSpawning(ProjectileTransform);
			}
		}
	}
}

void ATurret::UpdatePredictedPlayerDirection()
{
	// We will get the new location only according the right gun and use that for left gun
	const FVector DirectionOfTarget = m_Player->GetActorLocation() - TurretMesh->GetSocketLocation(bRightGunTurn?s_RightGunSocketName:s_LeftGunSocketName);
	const FVector TargetVelocity = m_Player->GetVelocity();
	const float ShootingSpeed = m_BulletSpeed;
	// Get the parameters of the quadratic equation to solve
	const float C = FVector::DotProduct(DirectionOfTarget, DirectionOfTarget);
	const float B = 2 * FVector::DotProduct(TargetVelocity, DirectionOfTarget);
	const float A = FVector::DotProduct(TargetVelocity, TargetVelocity) - ShootingSpeed * ShootingSpeed;

	float TimeToReachTarget = GetBestQuadraticSolution(A, B, C);
	m_PlayerPredictedLocation = m_Player->GetActorLocation() + TargetVelocity * TimeToReachTarget;
	DrawDebugSphere(GetWorld(), m_PlayerPredictedLocation, 20, 32, FColor::Red);

}

float ATurret::GetBestQuadraticSolution(const float a, const float b, const float c)
{
	float discriminant = b*b - 4 * a*c;
	if (discriminant > 0)
	{
		float srqrtDisc = sqrt(discriminant);
		float x1 = (-b + srqrtDisc) / (2 * a);
		float x2 = (-b - srqrtDisc) / (2 * a);
		// x2 will be returned only if it's the only positive or the lowest positive
		if ((x1 < 0.0 && x2 > 0.0) || (x1 > 0 && x2 > 0 && x1 > x2))
			return x2;
		else
			return x1;
	}
	else if (discriminant == 0)
	{
		return -b / (2 * a);
	}
	return -1.0f;
}

void ATurret::UpdatePlayerVariables()
{
	if(!m_Player)
		return;
	// Reset the variable to false
	bPlayerInSight = false;

	// Get Player Direction with respect to the socket of the turret
	const FVector PlayerLocation = m_Player->GetActorLocation();
	const FTransform SockTrans = TurretMesh->GetSocketTransform(s_LaserSocketName);
	const FVector PlayerLocalDirection = SockTrans.InverseTransformPosition(PlayerLocation).GetSafeNormal();

	// Get the angle of the direction
	FRotator PlayerAngle = PlayerLocalDirection.Rotation();
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
	FVector StartPoint = TurretMesh->GetSocketLocation(s_LaserSocketName);
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
				//GunSound->Deactivate();
			}
			else
			{
				UpdatePredictedPlayerDirection();
				Shoot(DeltaTime);
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
		ECC_Turret,
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
