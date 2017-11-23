// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	InActive,
	Searching,
	Shooting
};


UCLASS()
class SCORYBALL_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATurret();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	UFUNCTION(BlueprintPure, Category=AI)
	FORCEINLINE ETurretState GetCurrentState() const { return m_CurrentState;}

	UFUNCTION(BlueprintPure, Category = AI)
	FORCEINLINE FVector GetPlayerDirection() const { return  m_PlayerDirection;}

	UFUNCTION(BlueprintPure, Category = AI)
	FORCEINLINE FVector GetPredictedPlayerDirection() const { return  m_PlayerPredictedDirection; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category=AI )
	void AnimateLaserTarget(bool bPlay);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* m_TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Particle, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* m_LaserTarget;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* m_LaserBeam;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* m_RightGunMuzzle;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* m_LeftGunMuzzle;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* m_GunSound;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* m_TurretVoice;

	UPROPERTY(VisibleAnywhere, Category = AI, meta = (AllowPrivateAccess = "true"))
	float m_ScanningSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	TSubclassOf<class ALaserProjectile> m_ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category=AI)
	float m_ActivationAngle;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float m_LaserRange;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_ShootingCoolDown;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_ShotDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_ShootingSpeed;



	class APawn* m_Player;
	class AActor* m_DetectedActor;

	bool bPlayerDetected;

	ETurretState m_CurrentState;

	// Shooting -----------------------------------------------------------------
	bool bPlayerInSight;
	FVector m_PlayerDirection;
	FVector m_PlayerPredictedDirection;
	float m_LastShotTime;
	void Shoot(float DeltaTime, bool bPlayerTakesDamage);
	void SpawnProjectileAtPlayer(FVector SpawnLocation);
	void UpdatePredictedPlayerDirection();

	// AI ----------------------------------------------------------------------
	void UpdatePlayerVariables();
	void LaserScan();
	void SetNewState(ETurretState NewState);
	void UpdateAI(float DeltaTime);
	bool Trace(FVector Start, FVector End, FHitResult& HitOut);


};
