// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterObservable.h"


void UCharacterObservable::UpdateData(
	const TArray<bool>& _InHeader,
	const FVector& _InNewVec,
	const FRotator& _InNewRot,
	bool _bInNewInAir)
{
	m_PlayerHeader = _InHeader;

	m_PlayerLoc = _InNewVec;
	m_PlayerRot = _InNewRot;
	m_bPlayerInAir = _bInNewInAir;

	NotifyObservers();
}
