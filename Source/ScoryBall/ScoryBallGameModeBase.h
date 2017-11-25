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
	AScoryBallGameModeBase();

	virtual void PreInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;

	void BatteryPickedUp(int32 X, int32 Y);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization")
	bool bInitializeMapFromFile;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	FString FilePath;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	TSubclassOf<class AStaticMeshActor> GroundCell;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	TSubclassOf<class ATurret> TurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	TSubclassOf<class ABattery> BatteryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	float m_CellSize;

	UPROPERTY(EditDefaultsOnly, Category = "Map Initialization", meta = (EditCondition = "bInitializeMapFromFile"))
	float m_SpawningBatteriesInterval;

	FORCEINLINE bool ReadTextFromFile(const FString& FilePath, FString& Text);
	
	FORCEINLINE void FillBoard(const FString InText);

	FORCEINLINE void SpawnBoard();

	FORCEINLINE AActor* SpawnElement(UClass* Class, int32 RowNum, int32 ColNum, int32 Level);

	void SpawnBatteries();

	struct FBoard* m_Board;
	float m_SpawnBatteriesTimer;
};
