// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Observer.h"
#include "GameStateSub.generated.h"

/**
 * 
 */
UCLASS()
class HYPERIONBASE_API UGameStateSub : public UObject
{
	GENERATED_BODY()
	
private:
	// ������ ����Ʈ : ��ϵ� ���������� ���� ��ȭ�� ������
	TArray<TScriptInterface<IObserver>> Observers;

	// ��ü�� ���� ���� ( ��: �÷��̾� ���� )
	int32 PlayerScore;

public:
	// ������ : �ʱ� ���� ����
	UGameStateSub();

	// ������ ��� : ��ü�� ���� ��ȭ�� �˸� �������� �����
	void RegisterObserver(TScriptInterface<IObserver> observer);

	// ������ ��� ���� : ��ü�� ���� ��ȭ �˸��� ������ �������� ������
	void UnregisterObserver(TScriptInterface<IObserver> observer);

	// ���� ��ȭ �߻� �� ��� ���������� �˸�
	void NotifyObservers();

	// ����( ���� )�� �����ϴ� �Լ�. ���°� ����Ǹ� NotifyObserver()�� ȣ���
	void IncreaseScore(int32 Amount);

	// ���� ���� ��ȯ
	int32 GetScore() const { return PlayerScore; }
};
