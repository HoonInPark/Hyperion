// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObservableBase.generated.h"

class IObserver;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HYPERIONBASE_API UObservableBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObservableBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Subscribe(TScriptInterface<IObserver> observer);
	void Unsubscribe(TScriptInterface<IObserver> observer);
	void NotifyObservers();

	FORCEINLINE void UpdateData(FVector _NewData)
	{
		m_PlayerLoc = _NewData;

		UE_LOG(LogTemp, Warning, TEXT("Player Location: %s"), *m_PlayerLoc.ToString());
		NotifyObservers();
	}
	//int32 GetScore() const { return PlayerScore; }

private:
	TArray<TScriptInterface<IObserver>> Observers;

	FVector m_PlayerLoc;

};
