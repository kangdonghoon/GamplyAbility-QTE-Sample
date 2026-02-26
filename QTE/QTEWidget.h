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

	// ── UMG BindWidget ─────────────────────────────────────────────

	/** 키 위젯들이 동적으로 추가되는 컨테이너 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> KeyContainer;

	/** 제한 시간 프로그레스 바 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> TimerBar;

	/** 타이틀 텍스트 (예: "QTE!") */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	/** 남은 시간 숫자 텍스트 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;

	/** 결과 텍스트 (SUCCESS / FAILED) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;

	/** 라운드 카운터 텍스트 (예: "1 / 3") */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CountText;

	// ── 설정 ───────────────────────────────────────────────────────

	/** 키 하나를 표시하는 위젯 클래스 (UQTEKeyWidget의 Blueprint 서브클래스) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	TSubclassOf<UQTEKeyWidget> KeyWidgetClass;

	/** FKey 이름 → 아이콘/표시명을 매핑하는 DataTable (FQTEKeyIconRow) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE")
	TObjectPtr<UDataTable> KeyIconTable;

	// ── 스타일 ─────────────────────────────────────────────────────

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

	// ── 인터페이스 (AbilityTask에서 호출) ──

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
