// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editor/UnrealEd/Public/Toolkits/BaseToolkit.h"
#include "MBPFloorActor.h"
#include "MBPListOfFurnit.h"
#include "MBPBuilding.h"
#include "UnrealEd.h" 
#include "Editor.h"
#include "MBPComponentsForCreating.h"



class MODULARBUILDINGPLUGIN_API FModularBuildingPluginEdModeToolkit : public FModeToolkit
{
public:

	FModularBuildingPluginEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

	void StartFunction();


	//AboutFloor. List Of Floor
	FReply SpawnNewBuilding();

	FReply SpawnNewChildFloorActor();

	FReply ReloadMenuWidget();

	void SetNewSelectedBuilding(const FAssetData& AssetData);
	FString GetSelectedBuildingPath() const;

	FReply SetNewSelectedFloorActor(TWeakObjectPtr<AMBPFloorActor> NewSet);

	FReply CopySelectedFloor();

	void CheckSomeFloorWasDelete( UObject* SomeObject);

	void OnObjectForCreatingChanged(const FAssetData& AssetData,  int32 Num);

	FString GetObjectForCreatingPath( int32 Num) const;

	//All Check Box in Array
	TArray<TSharedRef<SCheckBox>> CheckBoxes;

	//Grid
	TSharedRef<SWidget>GetGridWidget();

	void SetGridX(int32 GridX_NE);

	void SetGridY(int32 GridY_NE);

	TOptional<int32> GetGridX() const;

	TOptional<int32> GetGridY() const;

	TOptional<float> GetGridScale() const;

	void SetGridScale(float GridScaleThis);

	void SetIsVisibilityOfCellsInRadius(ECheckBoxState newState);

	TOptional<int32> GetSelectGridRad() const;

	void SetSelectGridRad( int32 Loc_SelectGridRad);

	// Change Functions
	FReply MoveBuildingParts(EToMove ToMove);

	TSharedRef<SWidget> Changes();

	FReply TurnWallOnSelectedCells();

	FReply DeleteLastWall();

	FReply DeleteWallsOnSelectedCells();

	FReply DeleteFloorsOnSelectedCells();

	FReply DeleteAllBuildingParts();

	FReply ClearGrid();

	FReply DeleteAll();

	//Modes\Inst\Othet
	TSharedRef<SWidget> GetBuildingModeContent() const;

	bool IsCustomBuildingMode() const;

	void SetBuildingMode(EBuildingMode NewMode);

	FText GetSelectedBuildingModeName() const;

	void SetOneWallMode(ECheckBoxState newState);

	void SetFloorUnderGridCellMode(ECheckBoxState newState);

	FReply DebugButton();

	//Copy\Past
	FReply CopySelectedBuildingParts();

	FReply PasteCopiedBuildingParts();

	FReply CutSelectedBuildingParts();

	FReply TurnCopiedBuildingParts();

	FReply ClearCopiedBuildingParts();

	TSharedRef<SWidget> GetCopyPartContent() const;

	FText GetSelectedCopyPartName() const;

	void SetNewCopyPart(ESelectBuildingParts NewCopyPart);

	//FastBuildMode
	FReply SelectObjectsForFBM( int32 Num);

	//List of Object For Creating
	void CheckSelectedComponentsForCreating();

	FReply CreatingFloor( int32 Num);

	FReply CreatNewSlotToLisftOfObjectsForCreating();

	void AddSlotToLisftOfObjectsForCreating( int32 Num);

	void ReloadListOfObjectForCreating();

	void ReloadListOfObjectComboButton();
		//Widget Functions
	void SetSelectLisftOfObjectsForCreating( int32 SelectListLoc, bool bIsUpdateListForDelete = false);

	int32 GetSelectLisftOfObjectsForCreating() const;

	void SetDeleteOnlyComponentsInList_Mode(ECheckBoxState newState);

	bool GetDeleteOnlyComponentsInList_Mode();

	FReply ButtonTurnOfFastBuildMode();

