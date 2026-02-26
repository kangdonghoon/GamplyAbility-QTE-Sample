# QTE(Quick Time Event) — 키 시퀀스 입력 시스템

Lyra Starter Game 기반 **Gameplay Ability** 를 사용한 QTE(Quick Time Event) 시스템입니다.
제한 시간 내에 화면에 표시된 키 시퀀스를 순서대로 입력하면 성공,
잘못된 키를 누르면 시퀀스를 처음부터 다시 입력해야 합니다.

환경: Unreal Engine 5 (Lyra Starter Game)


## 파일 구조

```
QTE/

[데이터 정의]
  QTETypes.h                    — 결과/키 상태 enum, FQTEKeyCommand, FQTESequenceData, FQTEKeyIconRow

[AbilityTask — 입력/타이머 판정]
  AbilityTask_QTEInput.h/.cpp   — 키 시퀀스 입력 수신, 타이머, 성공/타임아웃/리셋 처리

[Gameplay Ability]
  LyraGameplayAbility_QTE.h/.cpp — QTE 어빌리티 베이스, 태스크 기동, 성공/실패 GE 적용

[UI 위젯]
  QTEWidget.h/.cpp              — 시퀀스 표시, 타이머 바, 결과 텍스트
  QTEKeyWidget.h/.cpp            — 개별 키 하나 표시 (Pending/Current/Completed/Failed)
```


## 아키텍처

### 핵심 개념

- **FQTESequenceData** — 키 목록(KeyCommands), 제한 시간(TimeLimit), 필요 라운드 수(RequiredSuccessCount), 잘못된 입력 시 리셋 여부(bResetOnWrongInput)
- **FQTEKeyCommand** — 개별 키(FKey), 아이콘/표시명 오버라이드(선택)
- **UAbilityTask_QTEInput** — 매 틱 타이머 감소, 올바른/잘못된 키 입력 판정, 라운드 진행 및 완료 시 브로드캐스트
- **ULyraGameplayAbility_QTE** — 어빌리티 활성화 시 태스크 시작, 완료 시 성공/실패 GameplayEffect 적용, 결과 표시 시간 후 어빌리티 종료
- **UQTEWidget / UQTEKeyWidget** — UMG 위젯으로 키 시퀀스·타이머·결과 표시

### 결과 / 키 상태

- **EQTEResult**: Success, Failed_Timeout, Failed_Cancelled
- **EQTEKeyState**: Pending, Current, Completed, Failed (위젯 비주얼용)


## 실행 흐름

### 1. 어빌리티 활성화

```
Input Trigger (또는 이벤트)
  -> ULyraGameplayAbility_QTE::ActivateAbility
      -> CommitAbility 성공 시 StartQTE()
      -> StartQTEWithData(DefaultSequenceData)
          -> UAbilityTask_QTEInput::StartQTESequence(this, SequenceData, QTEWidgetClass)
          -> ReadyForActivation()
```

### 2. 태스크 활성화 및 UI 표시

```
UAbilityTask_QTEInput::Activate
  -> KeyCommands 비어 있으면 CompleteQTE(Failed_Cancelled)
  -> RemainingTime = TimeLimit, CurrentKeyIndex = 0, CurrentRound = 0
  -> ShowQTEWidget() -> AddToViewport, InitializeQTE, UpdateKeyProgress(0)
```

### 3. 매 프레임 (TickTask)

```
TickTask(DeltaTime)
  -> RemainingTime -= DeltaTime
  -> ActiveWidget->UpdateTimer(RemainingTime, TimeLimit)
  -> RemainingTime <= 0 이면 CompleteQTE(Failed_Timeout)
  -> ProcessInput()
      -> 기대 키(KeyCommands[CurrentKeyIndex]) 방금 눌림?
          -> CurrentKeyIndex++
          -> 인덱스가 시퀀스 끝이면 AdvanceRound()
          -> 아니면 OnKeyProgress 브로드캐스트 + 위젯 UpdateKeyProgress
      -> bResetOnWrongInput 이고, 시퀀스에 있는 다른 키가 방금 눌림?
          -> ResetSequence() -> CurrentKeyIndex = 0, OnSequenceReset, 위젯 OnSequenceReset
```

