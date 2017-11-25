// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoryBallGameModeBase.h"
#include "ScoryBall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "GenericPlatformFile.h"
#include "PlatformFilemanager.h"
#include "FileHelper.h"
#include "Board.h"
#include "Turret.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Battery.h"

AScoryBallGameModeBase::AScoryBallGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bInitializeMapFromFile = false;
	FilePath = "level_data/map_data.dat";
	m_Board = new FBoard();
	m_CellSize = 100.0f;
	m_SpawningBatteriesInterval = 10.0f;
	m_SpawnBatteriesTimer = 0.0f;
}

void AScoryBallGameModeBase::PreInitializeComponents()
{
	if (bInitializeMapFromFile)
	{

		FString Data;
		if (ReadTextFromFile(FilePath, Data))
		{
			UE_LOG(LogLevelBuilding, Log, TEXT("Data Read Successful: %s"), *Data);
			FillBoard(Data);
			SpawnBoard();
		}
		else
		{
			UE_LOG(LogLevelBuilding, Error, TEXT("Couldn't Find File at location: %s"), *FilePath);
		}
	}
	AGameModeBase::PreInitializeComponents();
}

void AScoryBallGameModeBase::Tick(float DeltaSeconds)
{
	if (bInitializeMapFromFile)
	{
		m_SpawnBatteriesTimer += DeltaSeconds;
		if (m_SpawnBatteriesTimer > m_SpawningBatteriesInterval)
		{
			m_SpawnBatteriesTimer = 0.0f;
			SpawnBatteries();
		}

	}
}

void AScoryBallGameModeBase::BatteryPickedUp(int32 X, int32 Y)
{
	if (m_Board && m_Board->m_RowSize > 0)
	{
		(*m_Board)[X][Y].level1 = ECellType::Nothing;
	}
}

FORCEINLINE void AScoryBallGameModeBase::FillBoard(const FString InText)
{
	TArray<FString> Lines;
	int32 CurrentLineIndex = 0;
	InText.ParseIntoArrayLines(Lines);

	// Make sure the number of lines are more than 1
	if (Lines.Num() > 1)
	{
		TArray<FString> ParsedText;
		Lines[CurrentLineIndex].ParseIntoArrayWS(ParsedText);
		if (ParsedText[0] == "#map" && ParsedText[1].IsNumeric() && ParsedText[2].IsNumeric())
		{
			const int32 nRows = FCString::Atoi(*ParsedText[1]);
			const int32 nCols = FCString::Atoi(*ParsedText[2]);
			m_Board->Resize(nRows, nCols);
			// Read first level
			if (Lines.Num() > nRows)
			{
				for (int RowIndex = 0; RowIndex < nRows; RowIndex++)
				{
					CurrentLineIndex++;
					Lines[CurrentLineIndex].ParseIntoArrayWS(ParsedText);

					for (int ColIndex = 0; ColIndex < nCols; ColIndex++)
					{
						(*m_Board)[RowIndex][ColIndex].level1 = (ECellType)FCString::Atoi(*ParsedText[ColIndex]);
					}
				}
			}

			// Read second level
			if (Lines.Num() > nRows * 2)
			{
				// skip a line
				CurrentLineIndex++;
				for (int RowIndex = 0; RowIndex < nRows; RowIndex++)
				{
					CurrentLineIndex++;
					Lines[CurrentLineIndex].ParseIntoArrayWS(ParsedText);

					for (int ColIndex = 0; ColIndex < nCols; ColIndex++)
					{
						(*m_Board)[RowIndex][ColIndex].level2 = (ECellType)FCString::Atoi(*ParsedText[ColIndex]);
					}
				}
			}
		}
	}

}

FORCEINLINE void AScoryBallGameModeBase::SpawnBoard()
{
	if (!TurretClass || !GroundCell)
		return;

	for (int RowIndex = 0; RowIndex < m_Board->m_RowSize; RowIndex++)
	{
		for (int ColIndex = 0; ColIndex < m_Board->m_ColSize; ColIndex++)
		{
			SpawnElement(GroundCell, RowIndex, ColIndex, 0);
			switch ((*m_Board)[RowIndex][ColIndex].level1)
			{
				case ECellType::Block:
					SpawnElement(GroundCell, RowIndex, ColIndex, 1);
					break;
				case ECellType::Turret:
					SpawnElement(TurretClass, RowIndex, ColIndex, 1);
					break;
			}

			switch ((*m_Board)[RowIndex][ColIndex].level2)
			{
				case ECellType::Block:
					SpawnElement(GroundCell, RowIndex, ColIndex, 2);
					break;
				case ECellType::Turret:
					SpawnElement(TurretClass, RowIndex, ColIndex, 2);
					break;
			}
		}
	}

	SpawnBatteries();
}

FORCEINLINE AActor* AScoryBallGameModeBase::SpawnElement(UClass* Class, int32 RowNum, int32 ColNum, int32 Level)
{
	FVector SpawnLocation(RowNum * m_CellSize, ColNum * m_CellSize, Level * m_CellSize);

	UWorld* const World = GetWorld();
	if (World)
	{
		FTransform CellTransform;

		CellTransform.SetLocation(SpawnLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = this;

		if (Class != NULL)
		{
			return World->SpawnActor<AActor>(
				Class,
				CellTransform,
				SpawnParams);
		}
	}
	return NULL;
}

void AScoryBallGameModeBase::SpawnBatteries()
{
	TArray<FBoardCoords> BoardCoords;
	m_Board->GetEmptyCellCoords(BoardCoords);
	if (BoardCoords.Num() > 5)
	{
		// Shuffle the array
		const int32 LastIndex = BoardCoords.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(0, LastIndex);
			if (i != Index)
			{
				FBoardCoords temp = BoardCoords[i];
				BoardCoords[i] = BoardCoords[Index];
				BoardCoords[Index] = temp;
			}
		}

		// Pick first 3 locations
		for (int i = 0; i < 3; i++)
		{
			ABattery* Battery =Cast<ABattery>(SpawnElement(BatteryClass, BoardCoords[i].X, BoardCoords[i].Y, 1));
			if (Battery)
			{
				Battery->X = BoardCoords[i].X;
				Battery->Y = BoardCoords[i].Y;

				(*m_Board)[Battery->X][Battery->Y].level1 = ECellType::Battery;
			}
		}

	}
}

FORCEINLINE bool AScoryBallGameModeBase::ReadTextFromFile(const FString& FilePath, FString& Text)
{
	// Every function call, unless the function is inline, adds a small
	// overhead which we can avoid by creating a local variable like so.
	// But beware of making every function inline!
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// File Exists?
	if (PlatformFile.FileExists(*FilePath))
	{
		if (FFileHelper::LoadFileToString(Text, *FilePath))
		{
			return true;
		}

	}
	return false;
}
