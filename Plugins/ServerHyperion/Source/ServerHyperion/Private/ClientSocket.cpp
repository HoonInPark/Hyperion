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

int32 UClientSocket::ActivateThreads()
{
	m_pClientRunnable_Send = new FClientRunnable_Send();
	m_pClientRunnable_Recv = new FClientRunnable_Recv();

	if (m_pClientRunnable_Send && m_pClientRunnable_Recv)
		return 0;
	else
		return 1;
}

int32 UClientSocket::DeactivateThreads()
{
	m_pClientRunnable_Send->Stop();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Send::FClientRunnable_Send()
{
	pThread = FRunnableThread::Create(this, TEXT("ClientSendThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FClientRunnable_Send::~FClientRunnable_Send()
{
}

bool FClientRunnable_Send::Init()
{
	// socket initialize
	

	return true;
}

uint32 FClientRunnable_Send::Run()
{
	while (m_bIsRunning)
	{
		

		FPlatformProcess::Sleep(0.001f);
	}

	return 0;
}

void FClientRunnable_Send::Stop() // 
{
	m_bIsRunning = false;
}

void FClientRunnable_Send::Exit() // called when func Run() is returned
{
	pThread->WaitForCompletion();

	delete pThread;
	pThread = nullptr;
	
	delete this;
}

//////////////////////////////////////////////////////////////////////////

FClientRunnable_Recv::FClientRunnable_Recv()
{
	pThread = FRunnableThread::Create(this, TEXT("ClientRecvThread"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
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