### 4. 라운드 완료 및 최종 성공

```
AdvanceRound()
  -> CurrentRound++
  -> OnRoundCompleted 브로드캐스트, 위젯 OnRoundCompleted
  -> CurrentRound >= RequiredSuccessCount 이면 CompleteQTE(Success)
  -> 아니면 CurrentKeyIndex = 0 으로 시퀀스만 리셋 (타이머는 리셋 안 함)
```

### 5. 결과 처리 및 어빌리티 종료

```
CompleteQTE(Result)
  -> bIsCompleted = true
  -> ActiveWidget->OnQTEResult(Result)
  -> OnQTECompleted.Broadcast(Result)

LyraGameplayAbility_QTE::HandleQTECompleted(Result)
  -> OnQTECompleted(Result)  // Blueprint 이벤트
  -> Success 시 SuccessGameplayEffect, 실패 시 FailureGameplayEffect 적용
  -> ResultDisplayDuration 후 FinishAbilityAfterDisplay() -> EndAbility
  -> EndAbility 시 ActiveQTETask->EndTask(), RemoveQTEWidget
```


## 에디터 설정 방법

### 1. QTE 어빌리티 Blueprint 생성

1. LyraGameplayAbility_QTE를 상속한 Blueprint 생성 (예: GA_QTE_Example)
2. **Lyra|QTE** 카테고리에서 설정:
   - **DefaultSequenceData**: KeyCommands에 입력할 키 순서 추가 (예: W, A, S, D), TimeLimit, RequiredSuccessCount, bResetOnWrongInput
   - **QTEWidgetClass**: UQTEWidget을 상속한 위젯 Blueprint (KeyContainer, TimerBar 바인딩)
   - **ResultDisplayDuration**: 결과(SUCCESS/FAILED) 표시 시간(초)
   - **SuccessGameplayEffect / FailureGameplayEffect**: (선택) 적용할 GE
3. 입력에 어빌리티 트리거 바인딩 (Lyra 입력 설정)

### 2. QTE 위젯 Blueprint

1. UQTEWidget을 상속한 위젯 Blueprint 생성
2. 필수: **KeyContainer** (Horizontal Box), **TimerBar** (Progress Bar) — BindWidget과 동일한 이름으로 배치
3. 선택: TitleText, TimerText, StatusText, CountText
4. **KeyWidgetClass**: UQTEKeyWidget을 상속한 개별 키 위젯 Blueprint
5. **KeyIconTable**: (선택) DataTable (FQTEKeyIconRow), Row Name을 FKey 이름(예: "W", "SpaceBar")으로 사용

### 3. 키 아이콘 DataTable (선택)

1. Row 구조체: FQTEKeyIconRow (KeyIcon, DisplayName)
2. Row Name을 "W", "A", "SpaceBar", "Gamepad_FaceButton_Bottom" 등 FKey 이름으로 추가


## 핵심 설계 포인트

- **AbilityTask + 틱** — bTickingTask로 매 프레임 타이머 감소와 입력 처리. 어빌리티 생명주기와 일치
- **단일 타이머** — RequiredSuccessCount가 2 이상이어도 타이머는 리셋하지 않음. 전체 QTE에 하나의 제한 시간만 사용
- **리셋 정책** — bResetOnWrongInput이 true일 때만 시퀀스에 포함된 “다른” 키 입력 시 리셋. 그 외 키는 무시
- **델리게이트** — OnQTECompleted, OnKeyProgress, OnSequenceReset, OnRoundCompleted로 Blueprint/ C++ 양쪽에서 반응 가능
- **위젯 선택** — WidgetClass가 nullptr이면 UI 없이 로직만 동작 (서버/헤드리스 등에서 활용 가능)
