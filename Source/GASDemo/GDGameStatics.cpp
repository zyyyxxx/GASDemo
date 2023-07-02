// Fill out your copyright notice in the Description page of Project Settings.


#include "GDGameStatics.h"

#include "GameTypes.h"

const UItemStaticData* UGDGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if(IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass); // 返回类的默认对象
	}

	return nullptr;
}
