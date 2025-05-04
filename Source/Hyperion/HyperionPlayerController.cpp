// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"
#include "HyperionCharacter.h"
//#include "ServerHyperionLibrary/Serializer.h" 

void AHyperionPlayerController::OnNotify_Implementation(
	const TArray<bool>& _InHeader,
	const FVector& _InNewVec,
	const FRotator& _InNewRot,
	bool _bInNewInAir)
{
	// Handle the notification from the observable
	if (_InHeader[static_cast<int32>(ECharStatus::E_WASD)]) UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewVec.ToString()));
	if (_InHeader[static_cast<int32>(ECharStatus::E_AIR)]) UE_LOG(LogTemp, Error, TEXT("Start in the air!!!"));
}