	void SetAddToObjectsYawRotation( int32 Rotation,  int32 Num);

	TOptional<int32> GetAddToObjectsYawRotation( int32 Num) const;

	void SetOffsetForCreatingObject(float Offset, int32 Num);

	TOptional<float> GetOffsetForCreatingObject(int32 Num) const;

	//Combo Button
	TSharedRef<SWidget> GetListOfObjectsContent() const;

	//Slot Change
	TSharedRef<SWidget> SlotChangeOfListOfObjectsForCreatingWighet();

	TSharedRef<SWidget> VectorWidgetPart(EVectorPart Selectpart);

	void SetChangesVectorsPart(float NewPart, EAxis::Type Axis);

	TOptional<float> GetChangesVectorsPart(EAxis::Type Axis) const;

	FReply CreateNewComponentsForCreating();

	FReply ChangeSlotPosition(bool ToUp);

	FReply DeleteSelectedSlot();

	FReply SetSelectedComponentForCreatingIndex(int32 NewIndex);

	//List Change
	TSharedRef<SWidget> ListChangeOfListsOfObjectsForCreatingWighet();

	FReply CreateNewList();

	FReply DeleteList();
		//Widget Functions
	void SetNameOfListOfObjects(const FText& NewName, ETextCommit::Type CommitInfo);

	FText GetNameOfListOfObjects() const;

	FString GetSelectedComponentsForCreatingPath() const;

	void SetNewComponentsForCreating(const FAssetData& AssetData);

	FText GetNameOfNewList() const;

	void SetNameOfNewList(const FText& NewName, ETextCommit::Type CommitInfo);
	
	//Material
	TSharedRef<SWidget> MaterialChangeOfListOfObjectsForCreatingWighet();
		//Widget Functions
	void SetWallMaterial(const FAssetData& AssetData, int32 it);

	FString GetWallMaterialPath(int32 it) const;

	void SetMaterialSlotName(const FText& NewName, ETextCommit::Type CommitInfo, int32 Num);

	FText GetMaterialSlotName(int32 Num) const;

	FReply ButtonSetNewMaterialToWall();
		//~~~
	TArray<TWeakObjectPtr<UMaterialInterface>> MaterialAssets;

	TArray<FString> MaterialSlotNames;

	//Floor
	FReply SelectNone();

	FReply ChangeFloorPosition(bool bIsToUp);
		//Widget Functions	
	void SetFloorChangeVector(float NewVal, EAxis::Type Axis);

	TOptional<float> GetFloorChangeVector(EAxis::Type Axis) const;

	//Addit
	bool IsSelectedNeedsActorsValid() const;

	FReply RunAllToInstMesh();

	FReply RunAllToStaticMesh();

	void SetIsAllToInstAuto(ECheckBoxState newState);
	//***



private:
	// It's Need To Widget and other. In This Arrey save All ModularBuildingPlugin floors
	TArray<TWeakObjectPtr<AMBPFloorActor>> ListOfChildFloor;

	TSharedRef<SVerticalBox> ListOfChildFloorWidget = SNew(SVerticalBox);

	TWeakObjectPtr<AMBPBuilding> SelectedBuilding ;

	//List of Objects For Creating
	TWeakObjectPtr<AMBPComponentsForCreating> SelectedComponentsForCreating;

	int32 SelectedComponentForCreatingIndex = 0;

	TSharedRef<SVerticalBox> LisftOfObjectsForCreatingWidget = SNew(SVerticalBox);

	TArray<TSharedRef<SHorizontalBox>> ButtonsForCreatingWidget ;

	FText NewListOfObjectsForCreatingName = FText::FromString("ListOfObjects");

	TSharedRef<SHorizontalBox> ListOfObjectComboButton = SNew(SHorizontalBox);

	//What's in the room ?
	TSharedRef<SVerticalBox> ListOfFurn = SNew(SVerticalBox);

	TSharedPtr<SWidget> ToolkitWidget;


	
};
