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
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Item->GetRootComponent());
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Item->GetRootComponent());
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}