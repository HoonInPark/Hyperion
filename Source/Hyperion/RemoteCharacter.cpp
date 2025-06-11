// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharacter.h"

#include "RemoteCharacterManager.h"
#include "HyperionAnim/RemoteCharAnimInst.h"
#include "ServerHyperionLibrary/Packet.h"

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

	if (m_bIsMove)
	{
		FVector&& NewLocation = FMath::VInterpTo(GetActorLocation(), m_Dest, DeltaTime, m_InterpSpeed);
		SetActorLocation(NewLocation);

		FRotator CurrentRot = GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, m_Rot, DeltaTime, m_InterpSpeed);
		SetActorRotation(NewRot);

		const bool&& bLocReached = FVector::Dist(NewLocation, m_Dest) < 1.f;
		const bool&& bRotReached = FMath::Abs((NewRot - m_Rot).GetNormalized().Yaw) < 1.f
			&& FMath::Abs((NewRot - m_Rot).GetNormalized().Pitch) < 1.f
			&& FMath::Abs((NewRot - m_Rot).GetNormalized().Roll) < 1.f;

		if (bLocReached && bRotReached)
		{
			m_bIsMove = false;
		}
	}

	if (auto pAnimInst = Cast<URemoteCharAnimInst>(GetMesh()->GetAnimInstance()))
	{
		pAnimInst->SetCurSpeed(GetVelocity().Size());
	}
}

void ARemoteCharacter::OnNotify_Implementation(UObservableBase* _pInObservable)
{
	auto pRemoteCharMng = Cast<URemoteCharacterManager>(_pInObservable);
	//check(pRemoteCharMng);

	if (pRemoteCharMng->GetCurPack()->GetSessionIdx() == m_SessionIdx)
	{
		if (pRemoteCharMng->GetCurPack()->GetHeader()[static_cast<int>(Packet::Header::POS_X)])
		{
			SetDestination(
				pRemoteCharMng->GetCurPack()->GetPosX(),
				pRemoteCharMng->GetCurPack()->GetPosY(),
				pRemoteCharMng->GetCurPack()->GetPosZ());
		}
		
		if (pRemoteCharMng->GetCurPack()->GetHeader()[static_cast<int>(Packet::Header::ROT_X)])
		{
			SetRotation(
				pRemoteCharMng->GetCurPack()->GetRotX(),
				pRemoteCharMng->GetCurPack()->GetRotY(),
				pRemoteCharMng->GetCurPack()->GetRotZ());
		}
	}
}

void ARemoteCharacter::SetDestination(float _InDestX, float _InDestY, float _InDestZ)
{
	m_Dest.X = _InDestX;
	m_Dest.Y = _InDestY;
	m_Dest.Z = _InDestZ;

	m_bIsMove = true;
}

void ARemoteCharacter::SetRotation(float _InRotX, float _InRotY, float _InRotZ)
{
	m_Rot.Pitch = /*_InRotX*/ 0.f;
	m_Rot.Roll = _InRotY;
	m_Rot.Yaw = _InRotZ;

	m_bIsMove = true;
}
