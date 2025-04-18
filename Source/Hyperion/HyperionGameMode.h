// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HyperionGameMode.generated.h"

class UHealthComponent;

UCLASS(minimalapi)
class AHyperionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHyperionGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	float m_Timer{ 0.f };
	UHealthComponent* m_pHealthComp{ nullptr };
};



