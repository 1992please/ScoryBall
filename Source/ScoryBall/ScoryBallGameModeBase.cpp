// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoryBallGameModeBase.h"
#include "ScoryBall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "GenericPlatformFile.h"
#include "PlatformFilemanager.h"

static FORCEINLINE bool VerifyOrCreateDirectory(const FString& TestDir)
{
	// Every function call, unless the function is inline, adds a small
	// overhead which we can avoid by creating a local variable like so.
	// But beware of making every function inline!
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Directory Exists?
	if (!PlatformFile.DirectoryExists(*TestDir))
	{
		PlatformFile.CreateDirectory(*TestDir);

		if (!PlatformFile.DirectoryExists(*TestDir))
		{
			UE_LOG(LogLevelBuilding, Warning, TEXT("nope"));
			
			return false;
			//~~~~~~~~~~~~~~

		}
	}
	UE_LOG(LogLevelBuilding, Warning, TEXT("yupe: %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForRead(*TestDir));

	return true;
}

void AScoryBallGameModeBase::OnConstruction(const FTransform& Transform)
{
	//VerifyOrCreateDirectory("Hello World");

	if(!bInitializeMapFromFile)
		return;
	UE_LOG(LogLevelBuilding, Warning, TEXT("hey bastard"));
	AGameModeBase::OnConstruction(Transform);

	UWorld* const World = GetWorld();
	if (World)
	{
		FTransform CellTransform;

		CellTransform.SetLocation(FVector(0.0f));
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = this;

		if (GroundCell != NULL)
		{
			World->SpawnActor<AStaticMeshActor>(
				GroundCell,
				CellTransform,
				SpawnParams);
		}
	}
	
}
