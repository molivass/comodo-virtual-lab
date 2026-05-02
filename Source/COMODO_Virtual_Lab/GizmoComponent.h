// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GizmoComponent.generated.h"


UENUM(BlueprintType)
enum class EGizmoAxis : uint8 {
	X,
	Y,
	Z,
	All
};

/**
 * 
 */
UCLASS()
class COMODO_VIRTUAL_LAB_API UGizmoComponent : public UStaticMeshComponent {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGizmoAxis GizmoAxis;
	
	virtual void Dragged(const FVector2D MouseDelta) {}

	UFUNCTION(BlueprintCallable)
	void HideComponent();
	
	UFUNCTION(BlueprintCallable)
	void ShowComponent();
};
