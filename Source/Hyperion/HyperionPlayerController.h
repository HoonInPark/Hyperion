// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HyperionBase/ObserverBase.h"
#include "HyperionBase/ObservableBase.h"
#include "HyperionPlayerController.generated.h"

class UHyperionClientSocket;
class URemoteCharacterManager;
/**
 * 
 */
UCLASS()
class HYPERION_API AHyperionPlayerController : public APlayerController, public IObserverBase
{
	GENERATED_BODY()
	
public:
	AHyperionPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	virtual void OnNotify_Implementation(UObservableBase* _pInObservable) override;

	FORCEINLINE URemoteCharacterManager* GetRemoteCharMng() const { return m_pRemoteCharMng; }

protected:
	UPROPERTY(EditAnywhere, Category = "RemoteCharacterManager")
	URemoteCharacterManager* m_pRemoteCharMng{ nullptr };

private:
	UHyperionClientSocket* m_pClientSock{ nullptr };
};
