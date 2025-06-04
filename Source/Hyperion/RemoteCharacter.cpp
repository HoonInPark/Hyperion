// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacter.h"

#include "RemoteCharacterManager.h"

// Sets default values
ARemoteCharacter::ARemoteCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARemoteCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARemoteCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARemoteCharacter::OnNotify_Implementation(UObservableBase* _pInObservable)
{
	auto pRemoteCharMng = Cast<URemoteCharacterManager>(_pInObservable);
	check(pRemoteCharMng);

	if (pRemoteCharMng->GetCurPack().GetSessionIdx() == m_SessionIdx)
	{
		UE_LOG(LogTemp, Warning, TEXT("msg type: %d || cli idx %d || %f, %f, %f || %f, %f, %f"),
			static_cast<int>(pRemoteCharMng->GetCurPack().GetMsgType()),
			pRemoteCharMng->GetCurPack().GetSessionIdx(),
			pRemoteCharMng->GetCurPack().GetPosX(),
			pRemoteCharMng->GetCurPack().GetPosY(),
			pRemoteCharMng->GetCurPack().GetPosZ(),
			pRemoteCharMng->GetCurPack().GetRotX(),
			pRemoteCharMng->GetCurPack().GetRotY(),
			pRemoteCharMng->GetCurPack().GetRotZ());
	}
}
