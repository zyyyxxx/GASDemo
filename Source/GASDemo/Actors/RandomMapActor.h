// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomMapActor.generated.h"

class Kruskal
{
public:
	void Find();
	void Union();

private:
	
};


UCLASS()
class GASDEMO_API ARandomMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CalculateRandomMap();
	
	
	void KruskalRoadGenerate();
	void FysRoomGenerate();
	
	// 地图大小
	UPROPERTY(EditDefaultsOnly , Category="RandomMap")
	int RowNum = 51;

	UPROPERTY(EditDefaultsOnly , Category="RandomMap")
	int ColumnNum = 51;

	TArray<TArray<int32>> MapGrid;
	
	UPROPERTY()
	TArray<UStaticMeshComponent*> Meshes;
	
	// 房间数量
	UPROPERTY(EditDefaultsOnly , Category="RandomMap")
	int RoomNum = 10;
	
	UPROPERTY(EditDefaultsOnly , Category= "RandomMap")
	UStaticMesh* CubeMesh;

};
