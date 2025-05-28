// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HyperionGameMode.generated.h"


/*
* 
*/

class UUIController;
class UUIModel;
class UUIViewer;

UCLASS(minimalapi)
class AHyperionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHyperionGameMode();

	virtual void BeginPlay() override;

private:
	UUIController* m_pUIController;
	UUIModel* m_pUIModel;
	UUIViewer* m_pUIViewer;
};
