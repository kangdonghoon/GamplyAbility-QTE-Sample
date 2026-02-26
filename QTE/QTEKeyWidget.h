// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "QTE/QTETypes.h"

#include "QTEKeyWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

// ---------------------------------------------------------------------------
// Single key display — 시퀀스 내 키 하나
// ---------------------------------------------------------------------------

/** QTE 시퀀스에서 개별 키 하나를 표시. BindWidgetOptional: KeyImage, KeyText, KeyBorder */
UCLASS(Blueprintable)
class LYRAGAME_API UQTEKeyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** 키 아이콘 이미지 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> KeyImage;

	/** 키 텍스트 라벨 */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> KeyText;

	/** 키 박스 외곽 (색상으로 상태를 표현) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> KeyBorder;

	void SetupKey(const FText& Label, UTexture2D* Icon);
	virtual void SetKeyState(EQTEKeyState NewState);

protected:

	// ── 상태별 색상 (EditDefaultsOnly로 Blueprint에서 조정 가능) ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor PendingBorderColor = FLinearColor(0.22f, 0.22f, 0.28f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor PendingTintColor = FLinearColor(0.45f, 0.45f, 0.5f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CurrentBorderColor = FLinearColor(1.0f, 0.82f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CurrentTintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CompletedBorderColor = FLinearColor(0.08f, 0.7f, 0.22f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CompletedTintColor = FLinearColor(0.1f, 0.8f, 0.28f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor FailedBorderColor = FLinearColor(0.85f, 0.1f, 0.08f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor FailedTintColor = FLinearColor(0.85f, 0.1f, 0.08f, 1.0f);
};
