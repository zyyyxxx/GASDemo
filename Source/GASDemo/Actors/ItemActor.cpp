// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("USphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this , &AItemActor::OnSphereOverlap);
	
}

void AItemActor::OnEquipped()
{
	ItemState = EItemState::Equipped;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnUnEquipped()
{
	ItemState = EItemState::None;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnDropped()
{
	ItemState = EItemState::Dropped;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if(AActor* ActorOwner = GetOwner())
	{
		const FVector Location = GetActorLocation();
		const FVector Forward = ActorOwner->GetActorForwardVector();

		const float droppedItemDist = 100.f;
		const float droppedItemTraceDist = 1000.f;
		
		const FVector TraceStart = Location + Forward * droppedItemDist;
		const FVector TraceEnd = TraceStart - FVector::UpVector * droppedItemTraceDist;

		TArray<AActor*> ActorsToIgnore = {GetOwner()};

		FHitResult TraceHit;

		// Debug 
		static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
		const bool bShowInventory = CVar->GetInt() > 0;
		EDrawDebugTrace::Type DebugDrawType = bShowInventory ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		
		//丢弃射线检测
		if(UKismetSystemLibrary::LineTraceSingleByProfile(this , TraceStart , TraceEnd , TEXT("Worldstatic") , true,
														ActorsToIgnore,DebugDrawType , TraceHit,	true))
		{
			if(TraceHit.bBlockingHit)
			{
				SetActorLocation(TraceHit.Location);
				return;
			}
		}

		SetActorLocation(TraceEnd);
		
		
	}
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	WroteSomething |= Channel->ReplicateSubobject(ItemInstance , *Bunch , *RepFlags); // Replicates given subobject on this actor channel
	
	return WroteSomething;
	
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AItemActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FGameplayEventData EventPayload;
	EventPayload.OptionalObject = this;
	/**
	 * This function can be used to trigger an ability on the actor in question with useful payload data.
	 * NOTE: GetAbilitySystemComponent is called on the actor to find a good component, and if the component isn't
	 * found, the event will not be sent.
	 */
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, OverlapEventTag , EventPayload);
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor , ItemInstance);
	DOREPLIFETIME(AItemActor , ItemState);
	
}
