#include "RobotArm_DT.h"

ARobotArm_DT::ARobotArm_DT() {
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	SetRootComponent(Root);
	
	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Base->SetupAttachment(RootComponent);
	Link1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link1"));
	Link1->SetupAttachment(Base);
	Link2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link2"));
	Link2->SetupAttachment(Link1);
	Link3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link3"));
	Link3->SetupAttachment(Link2);
	Link4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link4"));
	Link4->SetupAttachment(Link3);
	Link5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link5"));
	Link5->SetupAttachment(Link4);
	Link6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Link6"));
	Link6->SetupAttachment(Link5);
	
	GhostBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostBase"));
	GhostBase->SetupAttachment(RootComponent);
	GhostLink1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink1"));
	GhostLink1->SetupAttachment(GhostBase);
	GhostLink2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink2"));
	GhostLink2->SetupAttachment(GhostLink1);
	GhostLink3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink3"));
	GhostLink3->SetupAttachment(GhostLink2);
	GhostLink4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink4"));
	GhostLink4->SetupAttachment(GhostLink3);
	GhostLink5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink5"));
	GhostLink5->SetupAttachment(GhostLink4);
	GhostLink6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostLink6"));
	GhostLink6->SetupAttachment(GhostLink5);
}

void ARobotArm_DT::BeginPlay() {
	Super::BeginPlay();
}

void ARobotArm_DT::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}



