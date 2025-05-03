// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"
//#include "ServerHyperionLibrary/Serializer.h" 

void AHyperionPlayerController::OnNotify_Implementation(
	const TArray<bool>& _InHeader,
	const FVector& _InNewVec,
	const FRotator& _InNewRot,
	bool _bInNewInAir)
{
	// Handle the notification from the observable
	UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_UpdateData.ToString()));
}