// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RandomMapActor.h"

// Sets default values
ARandomMapActor::ARandomMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void ARandomMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

