// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ScoryBallGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class SCORYBALL_API AScoryBallGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void OnConstruction(const FTransform& Transform) override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization")
	bool bInitializeMapFromFile;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	FString FilePath;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	TSubclassOf<class AStaticMeshActor> GroundCell;



};
