// Copyright Epic Games, Inc. All Rights Reserved.

#include "rtsPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "rtsCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "MyAttributeSet.h"
#include "HUDWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ArtsPlayerController::ArtsPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ArtsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())return;
	
	ASC = GetPawn<AUnitCharacter>()->GetAbilitySystemComponent();
	
	HudWidget = CreateWidget<UUserWidget>(
		this,
		HUDWidgetClass
	);
	if (HudWidget)HudWidget->AddToViewport();
	//委托更新
	// ASC->GetGameplayAttributeValueChangeDelegate( UMyAttributeSet::GetHealthAttribute() ).AddUObject(HudWidget,&UHUDWidget::OnHealthChanged);
	
}

void ArtsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 添加输入映射上下文
	// if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	// {
	// 	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	// }
	
	GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(DefaultMappingContext, 0);
	
	
	// 设置动作绑定
	if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 设置鼠标输入事件
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ArtsPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ArtsPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ArtsPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ArtsPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		// EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ArtsPlayerController::OnInputStarted);
		// EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ArtsPlayerController::OnTouchTriggered);
		// EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ArtsPlayerController::OnTouchReleased);
		// EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ArtsPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ArtsPlayerController::OnInputStarted()
{
	StopMovement();//停止
}

// 当输入被按住时，每一帧都会触发
void ArtsPlayerController::OnSetDestinationTriggered()
{
	// 我们标记输入正处于被按下的状态。
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// 我们查找玩家在世界空间中按下输入的位置。
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// 向鼠标指针或触摸点 径直 移动
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}
//当设置目标点释放
void ArtsPlayerController::OnSetDestinationReleased()
{
	// 如果是短按
	if (FollowTime <= ShortPressThreshold)
	{
		// 我们移动到那里，并生成一些粒子
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}








// Triggered every frame when the input is held down
void ArtsPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ArtsPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
