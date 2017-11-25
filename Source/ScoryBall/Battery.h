// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Battery.generated.h"

UCLASS()
class SCORYBALL_API ABattery : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattery();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PickUp();

	int32 X;
	int32 Y;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
};
