// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "QTE/QTETypes.h"

#include "QTEWidget.generated.h"

class UHorizontalBox;
class UTextBlock;
class UProgressBar;
class UQTEKeyWidget;
class UDataTable;

// ---------------------------------------------------------------------------
// Main QTE UI — 시퀀스 표시 / 타이머 / 결과
// ---------------------------------------------------------------------------

/** QTE 시퀀스를 화면에 표시하는 메인 위젯. BindWidget: KeyContainer, TimerBar. 키 아이콘: KeyIconOverride → KeyIconTable → FKey::GetDisplayName */
UCLASS(Blueprintable)
class LYRAGAME_API UQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> KeyContainer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> TimerBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CountText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	TSubclassOf<UQTEKeyWidget> KeyWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	TObjectPtr<UDataTable> KeyIconTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor TimerGreenColor = FLinearColor(0.15f, 0.82f, 0.25f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor TimerYellowColor = FLinearColor(0.92f, 0.82f, 0.1f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor TimerRedColor = FLinearColor(0.92f, 0.15f, 0.1f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor SuccessTextColor = FLinearColor(0.1f, 0.9f, 0.25f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor FailTextColor = FLinearColor(0.92f, 0.15f, 0.1f, 1.0f);
	
	virtual void InitializeQTE(const FQTESequenceData& SequenceData);
	virtual void UpdateKeyProgress(int32 CurrentIndex);
	virtual void UpdateTimer(float RemainingTime, float TotalTime);
	virtual void OnSequenceReset();
	virtual void OnQTEResult(EQTEResult Result);
	virtual void UpdateRoundCounter(int32 CurrentRound, int32 TotalRounds);
	virtual void OnRoundCompleted(int32 CompletedRound, int32 TotalRounds);

protected:
	UTexture2D* FindKeyIcon(const FQTEKeyCommand& Command) const;
	FText GetKeyDisplayName(const FQTEKeyCommand& Command) const;

private:
	void ClearKeyWidgets();
	void SetAllKeyStates(EQTEKeyState State);
	FLinearColor GetTimerColor(float Percent) const;
	UPROPERTY()
	TArray<TObjectPtr<UQTEKeyWidget>> KeyWidgetInstances;
};
