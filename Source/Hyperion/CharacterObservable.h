// F1ill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperionBase/ObservableBase.h"
#include "CharacterObservable.generated.h"

/**
 *
 */
UCLASS()
class HYPERION_API UCharacterObservable : public UObservableBase
{
	GENERATED_BODY()

public:
	void UpdateData(
		const TArray<bool>& _InHeader,
		const FVector& _InNewVec,
		const FRotator& _InNewRot,
		bool _bInNewInAir);

	const TArray<bool>& GetPlayerHeader() { return m_PlayerHeader; }

	const FVector& GetPlayerLoc() { return m_PlayerLoc; }
	const FRotator& GetPlayerRot() { return m_PlayerRot; }
	const bool GetPlayerInAir() { return m_bPlayerInAir; }


private:
	TArray<bool> m_PlayerHeader;

	FVector m_PlayerLoc;
	FRotator m_PlayerRot;
	bool m_bPlayerInAir;

};
