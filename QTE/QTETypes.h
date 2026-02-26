// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InputCoreTypes.h"

#include "QTETypes.generated.h"

class UTexture2D;

// ---------------------------------------------------------------------------
// Result / Key state enums
// ---------------------------------------------------------------------------

/** QTE 최종 결과 */
UENUM(BlueprintType)
enum class EQTEResult : uint8
{
	Success,
	Failed_Timeout,
	Failed_Cancelled
};

/** 개별 키의 현재 상태 (위젯 비주얼 구동용) */
UENUM(BlueprintType)
enum class EQTEKeyState : uint8
{
	Pending,
	Current,
	Completed,
	Failed
};

// ---------------------------------------------------------------------------
// Key command — 시퀀스 내 개별 키
// ---------------------------------------------------------------------------

/** KeyIconOverride / DisplayName이 비어 있으면 DataTable → FKey 기본값 순으로 폴백 */
USTRUCT(BlueprintType)
struct LYRAGAME_API FQTEKeyCommand
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	TSoftObjectPtr<UTexture2D> KeyIconOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	FText DisplayName;
};

// ---------------------------------------------------------------------------
// Sequence data — QTE 시퀀스 전체 설정
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct LYRAGAME_API FQTESequenceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	TArray<FQTEKeyCommand> KeyCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE", meta = (ClampMin = "1.0"))
	float TimeLimit = 5.0f;

	/** 최종 성공까지 필요한 시퀀스 완료 횟수. 타이머는 리셋되지 않고 전체 라운드에 걸쳐 공유 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE", meta = (ClampMin = "1"))
	int32 RequiredSuccessCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	bool bResetOnWrongInput = true;

	int32 GetRequiredRounds() const { return FMath::Max(1, RequiredSuccessCount); }
};

// ---------------------------------------------------------------------------
// DataTable row — 키별 아이콘/표시명 (Row Name = FKey 이름)
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct LYRAGAME_API FQTEKeyIconRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	TSoftObjectPtr<UTexture2D> KeyIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	FText DisplayName;
};
