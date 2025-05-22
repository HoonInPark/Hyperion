// Fill out your copyright notice in the Description page of Project Settings.


#include "HyperionClientSocket.h"

void UHyperionClientSocket::OnConnect()
{
	BindRecv();
}

void UHyperionClientSocket::OnClose()
{
}

void UHyperionClientSocket::OnReceive(const UINT32 _InSize)
{
	Packet Pack;
	Pack.Read(m_RecvBuff, _InSize);

	UE_LOG(LogTemp, Warning, TEXT("Echoing Player : %f, %f, %f || %f, %f, %f"), 
		Pack.GetPosX(),
		Pack.GetPosY(),
		Pack.GetPosZ(),
		Pack.GetRotX(),
		Pack.GetRotY(),
		Pack.GetRotZ());
}
