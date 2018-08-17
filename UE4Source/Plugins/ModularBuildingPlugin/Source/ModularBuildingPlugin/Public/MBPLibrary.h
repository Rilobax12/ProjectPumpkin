// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
//#include "Runtime/CoreUObject/Public/UObject/Object.h"

#include "DestructibleMesh.h" 
#include "DestructibleActor.h"		
#include "DestructibleComponent.h"

#include "MBPLibrary.generated.h"
//#include "MBPFloorActor.h"

UENUM(BlueprintType)
enum class ESelectBuildingParts : uint8
{
	Floor 	UMETA(DisplayName = "Floor"),
	Wall 	UMETA(DisplayName = "Wall"),
	WallAndFloor UMETA(DisplayName = "WallAndFloor"),
	Grid 	UMETA(DisplayName = "Grid"),
	All 	UMETA(DisplayName = "All")
};

UENUM(BlueprintType)
enum class ETypeOfWalls : uint8
{
	Left 	UMETA(DisplayName = "Left"),
	Right 	UMETA(DisplayName = "Right"),
	Standard	UMETA(DisplayName = "Standard"),
	Nothing		UMETA(DisplayName = "Nothing")
};

UENUM(BlueprintType)
enum class EToMove : uint8
{
	X UMETA(DisplayName = "X"),
	InvertX UMETA(DisplayName = "-X"),
	Y UMETA(DisplayName = "Y"),
	InvertY UMETA(DisplayName = "-Y")
};

UENUM(BlueprintType)
enum class EOrientationByX : uint8
{
	Center		UMETA(DisplayName = "Center"),
	CenterBottom UMETA(DisplayName = "CenterBottom"),
	CenterTop	 UMETA(DisplayName = "CenterTop"),
	LeftBottom 	UMETA(DisplayName = "LeftBottom"),
	RightBottom	UMETA(DisplayName = "RightBottom"),
	LeftTop 	UMETA(DisplayName = "LeftTop"),
	RightTop	UMETA(DisplayName = "RightTop")
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Left 	UMETA(DisplayName = "Left"),
	Right 	UMETA(DisplayName = "Right"),
	Up		UMETA(DisplayName = "Up"),
	Down	UMETA(DisplayName = "Down")
};

UENUM(BlueprintType)
enum class EVectorPart : uint8
{
	X 	UMETA(DisplayName = "X"),
	Y 	UMETA(DisplayName = "Y"),
	Z		UMETA(DisplayName = "Z")
};

UENUM(BlueprintType)
enum class EBuildingMode : uint8
{
	OutsideAndInside 	UMETA(DisplayName = "OutsideAndInside"),
	InsideOnly 	UMETA(DisplayName = "InsideOnly"),
	Custom		UMETA(DisplayName = "Custom")
};



class MODULARBUILDINGPLUGIN_API MBPLibrary
{
public:
	MBPLibrary();
	~MBPLibrary();

	UFUNCTION()
	static FVector OptimizeVector(FVector ThisVector);

	UFUNCTION()
	static double MyRound(const double x);

	UPROPERTY() FTransform DefaultTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.f, 1.f, 1.f));
};

