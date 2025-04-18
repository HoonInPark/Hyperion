// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Observer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UObserver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HYPERIONBASE_API IObserver
{
	GENERATED_BODY()

public:

	// 주체로부터 상태 변화를 수신 하는 함수
	// BlueprintNativeEvent로 정의 되며, C++ 또는 블루프린트에서 구현 가능
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Observer")
	void OnNotify(int32 UpdateScore);

};
