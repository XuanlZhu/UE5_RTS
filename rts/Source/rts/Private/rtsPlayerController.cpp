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
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"

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
	
	if (HUDWidgetClass)
	{
		HudWidget = CreateWidget<UUserWidget>(
			this,
			HUDWidgetClass
		);
	}
	if (HudWidget)HudWidget->AddToViewport();
	//委托更新
	// ASC->GetGameplayAttributeValueChangeDelegate( UMyAttributeSet::GetHealthAttribute() ).AddUObject(HudWidget,&UHUDWidget::OnHealthChanged);
	SelectBoxWidget = HudWidget->GetWidgetFromName(TEXT("SelectionBox"));
	
	
}

void ArtsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	// 添加输入映射上下文,也就是设置IMC_
	GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(DefaultMappingContext, 0);
	
	
	// 设置动作绑定
	if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 设置鼠标输入事件
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ArtsPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ArtsPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ArtsPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ArtsPlayerController::OnSetDestinationReleased);
		
		// 设置按下Q
		EnhancedInputComponent->BindActionInstanceLambda(PressQ, ETriggerEvent::Started, [this](const FInputActionInstance& Instance){
			UE_LOG(LogTemp, Log, TEXT("Press Q"));

			ASC->ApplyModToAttribute(
				UMyAttributeSet::GetHealthAttribute(),
				EGameplayModOp::Additive,
				-10.0f
			);
		});
		//设置 按下S
		EnhancedInputComponent->BindActionInstanceLambda(PressS, ETriggerEvent::Started, [this](const FInputActionInstance& Instance){
			StopMovement();
		});
		// UE_LOG(LogTemp, Log, TEXT("Press Q"));
		//输入左键
		EnhancedInputComponent->BindAction(SelectBox, ETriggerEvent::Started, this, &ArtsPlayerController::OnSelectBoxStart);
		EnhancedInputComponent->BindAction(SelectBox, ETriggerEvent::Completed, this, &ArtsPlayerController::OnSelectBoxEnd);
		EnhancedInputComponent->BindAction(SelectBox, ETriggerEvent::Triggered, this, &ArtsPlayerController::OnSelectBoxTrigger);
	}
}
void ArtsPlayerController::OnSelectBoxTrigger()
{
	// UE_LOG(LogTemp, Log, TEXT("框选触发"));
	// GetMousePosition(SelectBoxCurrent.X,SelectBoxCurrent.Y);
	// Cast<UCanvasPanelSlot>(SelectBoxWidget->Slot)->SetSize(FVector2D(SelectBoxCurrent.X-SelectBoxStart.X, 100));
	
	GetMousePosition(SelectBoxCurrent.X,SelectBoxCurrent.Y);

	// 获取 HUD 的 Geometry
	FGeometry Geometry = HudWidget->GetCachedGeometry();

	// 屏幕坐标 -> HUD 局部坐标
	FVector2D StartLocal;
	FVector2D CurrentLocal;

	USlateBlueprintLibrary::ScreenToWidgetLocal(
		this,
		Geometry,
		SelectBoxStart,
		StartLocal
	);

	USlateBlueprintLibrary::ScreenToWidgetLocal(
		this,
		Geometry,
		SelectBoxCurrent,
		CurrentLocal
	);

	// 计算矩形左上角
	FVector2D Position(
		FMath::Min(StartLocal.X, CurrentLocal.X),
		FMath::Min(StartLocal.Y, CurrentLocal.Y)
	);

	// 计算矩形大小
	FVector2D Size(
		FMath::Abs(CurrentLocal.X - StartLocal.X),
		FMath::Abs(CurrentLocal.Y - StartLocal.Y)
	);

	// 设置位置和大小
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SelectBoxWidget->Slot);
	CanvasSlot->SetPosition(Position);
	CanvasSlot->SetSize(Size);

}
void ArtsPlayerController::OnSelectBoxStart()
{
	UE_LOG(LogTemp, Log, TEXT("框选开始"));
	
	bSelecting = true;
	// 记录鼠标按下的位置
	GetMousePosition(SelectBoxStart.X,SelectBoxStart.Y);
	
	FVector2D LocalPosition;

	USlateBlueprintLibrary::ScreenToWidgetLocal(
		this,
		HudWidget->GetCachedGeometry(),
		SelectBoxStart,
		LocalPosition
	);
	
	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(SelectBoxWidget->Slot);
	Slot->SetPosition(LocalPosition);
	Slot->SetSize(FVector2D(0, 100));
	
	
	
	SelectBoxWidget->SetVisibility(
		ESlateVisibility::Visible
	);
}

void ArtsPlayerController::OnSelectBoxEnd()
{
	UE_LOG(LogTemp, Log, TEXT("框选结束"));
	bSelecting = false;

	SelectBoxWidget->SetVisibility(
		ESlateVisibility::Collapsed
	);
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




