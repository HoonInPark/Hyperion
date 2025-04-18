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
 * reference : https://velog.io/@apzl79/Unreal-Engine-5-22-Observer-Pattern
 */
class HYPERIONBASE_API IObserver
{
	GENERATED_BODY()

public:

	// ��ü�κ��� ���� ��ȭ�� ���� �ϴ� �Լ�
	// BlueprintNativeEvent�� ���� �Ǹ�, C++ �Ǵ� �������Ʈ���� ���� ����
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Observer")
	void OnNotify(int32 UpdateScore);

};
