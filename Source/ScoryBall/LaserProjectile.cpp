// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BallPlayer.h"
#include "ScoryBall.h"
// Sets default values
ALaserProjectile::ALaserProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Capsule"));
	RootComponent = SphereCollider;
	SphereCollider->SetSphereRadius(4.0f);
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Block);
	SphereCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SphereCollider->SetCollisionResponseToChannel(ECC_Turret, ECR_Ignore);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->Velocity = FVector(1.0f, 0.0f, 0.0f);
	ProjectileMovement->InitialSpeed = 0.0f; // we don't want it to treat the velocity as direction
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	m_LifeTime = 5.0f;
	m_LifeTimeCounter = 0.0f;
	m_Damage = 5;
}

// Called when the game starts or when spawned
void ALaserProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void ALaserProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	ABallPlayer* Player = Cast<ABallPlayer>(Other);
	if (Player)
	{
		Player->TakeDamage(m_Damage, FDamageEvent(), NULL, this);
		Destroy();
	}
}

// Called every frame
void ALaserProjectile::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	m_LifeTimeCounter += DeltaTime;
	if (m_LifeTimeCounter >= m_LifeTime)
		Destroy();
}

void ALaserProjectile::SetProjectileInitialSpeed(float Speed)
{
	ProjectileMovement->InitialSpeed = Speed;
}

void ALaserProjectile::SetProjectileInitialVelocity(FVector Velocity)
{
	ProjectileMovement->Velocity = Velocity;
}

void ALaserProjectile::SetDamage(float Damage)
{
	m_Damage = Damage;
}

