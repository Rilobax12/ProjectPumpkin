// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingPart.h"
#include "BuildingWall.generated.h"

/**
 * 
 */
UCLASS()
class MODULARBUILDINGPLUGIN_API UBuildingWall : public UBuildingPart
{
	GENERATED_BODY()

public:
		//Set
		void SetTypeOfThisWall(ETypeOfWalls NewType);

		void SetCreateWithOneWallMode(bool bNewSet);

		//Get
		ETypeOfWalls GetTypeOfThisWall();

		bool GetCreateWithOneWallMode();


protected:

	UPROPERTY() ETypeOfWalls TypeOfThisWall = ETypeOfWalls::Standard;

	UPROPERTY() bool bCreateWithOneWallMode = false;

};