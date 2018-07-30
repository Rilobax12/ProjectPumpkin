// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MBPFloorActor.h"
#include "MBPLibrary.h"
#include "MBPComponentsForCreating.h"
#include "MBPBuilding.generated.h"


UCLASS()
class MODULARBUILDINGPLUGIN_API AMBPBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBPBuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostActorCreated() override;

	//Get
	TArray<TWeakObjectPtr<AMBPFloorActor>> GetChildFloorsActors();

	TWeakObjectPtr<AMBPFloorActor> GetSelectedChildFloorActor();

	//Spawn
	TWeakObjectPtr<AMBPFloorActor> SpawnNewChildFloorActor();

	TWeakObjectPtr<AMBPFloorActor> CopyThisChildFloorActor(TWeakObjectPtr<AMBPFloorActor> ThisChildFloorActor);

	void AfterCopiedUpdate();

	//Select New 
	void SelectChildFloorActor(TWeakObjectPtr<AMBPFloorActor> NewSet, bool bIgnoreSelfLock = false);

	void SelectNone();

	//Update
	void UpdateChildFloorsActors();

	void UpdateBuildingPartOnFloorActor(TWeakObjectPtr<AMBPFloorActor> SelectedFloorActor, TWeakObjectPtr<AMBPFloorActor> ParentFloorActor);

	//Vision
	void SetVisionForSelectedFloorActor(ESelectBuildingParts Select, bool bNewVision);

	void SetVisionForThisFloorActor(TWeakObjectPtr<AMBPFloorActor> ThisFloorActor, ESelectBuildingParts Select, bool bNewVision);

	void SetVisionForSeveralFloorsActors(TWeakObjectPtr<AMBPFloorActor> InitialFloorActor, ESelectBuildingParts Select);

	void SetVisionForAllFloorsActors(ESelectBuildingParts Select, bool bNewVision);

	UPROPERTY() bool bAutoReplaceStaticInst = true;

	//Click
	UFUNCTION() void OnClick(UObject* ThisObject);

	//Fast Build Mode
	void SetSelectedComponentsForFBM(int32 SelectedComponentIndex, TWeakObjectPtr<AMBPComponentsForCreating> SelectedComponentsForCreating);

	bool GetFastBuildMode();

	void SetFastBuildMode(bool bNewFastBuildMode);

	//Creat
	void CreateFloorsByObject(int32 SelectedComponentIndex, TWeakObjectPtr<AMBPComponentsForCreating> SelectedComponentsForCreating);

	//Delete
	void DeleteOnSelectCells(ESelectBuildingParts SelectPart);

	void DeleteSelectedBuildingParts();

	//Change
	void TurnWallOnSelectCell();

	void MoveBuildingPartsOnSelectedFloorActor(EToMove ToMove);

	void DebugOnSelectedBuildingFloor();
	//Copy\Paste
	void CopySelectedBuildingParts();

	void PasteCopiedBuildingParts();

	void CutSelectedBuildingParts();

	ESelectBuildingParts GetSelectedCopyPart();

	void SetSelectedCopyPart(ESelectBuildingParts NewSelectedCopyPart);

	void TurnCopiedParts();

	void ClearCopiedParts();

	//Material
	void SetWallMaterialOnSelectCell(FMaterialStruct  OuterMaterial_L, FMaterialStruct  InnerMaterial_L);

	//Mode
	bool GetIsVisibilityOfCellsInRadius();

	void SetIsVisibilityOfCellsInRadius(bool NewSet);

	//Floor
	void ChangeSelectedFloorPosition(bool bIsToUp);

	void SetFloorChangeVectorForSelectedFloorActor(FVector NewVector);

	FVector GetFloorChangeVectorForSelectedFloorActor();

	//Check
	bool IsItSelectedFloorActor(TWeakObjectPtr<AMBPFloorActor> ItFloorActor);

	UPROPERTY()	TWeakObjectPtr<AMBPBuilding> BuildingLinkForCopy;
		
protected:

	//Grid
	UPROPERTY() bool IsVisibilityOfCellsInRadius = false;

	//Selected
	TArray<UBuildingCell*> SelectedCells;

	TArray<USceneComponent*> SelectedBuildingParts;



	UPROPERTY()	TArray<TWeakObjectPtr<AMBPFloorActor>> ListOfChildFloor;

	TWeakObjectPtr<AMBPFloorActor> SelectedChildFloorActor;

	FObjectsForCreating SelectedComponents;

	bool bFastBuildMode = false;

	bool bIgnoreSelect = false;

	UTextRenderComponent* NameOfHome;
};
