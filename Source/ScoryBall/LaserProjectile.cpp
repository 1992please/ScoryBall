// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
// Sets default values
ALaserProjectile::ALaserProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Capsule"));
	RootComponent = SphereCollider;
	SphereCollider->SetSphereRadius(5.0f);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->Velocity = FVector(1.0f, 0.0f, 0.0f);
	ProjectileMovement->InitialSpeed = 0.0f; // we don't want it to treat the velocity as direction
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	m_LifeTime = 5.0f;
	m_LifeTimeCounter = 0.0f;
}

// Called when the game starts or when spawned
void ALaserProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_LifeTimeCounter += DeltaTime;
	if(m_LifeTimeCounter >= m_LifeTime)
		Destroy();
}

void ALaserProjectile::SetProjectileSpeed(float Speed)
{
	ProjectileMovement->InitialSpeed = Speed;
}

