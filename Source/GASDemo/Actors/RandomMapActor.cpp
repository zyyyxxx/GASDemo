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
			int CubeType = (Row % 2) && (Column % 2) && Column * Row != 0 && Row != RowNum - 1 && Column != ColumnNum - 1; // 0-wall  1-floor  3-room floor  4-room wall 
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

	// 房间链接迷宫

	for(auto Room : RoomMap)
	{
		int width = Room.Get<1>().Get<0>();
		int height = Room.Get<1>().Get<1>();
		int r1 = Room.Get<0>().Get<0>();
		int c1 = Room.Get<0>().Get<1>();

		TArray<TTuple<int ,int>> RoomWallCoord;
		
		for(int r = r1 ; r < r1+height ; r++)
		{
			for(int c = c1 ; c < c1+width ; c++)
			{
				if(r > RowNum-1 || c > ColumnNum-1) continue;
				if(MapGrid[r][c] == 4)
				{
					// 房间墙壁位置数组
					RoomWallCoord.Add({r,c});
				}
			}
		}

		if(RoomWallCoord.Num() == 0) continue;
		
		// 对每个房间的墙壁进行随机选取
		bool bSuccessful = false;
		while(!bSuccessful)
		{
			if(RoomWallCoord.Num() == 0) break;
			int RandomIndex = UKismetMathLibrary::RandomInteger(RoomWallCoord.Num());

			auto [R,C] = RoomWallCoord[RandomIndex];

			int dirs[4][2] = {{1,0} , {-1,0} , {0,1} , {0,-1}};

			// 遍历四个方向
			for(int i=0 ; i<4 ; i++)
			{
				int NextR = R + 2*dirs[i][0];
				int NextC = C + 2*dirs[i][1];

				if(NextR <= 0 || NextR >= RowNum-1 || NextC <= 0 || NextC >= ColumnNum-1) continue;

				if(MapGrid[NextR][NextC] == 1)
				{
					R = NextR - dirs[i][0];
					C = NextC - dirs[i][1];
					FVector Location = Meshes[(R) * ColumnNum + (C)]->GetRelativeLocation();
					Meshes[(R) * ColumnNum + (C)]->SetRelativeLocation(FVector(Location.X,Location.Y,-100)); // floor
					bSuccessful = true;
					break;
				}
			}

			// 四个方向都失败
			RoomWallCoord.RemoveAt(RandomIndex);
		}


		
	}
		
	
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
	
	while(GeneratedRoomNum < RoomNum && Index < GridArray.Num())
	{
		// 房间左下角坐标
		auto[R , L] = GridArray[Index];
		// 随机到的点不是房间
		if(MapGrid[R][L] != 3 && MapGrid[R][L] != 0)
		{
			// 生成房间，随机长宽
			int Width = 5 + UKismetMathLibrary::RandomInteger(FMath::Min(RowNum , ColumnNum)/4);
			Width += (Width % 2 ? 0 : 1);
			int Height = 5 + UKismetMathLibrary::RandomInteger(FMath::Min(RowNum , ColumnNum)/4);
			Height += (Height % 2 ? 0 : 1);

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
					
					FVector Location = Meshes[(R+i) * ColumnNum + (L+j)]->GetRelativeLocation();
					Meshes[(R+i) * ColumnNum + (L+j)]->SetRelativeLocation(FVector(Location.X,Location.Y,-100)); // floor
					MapGrid[R+i][L+j] = 3; // 房间地面
					if(i == 0 || j == 0 || i == Height-1 || j == Width-1)
					{
						MapGrid[R+i][L+j] = 4; // 房间墙壁
					}
				}
			}
			
			GeneratedRoomNum++;
			RoomMap.Emplace({R,L} , {Width , Height});
		}
		Index++;
		
	}
}



void ARandomMapActor::KruskalRoadGenerate()
{
	// Kruskal生成随机地图
	TArray<TTuple<int, int>> WallArray;	// 可用的墙
	TMap<TTuple<int, int>, TTuple<int, int>> Parent;
	TMap<TTuple<int, int>, int> Rank;	

	// 初始化并查集
	for(int r = 1 ; r < RowNum-1 ; r++)
	{
		for(int c = 1 ; c < ColumnNum-1 ; c++)
		{
			TTuple<int, int> Coord = TTuple<int, int>(r, c);
			Parent.Emplace(Coord , Coord);
			Rank.Add(Coord, 1);
			if(MapGrid[r][c] == 0)
			{
				WallArray.Emplace(r,c);
			}
		}
	}
	

	std::function<TTuple<int,int>(const TTuple<int, int>&)> Find = [&](const TTuple<int, int>& Coord)
	{
		if(Parent[Coord] != Coord)
		{
			Parent[Coord] = Find(Parent[Coord]);
		}

		return Parent[Coord];
	};

	auto Union = [&](const TTuple<int, int>& Coord1, const TTuple<int, int>& Coord2)
	{
		auto Root1 = Find(Coord1);
		auto Root2 = Find(Coord2);
		if (Rank[Root1] <= Rank[Root2])
			Parent[Root1] = Root2;
		else
			Parent[Root2] = Root1;
		if (Rank[Root1] == Rank[Root2] && Root1 != Root2)
			Rank[Root2]++;
	};

	while(WallArray.Num() > 0)
	{
		//随机选取墙进行联通
		int Index = UKismetMathLibrary::RandomInteger(WallArray.Num());
		auto [Row , Col] = WallArray[Index];

		//取墙体两边的点
		TTuple<int, int> P1 = Col % 2 ? TTuple<int, int>(Row-1, Col) : TTuple<int, int>(Row, Col-1);
		TTuple<int, int> P2 = Col % 2 ? TTuple<int, int>(Row+1, Col) : TTuple<int, int>(Row, Col+1);

		if(MapGrid[P1.Get<0>()][P1.Get<1>()] == 1 && MapGrid[P2.Get<0>()][P2.Get<1>()] == 1 && Find(P1) != Find(P2))
		{
			// 合并无交集的两个floor
			Union(P1 , P2);
			// 移除该墙体
			Parent[{Row , Col}] = P1;
			MapGrid[Row][Col] = 1;
			FVector Location = Meshes[(Row) * ColumnNum + (Col)]->GetRelativeLocation();
			Meshes[(Row) * ColumnNum + (Col)]->SetRelativeLocation(FVector(Location.X,Location.Y,-100)); // floor
		}
		WallArray.Swap(Index , WallArray.Num()-1);
		WallArray.Pop();
		
	}
	
	
}