// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteCharAnimInst.h"

void URemoteCharAnimInst::NativeBeginPlay()
{
	Montage_Play(m_MtgSpawn);
}

void URemoteCharAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


}

void URemoteCharAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}
