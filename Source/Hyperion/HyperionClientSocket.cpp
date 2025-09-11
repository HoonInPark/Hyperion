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
		SetSessionIdx(m_pPack->GetSessIdx());
		break;
	}
	case MsgType::MSG_SPAWN:
	{
		if (m_pPack->GetSessIdx() == GetSessIdx())
		{
			break;
		}

		// TODO : 이미 사람들이 접속해 있는 방에 새로 들어올 때, 현재는 replicate되고 있는지 여부를 확인해서 없으면 새로 스폰하는 식으로 메시지를 처리하지만, 
		// TODO : 앞으로는 새로 접속한 사용자는 기존의 방에 누가 있는지 메시지로 쏴서 스폰하도록 명령을 줘야 한다. 
		auto pOwner = GetOwner();
		if (auto pController = Cast<AHyperionPlayerController>(pOwner))
		{
			pController->GetRemoteCharMng()->Spawn(m_pPack);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast Owner to AHyperionPlayerController"));
		}
		break;
	}
	case MsgType::MSG_GAME:
	{
		if (m_pPack->GetSessIdx() == GetSessIdx())
		{
			break;
		}

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
	case MsgType::MSG_CLOSE:
	{
		auto pOwner = GetOwner();
		if (auto pController = Cast<AHyperionPlayerController>(pOwner))
		{
			pController->GetRemoteCharMng()->DestroyReplicant(m_pPack);
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
