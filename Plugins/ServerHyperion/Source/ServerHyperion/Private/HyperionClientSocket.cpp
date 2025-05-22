// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionClientSocket.h"

void UHyperionClientSocket::OnConnect()
{
	BindRecv();

	m_pPack = new Packet;
}

void UHyperionClientSocket::OnClose()
{
}

void UHyperionClientSocket::OnReceive(const UINT32 _InSize)
{
	m_pPack->Read(m_RecvBuff, _InSize);

	UE_LOG(LogTemp, Warning, TEXT("Echoing Player : %f, %f, %f || %f, %f, %f"), 
		m_pPack->GetPosX(),
		m_pPack->GetPosY(),
		m_pPack->GetPosZ(),
		m_pPack->GetRotX(),
		m_pPack->GetRotY(),
		m_pPack->GetRotZ());
}
