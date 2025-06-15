// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "Hyperion/RemoteCharacter.h"
#include "RemoteCharAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class HYPERIONANIM_API URemoteCharAnimInst : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	/*
private:
	FORCEINLINE ARemoteCharacter* GetRemoteCharOwningThis()
	{
		auto pPawn = TryGetPawnOwner();
		check(pPawn);

		if (auto pRemoteChar = Cast<ARemoteCharacter>(pPawn))
		{
			return pRemoteChar;
		}

		return nullptr;
	}
	*/

public:
	FORCEINLINE void SetCurVel(FVector _InVec) 
	{
		m_CurVel = _InVec;
		m_CurSpeed = _InVec.Size();
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* m_MtgSpawn;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FVector m_CurVel{ FVector() };
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float m_CurSpeed{ 0.f };
	//UPROPERTY(BlueprintReadOnly, Category = "Animation")
	//bool m_bIsJump{ false };
};
