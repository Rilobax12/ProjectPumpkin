// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"


#include "BuildingCell.h"
#include "MBPLibrary.h"

#include "MBPFloorActor.generated.h"


USTRUCT()
struct FGridY
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()

	UPROPERTY() TArray<UBuildingCell*> GridsByY;

	FGridY()
	{

	}
};

USTRUCT()
struct FGridX
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()

	UPROPERTY() TArray<FGridY> SetsOfGridsByX;

	TWeakObjectPtr<UBuildingCell> Get(int32 NeedX, int32 NeedY)
	{
		if (SetsOfGridsByX.IsValidIndex(NeedX))
		{
			if (SetsOfGridsByX[NeedX].GridsByY.IsValidIndex(NeedY))
				return SetsOfGridsByX[NeedX].GridsByY[NeedY];
			else return NULL;	
		}
		else return NULL;
	}

	int32 GetNumByX()
	{
		return SetsOfGridsByX.Num();
	}

	int32 GetNumByY()
	{
		if(SetsOfGridsByX.IsValidIndex(0))
		return SetsOfGridsByX[0].GridsByY.Num();
		else return 0;
	}

	void AddSetOfGridByX()
	{
		SetsOfGridsByX.Add(FGridY());
	}

	FGridX()
	{

	}
};

USTRUCT()
struct FObjectProperties
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() TWeakObjectPtr<UObject> NewObject;

	UPROPERTY() float WallOffset = 0.f;

	UPROPERTY() FVector ObjectChangeVector = FVector::ZeroVector;

	void Empty()
	{
		NewObject = NULL;
		WallOffset = 0;
		ObjectChangeVector = FVector::ZeroVector;
	}

	FObjectProperties()
	{

	}

};

USTRUCT()
struct FObjectsForCreating
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()

	UPROPERTY() FObjectProperties MainObject;

	UPROPERTY() FObjectProperties LeftObject;

	UPROPERTY() FObjectProperties RightObject;

	UPROPERTY()	TArray<UBuildingCell*> SelectedCells;

	UPROPERTY() bool bSelectOneCell = false;

	void Empty()
	{
		MainObject.Empty();
		LeftObject.Empty();
		RightObject.Empty();

		SelectedCells.Empty();
		bSelectOneCell = false;
	}

	FObjectsForCreating()
	{

	}
};

USTRUCT()
struct FMaterialStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() UMaterialInterface*  Material;

	UPROPERTY() FString SlotName;

	//UPROPERTY() int32 Index = 0;

	TWeakObjectPtr<UMaterialInterface> GetWeakObjectMaterial()
	{
		TWeakObjectPtr<UMaterialInterface> ReturnMaterial = Material;

		return ReturnMaterial;
	}

	FMaterialStruct()
	{

	}

};

USTRUCT()
struct FCopiedCell
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() TArray<TWeakObjectPtr<UBuildingWall>> BuildingWallsOnIt;

	UPROPERTY() TArray<TWeakObjectPtr<UBuildingFloor>> BuildingFloorsOnIt;

	UPROPERTY() ESelectBuildingParts SelectedDeletePart = ESelectBuildingParts::WallAndFloor;

	UPROPERTY() int32 DefAddX = 0;

	UPROPERTY() int32 DefAddY = 0;

	UPROPERTY() int32 AddX = 0;

	UPROPERTY() int32 AddY = 0;

	FCopiedCell()
	{

	}
};

UCLASS()
class MODULARBUILDINGPLUGIN_API AMBPFloorActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform & Transform) override;

	virtual void PostActorCreated() override;

	//Array with Walls
	UPROPERTY(EditAnywhere, Category = Spawning) TArray<UBuildingWall*> BuildingWalls;

	//Array with Floors
	UPROPERTY(EditAnywhere, Category = Spawning) TArray<UBuildingFloor*> BuildingFloors;

