// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GizmoComponent.h"
#include "TranslationGizmoComponent.generated.h"

/**
 * 
 */
UCLASS()
class COMODO_VIRTUAL_LAB_API UTranslationGizmoComponent : public UGizmoComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void Dragged(const FVector2D MouseDelta) override;
};
