#include "GizmoComponent.h"

void UGizmoComponent::HideComponent() {
	SetVisibility(false,false);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Ignore);
}

void UGizmoComponent::ShowComponent() {
	SetVisibility(true,false);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Block);
}