//Grid
	void SpawnGrid();

	//Select Grid And set Visibility true for all grids in radius
	void VisibilityOfCellsInRadius(UBuildingCell* SelectedCell);

	void ClearGrid();

	void UpdateGridLocation(bool bCheckAllFloors = true, float NewFloorHeight = 0.f);

	float GetHeightOfGrid();

	//Scale grid element
	UPROPERTY() FGridX ThisGrid;

	UPROPERTY()	float GridScale;

	UPROPERTY() int32 GridX;

	UPROPERTY() int32 GridY;

	UPROPERTY() int32 SelectGridRad;

	UPROPERTY() UStaticMesh* Grid_StaticMesh = NULL;

//Material
	void SetWallMaterial(TArray<UBuildingCell*> SelectedCells);

	void SetWallMaterialMacros(TWeakObjectPtr<UBuildingCell> SelectedCell, FRotator SelectedRotation);
		//**
	UPROPERTY() FMaterialStruct OuterMaterial;

	UPROPERTY() FMaterialStruct InnerMaterial;

//InstancedStaticMesh
	void AllToInstMesh();

	void AllToStaticMesh();
		//***
	UPROPERTY(EditAnywhere) UInstancedStaticMeshComponent* SelecetSpawnParam;

//Changes
	void TurnWallOnSelectCell(UBuildingCell* SelectCell);

	void MoveBuildingParts(EToMove ToMove);

	void TurnCeiling(bool TurnSet);

	void CopyBuildingPartsOnSelectedCells(TArray<UBuildingCell*> SelectedCells);

	void PasteCopiedBuildingParts(UBuildingCell* SelectedCell);

	void AllotCellsForCopy(UBuildingCell* SelectedCell);

	void TurnCopiedParts();

	void Debug();
	//***
	TArray<FCopiedCell> CopiedCells;

	TArray<TWeakObjectPtr<UBuildingCell>> UseForCopyCells;

	FRotator ChangeAllottedCellsRotation = FRotator::ZeroRotator;

	FRotator AddCopiedRotation = FRotator::ZeroRotator;

	bool bIsCutBuildingParts = false;

	ESelectBuildingParts SelectedCopyPart = ESelectBuildingParts::WallAndFloor;

//Creat
	void CreatWallsByObjectsFBM(FObjectsForCreating NewObjects);

	void CreateLeftTypeWall(FObjectsForCreating NewObjects, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> ItCell, bool bIsCreateOnlyLeftWall = false);

	void CreatFloorsByComponent(UObject* NewComponent, FVector ChangeVector, TArray<UBuildingCell*> SelectedCells);

	bool CreatBuildingPart(TWeakObjectPtr<UObject> NewComponent, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ESelectBuildingParts SelectPart, ETypeOfWalls SelectTypeOfWall = ETypeOfWalls::Standard);

	bool ReplaceSelectedWallToNewWall(TWeakObjectPtr<UBuildingWall> SelectedWall, FObjectsForCreating NewObjects);

	//Static  Mesh
	bool CreatWallByStaticMesh(UStaticMesh* SelectStaticMesh, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType);

	bool CreatFloorByStaticMesh(UStaticMesh* SelectStaticMesh, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell);

	//Use Blueprint
	bool CreatWallByBlueprint(UBlueprint* SelectBlueprint, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType);

	bool CreatFloorByBlueprint(UBlueprint* SelectBlueprint, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell);

	//Use Actor
	bool CreatWallByActor(AActor* SelectActor, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType);

	bool CreatFloorByActor(AActor* SelectActor, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell);

	//Destructible Mesh
	bool CreatWallByDestructibleMesh(UDestructibleMesh* SelectDestructibleMesh, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType);

	bool CreatFloorByDestructibleMesh(UDestructibleMesh* SelectDestructibleMesh, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell);

	//Change Transform
	FTransform ChangeTransformForWall(FBox ItBox, float NewOffset, FVector ChangeVector, FTransform NewTransform, ETypeOfWalls NewType);

	FTransform ChangeTransformForFloor(FBox ItBox, FVector ChangeVector, FTransform NewTransform);

	FTransform ShiftInDirection(FVector FirstLocation, FTransform ItTransform, int32 ItOffset , EDirection ItDirection , bool bForthMeansLarger = true);

	//***
	UPROPERTY() FRotator RotationForNewBuildingFloors;

