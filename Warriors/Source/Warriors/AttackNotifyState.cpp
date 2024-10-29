// Copyright Epic Games, Inc. All Rights Reserved.

#include "AttackNotifyState.h"
#include "WarriorsCharacter.h"
#include "Item.h"

void UAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AWarriorsCharacter* Character = Cast<AWarriorsCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		AItem* Item = Character->GetEquippedItem();
		if (IsValid(Item))
		{
			Item->OnStartAttack.Broadcast();
		}
	}
}

void UAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AWarriorsCharacter* Character = Cast<AWarriorsCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		AItem* Item = Character->GetEquippedItem();
		if (IsValid(Item))
		{
			Item->OnEndAttack.Broadcast();
		}
	}
}