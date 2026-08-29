// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "rtsPlayerController.generated.h"

class UWidget;
class UAbilitySystemComponent;
class UHUDWidget;
/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class ArtsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ArtsPlayerController();

	/** 时间阈值用于判断是否为短按 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** 点击时将生成的特效（FX）类 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;//粒子特效
	
	/** 映射上下文 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;//映射表
	
	/** 设置目的地点击动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;//目的地点击动作
	
	//Q动作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PressQ;//目的地点击动作
	//S动作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PressS;//目的地点击动作
	//左键框选
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SelectBox;//框选
	
	UPROPERTY()
	UWidget* SelectBoxWidget;

	FVector2D SelectBoxStart;
	FVector2D SelectBoxCurrent;
	bool bSelecting = false;

protected:
	/** 如果受控角色应导航至鼠标光标位置，则为真。 */
	uint32 bMoveToMouseCursor : 1;
	
	//设置输入绑定
	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnSelectBoxStart();
	void OnSelectBoxEnd();
	void OnSelectBoxTrigger();


private:
	FVector CachedDestination;//缓存目标

	bool bIsTouch; // 是否是触摸设备
	float FollowTime; //按压时间
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UUserWidget* HudWidget;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
};


