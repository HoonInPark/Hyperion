// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"

#include "HyperionCharacter.h"
#include "ServerHyperion/Public/ClientSocket.h" 
#include "ServerHyperionLibrary/Packet.h"

AHyperionPlayerController::AHyperionPlayerController()
{
	m_pClientSocket = CreateDefaultSubobject<UClientSocket>(TEXT("ClientSocket"));
}

void AHyperionPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (m_pClientSocket->ActivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize client socket threads"));
}

void AHyperionPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (m_pClientSocket->DeactivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to deinitialize client socket threads"));
}

void AHyperionPlayerController::OnNotify_Implementation(
	const TArray<bool>& _InHeader,
	const FVector& _InNewVec,
	const FRotator& _InNewRot,
	bool _bInNewInAir)
{
	shared_ptr<Packet> pPackTmp = nullptr;
	if (m_pClientSocket->GetPackPool()->Acquire(pPackTmp))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Pop from Packet Object Pool"));
	}

	pPackTmp->SetSessionIdx(1);
	pPackTmp->SetIsJumping(_bInNewInAir);

	// Handle the notification from the observable
	if (_InHeader[static_cast<int32>(ECharStatus::E_WASD)] || _bInNewInAir)
	{
		pPackTmp->SetPosX(_InNewVec.X);
		pPackTmp->SetPosY(_InNewVec.Y);
		pPackTmp->SetPosZ(_InNewVec.Z);

		UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewVec.ToString()));
	}

	if (_InHeader[static_cast<int32>(ECharStatus::E_MOUSE)])
	{
		pPackTmp->SetRotPitch(_InNewRot.Pitch);
		pPackTmp->SetRotRoll(_InNewRot.Roll);
		pPackTmp->SetRotYaw(_InNewRot.Yaw);

		UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewRot.ToString()));
	}

	m_pClientSocket->SendPackQ_Push(pPackTmp);
}
