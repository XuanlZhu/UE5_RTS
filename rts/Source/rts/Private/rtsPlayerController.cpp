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
#include "Kismet/GameplayStatics.h"
// #include "unit/UnitCharacter.h"

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
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ArtsPlayerController::AllMoveStarted);
		// EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ArtsPlayerController::AllMoveTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ArtsPlayerController::AllMoveReleased);
		// EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ArtsPlayerController::OnSetDestinationReleased);
		
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
	ClearSelectedUnits();//清理选中框
	bSelecting = false;

	SelectBoxWidget->SetVisibility(
		ESlateVisibility::Collapsed
	);
	
	// 计算框选矩形
	const float MinX = FMath::Min(SelectBoxStart.X, SelectBoxCurrent.X);
	const float MaxX = FMath::Max(SelectBoxStart.X, SelectBoxCurrent.X);

	const float MinY = FMath::Min(SelectBoxStart.Y, SelectBoxCurrent.Y);
	const float MaxY = FMath::Max(SelectBoxStart.Y, SelectBoxCurrent.Y);

	// 获取所有 AUnitCharacter
	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AUnitCharacter::StaticClass(),
		Actors
	);

	for (AActor* Actor : Actors)
	{
		AUnitCharacter* Unit = Cast<AUnitCharacter>(Actor);

		if (!Unit)continue;

		// 世界坐标 -> 屏幕坐标
		FVector2D ScreenPosition;

		if (!ProjectWorldLocationToScreen(
			Unit->GetActorLocation(),
			ScreenPosition
		))
		{
			continue;
		}

		// 判断是否在框选区域
		const bool bInBox =
			ScreenPosition.X >= MinX &&
			ScreenPosition.X <= MaxX &&
			ScreenPosition.Y >= MinY &&
			ScreenPosition.Y <= MaxY;

		if (bInBox)
		{
			// UE_LOG(LogTemp, Log, TEXT("开启选中特效"));
			UE_LOG(
				LogTemp,
				Log,
				TEXT("框选到单位: %s | Screen: X=%.1f Y=%.1f"),
				*Unit->GetName(),
				ScreenPosition.X,
				ScreenPosition.Y
			);
			AddSelectedUnit(Unit);
			
		}
	}
}
// 添加一个选中单位
void ArtsPlayerController::AddSelectedUnit(AUnitCharacter* Unit)
{
	if (!Unit)return;
	
	// 防止重复添加
	SelectedUnits.AddUnique(Unit);
	UE_LOG(LogTemp, Log, TEXT("加入选中表"));
	Unit->SetSelected(true);
}
// 清空所有选中单位
void ArtsPlayerController::RemoveSelectedUnit(AUnitCharacter* Unit)
{
	if (!Unit)
	{
		return;
	}

	if (SelectedUnits.Remove(Unit) > 0)
	{
		Unit->SetSelected(false);
	}
}
// 移除一个选中单位
void ArtsPlayerController::ClearSelectedUnits()
{
	UE_LOG(LogTemp, Log, TEXT("清理选项框"));
	for (AUnitCharacter* Unit : SelectedUnits)
	{
		if (IsValid(Unit))
		{
			Unit->SetSelected(false);
		}
	}
	SelectedUnits.Empty();
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
	
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

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

void ArtsPlayerController::AllMoveStarted()
{
	StartFollowTime = GetWorld()->GetTimeSeconds();
	
	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
}
void ArtsPlayerController::AllMoveReleased()
{
	// 如果是短按
	if (GetWorld()->GetTimeSeconds()-StartFollowTime <= ShortPressThreshold)
	{
		// 我们移动到那里，并生成一些粒子
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		
		// 没有选中单位
		if (SelectedUnits.IsEmpty())
		{
			AddSelectedUnit(Cast<AUnitCharacter>(GetPawn()));
		}
		
		
		for (AUnitCharacter* Unit : SelectedUnits)
		{
			if (IsValid(Unit))
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<AController>(Unit->GetController()), CachedDestination);
			}
		}
		
	}

	FollowTime = 0.f;
}
void ArtsPlayerController::AllMoveTriggered()
{
}




