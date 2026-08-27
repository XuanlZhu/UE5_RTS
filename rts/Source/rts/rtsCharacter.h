// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "rtsCharacter.generated.h"

UCLASS(Blueprintable)
class ArtsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ArtsCharacter();

	// 每一帧都会被调用。
	virtual void Tick(float DeltaSeconds) override;

	/** 返回 TopDownCameraComponent 子对象。 **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** 返回 CameraBoom 子对象 **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** 俯视视角摄像机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** 将摄像机置于角色上方的摄像机吊臂 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};

