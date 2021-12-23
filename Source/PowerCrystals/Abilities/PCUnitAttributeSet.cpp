// Fill out your copyright notice in the Description page of Project Settings.


#include "PCUnitAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "../Units/PCUnit.h"
#include "GameplayEffectExtension.h"

bool UPCUnitAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UPCUnitAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (!GetOwningAbilitySystemComponent())
	{
		return;
	}

	static FProperty* HealthProperty = FindFieldChecked<FProperty>(UPCUnitAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UPCUnitAttributeSet, Health));

	FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	if (HealthProperty == ModifiedProperty)
	{
		Health = FMath::Clamp<float>(Health, 0.0, MaxHealth);

		APCUnit* OwningUnit = Cast<APCUnit>(GetOwningActor());
		if (OwningUnit)
		{
			AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
			OwningUnit->OnUnitHealthChanged(Health, Instigator);
		}
	}
}

void UPCUnitAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCUnitAttributeSet, MaxHealth);
	DOREPLIFETIME(UPCUnitAttributeSet, Health);
	DOREPLIFETIME(UPCUnitAttributeSet, Damage);
	DOREPLIFETIME(UPCUnitAttributeSet, Movement);
}