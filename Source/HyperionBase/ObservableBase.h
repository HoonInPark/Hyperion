// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObservableBase.generated.h"

class IObserverBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERIONBASE_API UObservableBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObservableBase();

public:
	void Subscribe(TScriptInterface<IObserverBase> _Observer);
	void Unsubscribe(TScriptInterface<IObserverBase> _Observer);
	void NotifyObservers();

	void UpdateData(
		const TArray<bool>& _InHeader,
		const FVector& _InNewVec,
		const FRotator& _InNewRot,
		bool _bInNewInAir);

	FORCEINLINE FVector GetData() const { return m_PlayerLoc; }

private:
	TArray<TScriptInterface<IObserverBase>> Observers;

	TArray<bool> m_PlayerHeader;
	
	FVector m_PlayerLoc;
	FRotator m_PlayerRot;
	bool m_bPlayerInAir;

};
