// Fill out your copyright notice in the Description page of Project Settings.

#include "../Private/ModularBuildingPluginPrivatePCH.h"
#include "BuildingWall.h"
#include "MBPFloorActor.h"
//#include "MBPLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

//Set
void UBuildingWall::SetTypeOfThisWall(ETypeOfWalls NewType)
{
	TypeOfThisWall = NewType;
}

void UBuildingWall::SetCreateWithOneWallMode(bool bNewSet)
{
	bCreateWithOneWallMode = bNewSet;
}

//Get
ETypeOfWalls  UBuildingWall::GetTypeOfThisWall()
{
	return TypeOfThisWall;
}

bool UBuildingWall::GetCreateWithOneWallMode()
{
	return bCreateWithOneWallMode;
}