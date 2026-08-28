// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAttributeSet.h"

UMyAttributeSet::UMyAttributeSet()
{
	// 初始化
	Health.SetBaseValue(100.f);
	Health.SetCurrentValue(100.f);
	
	
}
// GameplayEffect修改属性前调用
void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}
// GameplayEffect执行后调用
void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

