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

	UFUNCTION(BlueprintPure, Category=AI)
	ETurretState GetCurrentState() const { return m_CurrentState;}
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

	UPROPERTY(VisibleAnywhere, Category=AI, meta = (AllowPrivateAccess = "true"))
	float m_ScanningSpeed;

	UPROPERTY(VisibleAnywhere, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* LaserBeam;

	UPROPERTY(EditDefaultsOnly, Category=Mesh)
	FName m_LaserSocketName;

	UPROPERTY(EditDefaultsOnly, Category=AI)
	float m_ActivationAngle;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float m_LaserRange;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_ShootingCoolDown;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float m_ShotDamage;



	class APawn* m_Player;
	class AActor* m_DetectedActor;

	bool bPlayerDetected;

	ETurretState m_CurrentState;

	bool bPlayerInSight;
	FVector m_PlayerDirection;
	float m_LastShotTime;
	void Shoot(float DeltaTime, bool bPlayerTakesDamage);

	void UpdatePlayerVariables();

	void LaserScan();
	void SetNewState(ETurretState NewState);
	void UpdateAI(float DeltaTime);
	bool Trace(FVector Start, FVector End, FHitResult& HitOut);
};
