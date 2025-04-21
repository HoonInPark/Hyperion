// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HyperionBase/ObserverBase.h"
#include "HyperionPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HYPERIONMULTI_API AHyperionPlayerState : public APlayerState, public IObserverBase
{
	GENERATED_BODY()
	
	virtual void OnNotify_Implementation(const FVector& _UpdateData) override;
	
};
