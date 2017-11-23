// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserProjectile.generated.h"

UCLASS()
class SCORYBALL_API ALaserProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetProjectileSpeed(float Speed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Laser Projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollider;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float m_LifeTime;

	float m_LifeTimeCounter;
};
