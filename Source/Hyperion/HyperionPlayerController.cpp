// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"
#include "ServerHyperionLibrary/Serializer.h" 

void AHyperionPlayerController::OnNotify_Implementation(const FVector& _UpdateData)
{
	// Handle the notification from the observable
	UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_UpdateData.ToString()));

	if (test)
	{
		auto pSerializer = new Serializer(true);
		test = false;
	}
} 