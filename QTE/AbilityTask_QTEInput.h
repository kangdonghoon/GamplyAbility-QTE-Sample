// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "QTE/QTETypes.h"

#include "AbilityTask_QTEInput.generated.h"

class UQTEWidget;
class UGameplayAbility;

// ---------------------------------------------------------------------------
// Delegates
// ---------------------------------------------------------------------------

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQTECompletedDelegate, EQTEResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQTEProgressDelegate, int32, CurrentIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQTERoundCompletedDelegate, int32, CompletedRound, int32, TotalRounds);

// ---------------------------------------------------------------------------
// AbilityTask — 제한 시간 내 키 시퀀스 입력 판정
// ---------------------------------------------------------------------------

/** 제한 시간 내에 키 시퀀스 입력을 받아 성공/실패를 판정. 잘못된 키 입력 시 시퀀스를 처음부터 다시 시작 */
UCLASS()
class LYRAGAME_API UAbilityTask_QTEInput : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FQTECompletedDelegate OnQTECompleted;

	UPROPERTY(BlueprintAssignable)
	FQTEProgressDelegate OnKeyProgress;

	UPROPERTY(BlueprintAssignable)
	FQTEProgressDelegate OnSequenceReset;

	UPROPERTY(BlueprintAssignable)
	FQTERoundCompletedDelegate OnRoundCompleted;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_QTEInput* StartQTESequence(
		UGameplayAbility* OwningAbility,
		FQTESequenceData SequenceData,
		TSubclassOf<UQTEWidget> WidgetClass);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "QTE")
	float GetRemainingTime() const { return RemainingTime; }

	UFUNCTION(BlueprintCallable, Category = "QTE")
	int32 GetCurrentKeyIndex() const { return CurrentKeyIndex; }

	UFUNCTION(BlueprintCallable, Category = "QTE")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintCallable, Category = "QTE")
	int32 GetTotalRounds() const { return SequenceData.GetRequiredRounds(); }

private:

	APlayerController* GetOwnerPlayerController() const;
	void ProcessInput();
	void ResetSequence();
	void AdvanceRound();
	void CompleteQTE(EQTEResult Result);
	void ShowQTEWidget();
	void RemoveQTEWidget();

	FQTESequenceData SequenceData;

	UPROPERTY()
	TSubclassOf<UQTEWidget> WidgetClass;

	UPROPERTY()
	TObjectPtr<UQTEWidget> ActiveWidget;

	int32 CurrentKeyIndex = 0;
	int32 CurrentRound = 0;
	float RemainingTime = 0.0f;
	bool bIsCompleted = false;
};
