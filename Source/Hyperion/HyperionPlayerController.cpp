// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionPlayerController.h"

#include "HyperionCharacter.h"
#include "ServerHyperion/Public/ClientSocket.h" 
#include "ServerHyperionLibrary/Packet.h"

AHyperionPlayerController::AHyperionPlayerController()
{
	m_pClientSock = CreateDefaultSubobject<UClientSocket>(TEXT("ClientSocket"));
}

void AHyperionPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (0 != m_pClientSock->ActivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize client socket threads"));
}

void AHyperionPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (0 != m_pClientSock->DeactivateThreads())
		UE_LOG(LogTemp, Error, TEXT("Failed to deinitialize client socket threads"));
}

void AHyperionPlayerController::OnNotify_Implementation(
	const TArray<bool>& _InHeader,
	const FVector& _InNewVec,
	const FRotator& _InNewRot,
	bool _bInNewInAir)
{
	m_CS.Lock();
	
	shared_ptr<Packet> pPack = m_pClientSock->GetSendPackPool().Acquire();

	if (!pPack)
	{
		m_CS.Unlock();
		UE_LOG(LogTemp, Error, TEXT("Failed to Pop from Packet Object Pool"));
		return;
	}

	m_CS.Unlock();

	pPack->SetSessionIdx(2);
	pPack->SetIsJumping(_bInNewInAir);

	// Handle the notification from the observable
	if (_InHeader[static_cast<int32>(ECharStatus::E_WASD)] || _bInNewInAir)
	{
		pPack->SetPosX(_InNewVec.X);
		pPack->SetPosY(_InNewVec.Y);
		pPack->SetPosZ(_InNewVec.Z);

		UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewVec.ToString()));
	}

	if (_InHeader[static_cast<int32>(ECharStatus::E_MOUSE)])
	{
		pPack->SetRotPitch(_InNewRot.Pitch);
		pPack->SetRotRoll(_InNewRot.Roll);
		pPack->SetRotYaw(_InNewRot.Yaw);

		UE_LOG(LogTemp, Warning, TEXT("Player Location Updated: %s"), *(_InNewRot.ToString()));
	}

	m_CS.Lock();
	m_pClientSock->SendPackQ_Push(pPack);
	m_CS.Unlock();
}
