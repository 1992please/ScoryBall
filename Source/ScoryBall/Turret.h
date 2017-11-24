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
	FORCEINLINE FVector GetPlayerLocation() const { return  m_Player->GetActorLocation();}

	/** Get player predicted direction in perspective to the turret laser socket */
	UFUNCTION(BlueprintPure, Category = AI)
	FVector GetPlayerPredictedLocalDirection() const;


	//UFUNCTION(BlueprintPure, Category = AI)
	//FVector GetPredictedPlayerDirection() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category=AI )
	void AnimateLaserTarget(bool bPlay);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Particle, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* LaserTarget;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* LaserBeam;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* RightGunMuzzle;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* LeftGunMuzzle;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* GunSound;

	UPROPERTY(VisibleAnywhere, Category = FX, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* TurretVoice;

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
	/** the projectile movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_BulletSpeed;



	class APawn* m_Player;
	class AActor* m_DetectedActor;

	bool bPlayerDetected;

	ETurretState m_CurrentState;

	// Shooting -----------------------------------------------------------------
	bool bPlayerInSight;
	FVector m_PlayerPredictedLocation;
	float m_LastShotTime;
	bool bRightGunTurn;
	void Shoot(float DeltaTime);
	void SpawnProjectileAtLocation(FVector SpawnLocation);
	void UpdatePredictedPlayerDirection();

	// AI ----------------------------------------------------------------------
	void UpdatePlayerVariables();
	void LaserScan();
	void SetNewState(ETurretState NewState);
	void UpdateAI(float DeltaTime);
	bool Trace(FVector Start, FVector End, FHitResult& HitOut);

	// some math statics
	static float GetBestQuadraticSolution(const float a, const float b, const float c);
};
