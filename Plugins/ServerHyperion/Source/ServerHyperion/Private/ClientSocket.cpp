// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"



// Sets default values for this component's properties
UClientSocket::UClientSocket()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UClientSocket::Activate(bool bReset)
{
	Super::Activate(bReset);


}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Send::FClientRunnable_Send()
{
}

FClientRunnable_Send::~FClientRunnable_Send()
{
}

bool FClientRunnable_Send::Init()
{
	return false;
}

uint32 FClientRunnable_Send::Run()
{
	while (m_bIsRunning)
	{

	}

	return uint32();
}

void FClientRunnable_Send::Stop()
{
}

void FClientRunnable_Send::Exit()
{
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Recv::FClientRunnable_Recv()
{
}

FClientRunnable_Recv::~FClientRunnable_Recv()
{
}

bool FClientRunnable_Recv::Init()
{
	return false;
}

uint32 FClientRunnable_Recv::Run()
{
	while (m_bIsRunning)
	{

	}

	return uint32();
}

void FClientRunnable_Recv::Stop()
{
}

void FClientRunnable_Recv::Exit()
{
}