//Delete
	void DeleteOnCell(TWeakObjectPtr<UBuildingCell> ItCell, ESelectBuildingParts ItSelectPart, bool bUseDeleteOnlyFromListMode = true);

	void DeleteOnCellByRotation(TWeakObjectPtr<UBuildingCell> ItCell, FRotator ItRotation, ESelectBuildingParts ItSelectPart, bool bUseDeleteOnlyFromListMode = true);

	void DeleteWallsOnCellByTypeAndRotation(TWeakObjectPtr<UBuildingCell> ItCell, ETypeOfWalls SelectedType, FRotator ItRotation, bool bUseDeleteOnlyFromListMode = true);

	void DeleteSelectedWallOnSelectedCell(TWeakObjectPtr<UBuildingCell> ItCell, TWeakObjectPtr<UBuildingWall> ItWall);

	void DeleteBuildingWall(TWeakObjectPtr<UBuildingWall> DeletingWall);

	void DeleteBuildingFloor(TWeakObjectPtr<UBuildingFloor> DeletingFloor);

	void SetNewListOfObjectToDelete(TArray<TWeakObjectPtr<UObject>> NewList);

	void DeleteAllWallsAndFloors();

	void DeleteOnSelectedCells(ESelectBuildingParts SelectPart, TArray<UBuildingCell*> SelectedCells);

	void DeferredDeleteAdd(TWeakObjectPtr<UBuildingPart> NewPart, ESelectBuildingParts SelectPart);

	void DeferredDeleteStart();

	void DeleteLastWall();

	void DeleteInOneWallMode(TWeakObjectPtr<UBuildingCell> SelectedCell, FRotator ItRotation);

	bool DeleteBuildingPartBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent);

//Get
	FVector GetLocationByOrientation(FBox ItBox, FVector ChangeVector, FTransform ItTransform, EOrientationByX SelectOrientation);

	FVector GetCellBorderLocation(FTransform ItTransform, float ItOffset, EOrientationByX SelectOrientation);

	ETypeOfWalls GetTypeOfThisObject(TWeakObjectPtr<UObject> ItObject);

	FVector GetSizeOfThisObject(TWeakObjectPtr<UObject> ItObject);
	
	TWeakObjectPtr<UBuildingCell> GetCellAroundByDirection(EDirection SelectedDirection , TWeakObjectPtr<UBuildingCell> SelectedCell , FRotator  RotationOfRoot = FRotator::ZeroRotator);

	TWeakObjectPtr<UBuildingCell> GetCellByCellLocation(FVector CellLocation);

	TWeakObjectPtr<UBuildingWall> GetBuildingWallBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent);

	TWeakObjectPtr<UBuildingFloor> GetBuildingFloorBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent);

//Mode
	bool GetDeleteOnlyFromListMode();

	void SetDeleteOnlyFromListMode(bool bNewDeleteOnlyFromList);

	bool GetOneWallMode();

	void SetOneWallMode(bool bNewOneWallMode);

	bool GetFloorUnderGridCellMode();

	void SetFloorUnderGridCellMode(bool bNewFloorUnderGridCell);

	EBuildingMode GetSelectedBuildingMode();

	void SetBuildingMode(EBuildingMode NewBuildingMode);

//Castom
	bool IsOverlapeWithWallAround(TWeakObjectPtr<UBuildingCell> ItCell, FRotator ItRotator);

//Vision
	void SetNewVision(ESelectBuildingParts SelectParts, bool bNewVision);

//FloorActor
	UPROPERTY() FVector ChangeFloorActorLocation = FVector::ZeroVector;

protected:

//Grid
	UPROPERTY() float HeightOfGrid;

//Delete
	UPROPERTY() TArray<TWeakObjectPtr<UBuildingWall>> DeferredDeleteWalls;

	UPROPERTY() TArray<TWeakObjectPtr<UBuildingFloor>> DeferredDeleteFloors;
//Castom


//FastMode
	TArray<TWeakObjectPtr<UObject>> ListOfObjectToDelete;

//Mode
	 bool bDeleteOnlyFromList = false;
		
	UPROPERTY() bool bOneWallMode = false;

	UPROPERTY() bool bFloorUnderGridCell = false;

	UPROPERTY() EBuildingMode SelectedBuildingMode;
};
