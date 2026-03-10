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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor PendingBorderColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor PendingTintColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CurrentBorderColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CurrentTintColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CompletedBorderColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor CompletedTintColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor FailedBorderColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "QTE|Style")
	FLinearColor FailedTintColor;
};
