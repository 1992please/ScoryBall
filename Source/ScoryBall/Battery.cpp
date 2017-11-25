// Fill out your copyright notice in the Description page of Project Settings.

#include "Battery.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ScoryBallGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABattery::ABattery()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->SetSimulatePhysics(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionProfileName("NoCollision");
}

// Called when the game starts or when spawned
void ABattery::BeginPlay()
{
	Super::BeginPlay();
	X = 0;
	Y = 0;
}

// Called every frame
void ABattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattery::PickUp()
{
	AScoryBallGameModeBase* GameMode = Cast<AScoryBallGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(GameMode)
		GameMode->BatteryPickedUp(X, Y);
	Destroy();
}

