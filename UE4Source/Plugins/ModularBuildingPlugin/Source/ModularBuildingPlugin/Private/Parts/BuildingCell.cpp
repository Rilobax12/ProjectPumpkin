// Fill out your copyright notice in the Description page of Project Settings.

#include "../Private/ModularBuildingPluginPrivatePCH.h"
#include "BuildingCell.h"

//Add
void UBuildingCell::AddBuildingFloor(UBuildingFloor* NewBuildingFloor)
{
	BuildingFloorsOnIt.Add(NewBuildingFloor);
}

void UBuildingCell::AddBuildingWall(UBuildingWall* NewBuildingWall)
{
	BuildingWallsOnIt.Add(NewBuildingWall);
}

//Set
void UBuildingCell::SetCellLocation(FVector NewLocation)
{
	this->SetRelativeLocation(NewLocation + ChangeVector);
}

//Get
TArray<TWeakObjectPtr<UBuildingWall>> UBuildingCell::GetBuildingWallsOnIt()
{
	TArray<TWeakObjectPtr<UBuildingWall>> ReturnArray;

	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWallsOnIt)
	{
		if (CheckWallIsValid(ItWall))
			ReturnArray.Add(ItWall);
	}

	return ReturnArray;
}

TArray<TWeakObjectPtr<UBuildingFloor>> UBuildingCell::GetBuildingFloorsOnIt()
{
	TArray<TWeakObjectPtr<UBuildingFloor>> ReturnArray;

	for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloorsOnIt)
	{
		if (CheckFloorIsValid(ItFloor))
			ReturnArray.Add(ItFloor);
	}

	return ReturnArray;
}

FVector UBuildingCell::GetCellLocation()
{
	return FVector(this->RelativeLocation.X , this->RelativeLocation.Y , 0.f) ;
}

TArray<TWeakObjectPtr<UBuildingWall>> UBuildingCell::GetBuildingWallsByRotation(FRotator SelecredRotator, bool bGetCreatedWithOneWallModeOnly)
{
	TArray<TWeakObjectPtr<UBuildingWall>> ReturnWalls;
	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWallsOnIt)
	{
		if (CheckWallIsValid(ItWall))
		{
			if (ItWall->GetRelativeRotation().Equals(SelecredRotator))
			{
				if (bGetCreatedWithOneWallModeOnly ? ItWall->GetCreateWithOneWallMode() : true)
					ReturnWalls.Add(ItWall);
			}
		}
	}
	return ReturnWalls;
}

TArray<TWeakObjectPtr<UBuildingFloor>> UBuildingCell::GetBuildingFloorsByRotation(FRotator SelecredRotator)
{
	TArray<TWeakObjectPtr<UBuildingFloor>> ReturnFloors;
	for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloorsOnIt)
	{
		if (CheckFloorIsValid(ItFloor))
		{
			if (ItFloor->GetRelativeRotation().Equals(SelecredRotator))
				ReturnFloors.Add(ItFloor);
		}
	}
	return ReturnFloors;
}

TArray<TWeakObjectPtr<UBuildingWall>> UBuildingCell::GetBuildingWallByTypeOfWalls(ETypeOfWalls SelectedType, bool bGetCreatedWithOneWallModeOnly)
{
	TArray<TWeakObjectPtr<UBuildingWall>> ReturnWalls;
	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWallsOnIt)
	{
		if (CheckWallIsValid(ItWall))
		{
			if (ItWall->GetTypeOfThisWall() == SelectedType)
			{
				if (bGetCreatedWithOneWallModeOnly ? ItWall->GetCreateWithOneWallMode() : true)
					ReturnWalls.Add(ItWall);
			}
		}
	}
	return ReturnWalls;
}

TArray<TWeakObjectPtr<UBuildingWall>> UBuildingCell::GetBuildingWallsByTypeAndRotation(ETypeOfWalls SelectedType, FRotator SelecredRotator, bool bGetCreatedWithOneWallModeOnly )
{
	TArray<TWeakObjectPtr<UBuildingWall>> ReturnWalls;
	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWallsOnIt)
	{
		if (CheckWallIsValid(ItWall))
		{
			if (ItWall->GetTypeOfThisWall() == SelectedType && ItWall->GetRelativeRotation().Equals(SelecredRotator, 5.f))
			{
				if(bGetCreatedWithOneWallModeOnly ? ItWall->GetCreateWithOneWallMode() : true)
					ReturnWalls.Add(ItWall);
			}
		}
	}
	return ReturnWalls;
}

//Delete
void UBuildingCell::DeleteBuildingPartsOnIt(ESelectBuildingParts SelectParts)
{
	if(SelectParts == ESelectBuildingParts::Wall)
	BuildingWallsOnIt.Empty();
	if(SelectParts == ESelectBuildingParts::Floor)
	BuildingFloorsOnIt.Empty();
	if (SelectParts == ESelectBuildingParts::All)
	{
		DeleteBuildingPartsOnIt(ESelectBuildingParts::Wall);
		DeleteBuildingPartsOnIt(ESelectBuildingParts::Floor);
	}
}

void UBuildingCell::DeleteSelectBuildingWallOnIt(TWeakObjectPtr<UBuildingWall> SelectWall)
{
	if (SelectWall.IsValid())
	{
		if (BuildingWallsOnIt.Find(SelectWall) != INDEX_NONE)
			BuildingWallsOnIt.RemoveAt(BuildingWallsOnIt.Find(SelectWall));
	}
}

void UBuildingCell::DeleteSelectBuildingFloorOnIt(TWeakObjectPtr<UBuildingFloor> SelectFloor)
{
	if (SelectFloor.IsValid())
	{
		if (BuildingFloorsOnIt.Find(SelectFloor) != INDEX_NONE)
			BuildingFloorsOnIt.RemoveAt(BuildingFloorsOnIt.Find(SelectFloor));
	}
}

//Check
bool UBuildingCell::CheckWallIsValid(TWeakObjectPtr<UBuildingWall> SelectWall)
{
	if (SelectWall.IsValid())
		return true;
	else 
		if(BuildingWallsOnIt.Find(SelectWall) != INDEX_NONE)
		BuildingWallsOnIt.RemoveAt(BuildingWallsOnIt.Find(SelectWall));
	return false;
}

bool UBuildingCell::CheckFloorIsValid(TWeakObjectPtr<UBuildingFloor> SelectFloor)
{
	if (SelectFloor.IsValid())
		return true;
	else
		if (BuildingFloorsOnIt.Find(SelectFloor) != INDEX_NONE)
			BuildingFloorsOnIt.RemoveAt(BuildingFloorsOnIt.Find(SelectFloor));
	return false;
}