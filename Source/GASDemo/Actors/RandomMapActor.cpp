// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RandomMapActor.h"

#include "Kismet/KismetMathLibrary.h"

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
	CalculateRandomMap();
}

void ARandomMapActor::CalculateRandomMap()
{
	// 初始化网格
	MapGrid.AddDefaulted(RowNum);
	
	for(int Row=0 ; Row < RowNum ; Row++)
	{
		MapGrid[Row].AddDefaulted(ColumnNum);
		for(int Column=0 ; Column < ColumnNum ; Column++)
		{
			UStaticMeshComponent* StaticMeshComp = NewObject<UStaticMeshComponent>(this);
			StaticMeshComp->SetupAttachment(GetRootComponent());
			StaticMeshComp->RegisterComponent();
			int CubeType = (Row % 2) && (Column % 2) && Column * Row != 0 && Row != RowNum - 1 && Column != ColumnNum - 1; // 0-wall  1-floor  3-room
			StaticMeshComp->SetRelativeScale3D(FVector(1.0f));
			StaticMeshComp->SetRelativeLocation(FVector((Row - RowNum / 2) * 100, (Column - ColumnNum / 2) * 100, (CubeType == 1 ? 1:0 )*-100)); //地面下降100
			StaticMeshComp->SetStaticMesh(CubeMesh);
			//判断是否可以作为道路点
			MapGrid[Row][Column] = CubeType;
			
			Meshes.Add(StaticMeshComp);
		}
	}

	FysRoomGenerate();
	KruskalRoadGenerate();
	
	
}

void ARandomMapActor::KruskalRoadGenerate()
{
	// Kruskal生成随机地图
	
	
}

void ARandomMapActor::FysRoomGenerate()
{
	// 洗牌算法生成房间
	
	TArray<TTuple<int ,int>> GridArray;
	for(int i=0 ; i<RowNum ; i++)
	{
		for(int j=0 ; j<ColumnNum ; j++)
		{
			// 仅选择floor
			if(MapGrid[i][j] == 1)
			{
				GridArray.Add({i,j});
			}
		}
	}

	for(int i=0 ; i<GridArray.Num() ; i++)
	{
		int temp = i + UKismetMathLibrary::RandomInteger(GridArray.Num()-1 - i);
		Swap(GridArray[i] , GridArray[temp]);
	}

	int GeneratedRoomNum = 0;
	int Index = 0;
	TMap<TTuple<int , int> , TTuple<int , int>> RoomMap; // 坐标-长款
	
	while(GeneratedRoomNum < RoomNum && Index < GridArray.Num())
	{
		// 房间左下角坐标
		auto[R , L] = GridArray[Index];
		// 随机到的点不是房间
		if(MapGrid[R][L] != 3 && MapGrid[R][L] != 0)
		{
			// 生成房间，随机长宽
			int Width = 5 + UKismetMathLibrary::RandomInteger(FMath::Min(RowNum , ColumnNum)/8 * 2);
			int Height = 5 + UKismetMathLibrary::RandomInteger(FMath::Min(RowNum , ColumnNum)/8 * 2);

			// 不重叠
			bool flag = false;

			for (auto Room : RoomMap)
			{
				/*
						  [r2,l2]
					-----
					|   |
					-----
			[r1,l1]

						  [R2,L2]
					-----
					|   |
					-----
			[R,L]
			
				*/
				
				// 矩形左下角
				int r1 = Room.Get<0>().Get<0>();
				int l1 = Room.Get<0>().Get<1>();
				// 矩形右上角
				int r2 = r1 + Room.Get<1>().Get<1>();
				int l2 = l1 + Room.Get<1>().Get<0>();
				
				// 新房间右上角
				int R2 = R + Height;
				int L2 = L + Width;
				
				if(FMath::Max(r1 , R) < FMath::Min(r2 , R2) && FMath::Max(l1 , L) < FMath::Min(l2 , L2))
				{
					// 重叠
					flag = true;
					break;
				}
				
			}
			
			if(flag)
			{
				Index++;
				continue;
			}
			
			for(int i=0 ; i<Height ; i++)
			{
				for(int j=0 ; j<Width ; j++)
				{
					if(R+i >= RowNum-1 || L+j >= ColumnNum-1)
					{
						continue;
					}
					// 房间轮廓
					if(i == 0 || j == 0 || i == Height-1 || j == Width - 1)
					{
						FVector Location = Meshes[(R+i) * ColumnNum + (L+j)]->GetRelativeLocation();
						Meshes[(R+i) * ColumnNum + (L+j)]->SetRelativeLocation(FVector(Location.X,Location.Y,0)); // wall
						MapGrid[R+i][L+j] = 0;
					}else
					{
						// 房间地面
						FVector Location = Meshes[(R+i) * ColumnNum + (L+j)]->GetRelativeLocation();
						Meshes[(R+i) * ColumnNum + (L+j)]->SetRelativeLocation(FVector(Location.X,Location.Y,-100)); // floor
						MapGrid[R+i][L+j] = 3;
					}
					
				}
			}
			
			GeneratedRoomNum++;
			RoomMap.Emplace({R,L} , {Width , Height});
		}
		Index++;
		
	}
}

