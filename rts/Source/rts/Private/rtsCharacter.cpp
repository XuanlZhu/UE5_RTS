// Copyright Epic Games, Inc. All Rights Reserved.

#include "rtsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ArtsCharacter::ArtsCharacter()
{
	// 设置玩家胶囊体的大小
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 不要将角色旋转至面向摄像机的方向
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 配置角色移动
	GetCharacterMovement()->bOrientRotationToMovement = true; // 将角色转向移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;//约束到地面
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//当角色开始移动时，把角色的位置自动调整到约束的移动平面上。

	// 创建一个摄像机摇臂
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);//挂在根节点
	CameraBoom->SetUsingAbsoluteRotation(true); // 不希望手臂随角色转动
	CameraBoom->TargetArmLength = 10000.f;//程度
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));//设置相对旋转
	CameraBoom->bDoCollisionTest = false; // 不想让摄像机在与关卡发生碰撞时向内拉近。

	// 创建摄像机
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // 相机相对于机械臂不旋转

	// 启用 Tick（逐帧更新）以在每一帧更新光标
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ArtsCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
