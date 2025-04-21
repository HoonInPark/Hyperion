// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerState.h"

void AHyperionPlayerState::OnNotify_Implementation(const FVector& _UpdateData)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerState %s: Player Location Updated to %s"), *GetName(), *(_UpdateData.ToString()));
}