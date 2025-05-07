// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HyperionBase/ObserverBase.h"
#include "HyperionPlayerController.generated.h"

class UClientSocket;

/**
 * 
 */
UCLASS()
class HYPERION_API AHyperionPlayerController : public APlayerController, public IObserverBase
{
	GENERATED_BODY()
	
public:
	AHyperionPlayerController();

	virtual void OnNotify_Implementation(
		const TArray<bool>& _InHeader,
		const FVector& _InNewVec,
		const FRotator& _InNewRot,
		bool _bInNewInAir) override;

private:
	UClientSocket* m_ClientSocket;
};
