// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionClientSocket.h"

#include "HyperionCharacter.h"
#include "RemoteCharacterManager.h"
#include "HyperionPlayerController.h"

int32 UHyperionClientSocket::ActivateThreads(APawn* aPawn)
{
	if (AHyperionCharacter* pChar = Cast<AHyperionCharacter>(aPawn))
	{
		return Super::ActivateThreads(aPawn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast Pawn to AHyperionCharacter"));
		return -1; // Return an error code if casting fails
	}
}

int32 UHyperionClientSocket::DeactivateThreads()
{
	return Super::DeactivateThreads();
}

void UHyperionClientSocket::OnConnect()
{
	BindRecv();

	m_pPack = new Packet;
}

void UHyperionClientSocket::OnClose()
{
	if (m_pPack)
	{
		delete m_pPack;
		m_pPack = nullptr;
	}
}

void UHyperionClientSocket::OnReceive(const UINT32 _InSize)
{
	m_pPack->Read(m_RecvBuff, _InSize);
	
	switch (m_pPack->GetMsgType())
	{
	case MsgType::MSG_INIT:
	{
		if (!IsSessionIdxSet())
		{
			SetSessionIdx(m_pPack->GetSessionIdx());
			//check(SetSessionIdx(m_pPack->GetSessionIdx()));
		}
		else
		{
			// send initial pos to be spawned in other cli's world
			auto pHyperionPlayerController = Cast<AHyperionPlayerController>(GetOwner());
			auto pPawn = pHyperionPlayerController->GetPawn();
			auto pHyperionChar = Cast<AHyperionCharacter>(pPawn);

			pHyperionChar->UpdateInitialCharData();
		}

		break;
	}
	case MsgType::MSG_GAME:
	{
		/*
		UE_LOG(LogTemp, Warning, TEXT("msg type: %d || cli idx %d || %f, %f, %f || %f, %f, %f"), 
			static_cast<int>(m_pPack->GetMsgType()),
			m_pPack->GetSessionIdx(),
			m_pPack->GetPosX(),
			m_pPack->GetPosY(),
			m_pPack->GetPosZ(),
			m_pPack->GetRotX(),
			m_pPack->GetRotY(),
			m_pPack->GetRotZ());
		*/

		auto pOwner = GetOwner();
		if (auto pController = Cast<AHyperionPlayerController>(pOwner))
		{
			pController->GetRemoteCharMng()->Replicate(m_pPack);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast Owner to AHyperionPlayerController"));
		}

		break;
	}
	default:
		break;
	}
}
