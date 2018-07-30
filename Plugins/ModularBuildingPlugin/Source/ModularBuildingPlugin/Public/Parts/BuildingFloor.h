// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingPart.h"
#include "BuildingFloor.generated.h"

/**
 * 
 */
UCLASS()
class MODULARBUILDINGPLUGIN_API UBuildingFloor : public UBuildingPart
{
	GENERATED_BODY()

public:
	
	UPROPERTY() int32 BuildingFloorHeight = 0;

	UPROPERTY() bool bTurnCeiling = false;
};
