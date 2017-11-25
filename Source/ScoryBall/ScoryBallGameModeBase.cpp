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

AScoryBallGameModeBase::AScoryBallGameModeBase()
{
	bInitializeMapFromFile = false;
	FilePath = "level_data/map_data.dat";
	m_Board = new FBoard();
}

void AScoryBallGameModeBase::PreInitializeComponents()
{
	if (!bInitializeMapFromFile)
		return;

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

	AGameModeBase::PreInitializeComponents();
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
	if(!TurretClass || !GroundCell)
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
}

FORCEINLINE void AScoryBallGameModeBase::SpawnElement(UClass* Class, int32 RowNum, int32 ColNum, int32 Level)
{
	FVector SpawnLocation(RowNum * CellSize, ColNum * CellSize, Level * CellSize);

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
			World->SpawnActor<AActor>(
				Class,
				CellTransform,
				SpawnParams);
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
