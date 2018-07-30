// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"

#include "BuildingFloor.h"
#include "BuildingWall.h"
#include "MBPLibrary.h"

#include "BuildingCell.generated.h"

//UENUM(BlueprintType)
//enum class EBuildingPartsOnIt : uint8
//{
//	Floor 	UMETA(DisplayName = "Floor"),
//	Floor 	UMETA(DisplayName = "Floor"),
//	All 	UMETA(DisplayName = "All")
//};

UCLASS()
class MODULARBUILDINGPLUGIN_API UBuildingCell : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:

	//Add
	void AddBuildingFloor(UBuildingFloor* NewBuildingFloor);

	void AddBuildingWall(UBuildingWall* NewBuildingWall);

	//Set
	void SetCellLocation(FVector NewLocation);

	//Get
	TArray<TWeakObjectPtr<UBuildingWall>> GetBuildingWallsOnIt();

	TArray<TWeakObjectPtr<UBuildingFloor>> GetBuildingFloorsOnIt();

	FVector GetCellLocation();

	TArray<TWeakObjectPtr<UBuildingWall>> GetBuildingWallsByRotation(FRotator SelecredRotator, bool bGetCreatedWithOneWallModeOnly = false);

	TArray<TWeakObjectPtr<UBuildingFloor>> GetBuildingFloorsByRotation(FRotator SelecredRotator);

	TArray<TWeakObjectPtr<UBuildingWall>> GetBuildingWallByTypeOfWalls(ETypeOfWalls SelectedType, bool bGetCreatedWithOneWallModeOnly = false);

	TArray<TWeakObjectPtr<UBuildingWall>> GetBuildingWallsByTypeAndRotation(ETypeOfWalls SelectedType, FRotator SelecredRotator, bool bGetCreatedWithOneWallModeOnly = false);

	//Delete
	void DeleteBuildingPartsOnIt(ESelectBuildingParts SelectParts);

	void DeleteSelectBuildingWallOnIt(TWeakObjectPtr<UBuildingWall> SelectWall);

	void DeleteSelectBuildingFloorOnIt(TWeakObjectPtr<UBuildingFloor> SelectFloor);

	//Check
	bool CheckWallIsValid(TWeakObjectPtr<UBuildingWall> SelectWall);

	bool CheckFloorIsValid(TWeakObjectPtr<UBuildingFloor> SelectFloor);


	UPROPERTY(EditAnywhere) int32 X = 0;

	UPROPERTY(EditAnywhere) int32 Y = 0;
	
protected:

	UPROPERTY() FVector ChangeVector = FVector(0.f, 0.f, 1.f);

	UPROPERTY(EditAnywhere) TArray<TWeakObjectPtr<UBuildingWall>> BuildingWallsOnIt;

	UPROPERTY(EditAnywhere) TArray<TWeakObjectPtr<UBuildingFloor>> BuildingFloorsOnIt;

};
