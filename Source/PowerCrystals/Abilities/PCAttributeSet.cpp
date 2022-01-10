// Fill out your copyright notice in the Description page of Project Settings.


#include "PCAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Components/PCActionableActorComponent.h"

bool UPCAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UPCAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (!GetOwningAbilitySystemComponent())
	{
		return;
	}

	static FProperty* HealthProperty = FindFieldChecked<FProperty>(UPCAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UPCAttributeSet, Health));

	FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	if (HealthProperty == ModifiedProperty)
	{
		Health = FMath::Clamp<float>(Health, 0.0, MaxHealth);

		UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(GetOwningActor()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
		if (ActionableComponent)
		{
			AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();
			ActionableComponent->HealthChanged(Health, Instigator);
		}
	}
}

void UPCAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCAttributeSet, MaxHealth);
	DOREPLIFETIME(UPCAttributeSet, Health);
	DOREPLIFETIME(UPCAttributeSet, Damage);
	DOREPLIFETIME(UPCAttributeSet, Movement);
	DOREPLIFETIME(UPCAttributeSet, ConstructionTime);
}

