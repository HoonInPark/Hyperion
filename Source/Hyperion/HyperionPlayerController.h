// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HyperionBase/ObserverBase.h"
#include <vector>
#include "HyperionPlayerController.generated.h"

using namespace std;

/**
 * 
 */
UCLASS()
class HYPERION_API AHyperionPlayerController : public APlayerController, public IObserverBase
{
	GENERATED_BODY()
	
	virtual void OnNotify_Implementation(
		const vector<bool>& _InHeader,
		const FVector& _InNewVec,
		const FRotator& _InNewRot,
		bool _bInNewInAir) override;
};
