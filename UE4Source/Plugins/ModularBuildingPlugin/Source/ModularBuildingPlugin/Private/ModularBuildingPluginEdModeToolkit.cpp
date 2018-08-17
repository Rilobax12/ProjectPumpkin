// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ModularBuildingPluginPrivatePCH.h"
#include "ModularBuildingPluginEdMode.h"
#include "ModularBuildingPluginEdModeToolkit.h"
#include "MBPFloorActor.h"
#include "MBPListOfFurnit.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"
#include "Toolkits/ToolkitManager.h"
#include "EngineUtils.h"
#include "../PropertyEditor/Public/DetailLayoutBuilder.h"
#include "../PropertyEditor/Public/IDetailPropertyRow.h"
#include "../PropertyEditor/Public/DetailWidgetRow.h"
#include "../PropertyEditor/Public/IDetailGroup.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboButton.h"
#include "STextPropertyEditableTextBox.h"
#include "../PropertyEditor/Public/PropertyEditorModule.h"
#include "../PropertyEditor/Public/PropertyCustomizationHelpers.h"
#include "SExpandableArea.h"
#include "SNumericEntryBox.h"
#include "SVectorInputBox.h"
#include "SScaleBox.h"
#include "../DetailCustomizations/Private/AnimationAssetDetails.h"
#include "Runtime/Engine/Classes/Animation/PoseAsset.h"
#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Algo/Reverse.h"
#include "Editor/UnrealEd/Public/Editor.h"


#define LOCTEXT_NAMESPACE "FModularBuildingPluginEdModeToolkit"
#define NSLOCTEXT_NAMESPACE "Your Namespace3"

const FLinearColor BorderColor = FLinearColor(0.6f, 0.6f, 0.6f);//FLinearColor(1, 1, 1);

FMargin StandardPadding(6.f, 3.f);
FMargin StandardLeftPadding(6.f, 3.f, 3.f, 3.f);
FMargin StandardRightPadding(3.f, 3.f, 6.f, 3.f);

FSlateFontInfo FontInfo = IDetailLayoutBuilder::GetDetailFont();

FModularBuildingPluginEdModeToolkit::FModularBuildingPluginEdModeToolkit()
{
	ReloadMenuWidget();

	MaterialAssets.Init(NULL , 2);
	//MaterialIndex.Init(0, 2);
	//MaterialIndex[1] = 1;
	MaterialSlotNames.Init(FString("Outside"), 2);
	MaterialSlotNames[1] = FString("Inside");

	CheckBoxes.Init(SNew(SCheckBox), 6);

#if WITH_EDITOR
//	GEditor->FinishDestroy.AddUObject(this, &FModularBuildingPluginEdModeToolkit::CheckSomeFloorWasDelete);
#endif
}

//Menu Widget 
void FModularBuildingPluginEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	CheckSelectedComponentsForCreating();

	//------------------------------------------------------------------------------------------------------------------

	SAssignNew(ToolkitWidget, SBorder)
		.Padding(10)
		//	.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
		
		SNew(SScrollBox)
		.Orientation(Orient_Vertical)
		.ScrollBarAlwaysVisible(false)
		+ SScrollBox::Slot()
		[

		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[

			SNew(SExpandableArea)
			.InitiallyCollapsed(false)
			.Padding(StandardPadding)
			.BorderBackgroundColor(BorderColor)
			.BodyBorderBackgroundColor(BorderColor)
			.HeaderContent()
			[
					SNew(SHeader)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("MyWidget", "GridHeader", "Grid"))
					]
			]
			.BodyContent()
			[		
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryBottom"))
				[
					GetGridWidget()
				]
			]
		]

//---------------------------------List Of Object For Creating---------------------------------------------------------------
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SExpandableArea)
				.InitiallyCollapsed(false)
				.Padding(StandardPadding)
				.BorderBackgroundColor(BorderColor)
				.BodyBorderBackgroundColor(BorderColor)
				.HeaderContent()
				[
					SNew(SHeader)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("MyWidget", "SectionHeader", "List Of Meshes"))
					]
				]
				.BodyContent()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.Padding(StandardPadding)
					.AutoHeight()
					[
						SNew(SHorizontalBox)

						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("MyWidget", "List of Mesh", "Select Set Of Meshes"))
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(10.f,0.f,0.f,0.f)
						[
							ListOfObjectComboButton
							
						]
					]

					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.Padding(StandardPadding)
					.AutoHeight()
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.f,0.f,15.f,0.f)
						[
							CheckBoxes[0] = SNew(SCheckBox)
							.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
							.ToolTipText(FText::FromString("If CHECK - Will deleting only meshes what containes in selected list of mesh"))
							.IsChecked( ECheckBoxState::Unchecked)
							.OnCheckStateChanged(this, &FModularBuildingPluginEdModeToolkit::SetDeleteOnlyComponentsInList_Mode)
						]

						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
							.Text(FText::FromString("Add Slot"))
							.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CreatNewSlotToLisftOfObjectsForCreating)
						]
					]
//List Of Object
					+ SVerticalBox::Slot()
					.Padding(StandardPadding)
					.AutoHeight()
					[
					//	SNew(SBorder)
					//	.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder")) 
					//	[
							LisftOfObjectsForCreatingWidget
					//	]
					]
// FBM OFF
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.AutoHeight()
					.MaxHeight(20)
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(LOCTEXT("Disable Fast Build Mode", "FBM -> OFF"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ButtonTurnOfFastBuildMode)
					]
// Slot Change
					+ SVerticalBox::Slot()
					.AutoHeight()
					[

						SNew(SExpandableArea)
						.InitiallyCollapsed(true)
						.Padding(StandardPadding)
						.HeaderContent()
						[
							SNew(SHeader)
							[
								SNew(STextBlock)
								.Text(NSLOCTEXT("SlotSettings", "Slot Settings", "Slot Change"))	
							]
						]
						.BodyContent()
						[
								SlotChangeOfListOfObjectsForCreatingWighet()					
						]
						
					]
//List Change
					+ SVerticalBox::Slot()
					.AutoHeight()
					[

						SNew(SExpandableArea)
						.InitiallyCollapsed(true)
						.Padding(StandardPadding)
						.HeaderContent()
						[
							SNew(SHeader)
							[
								SNew(STextBlock)
								.Text(NSLOCTEXT("ListSettings", "List Settings", "List Change"))
							]
						]
						.BodyContent()
						[
							ListChangeOfListsOfObjectsForCreatingWighet()
						]

					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[

						SNew(SExpandableArea)
						.InitiallyCollapsed(true)
						.Padding(StandardPadding)
						.HeaderContent()
						[
							SNew(SHeader)
							[
								SNew(STextBlock)
								.Text(NSLOCTEXT("MaterialSettings", "Material Settings", "Material"))
							]
						]
						.BodyContent()
						[
							MaterialChangeOfListOfObjectsForCreatingWighet()
						]

					]

				]

			]

//---------------------------Changes--------------------------------------------
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SExpandableArea)
					.BorderBackgroundColor(BorderColor)
					.BodyBorderBackgroundColor(BorderColor)
					.InitiallyCollapsed(false)
					.Padding(StandardPadding)
					.HeaderContent()
					[
						SNew(SHeader)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Changes2", "Changes1", "Changes"))
						]
					]
					.BodyContent()
					[
						FModularBuildingPluginEdModeToolkit::Changes()
					]
				]


// ----------------------------   Floores -----------------

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SExpandableArea)
				.BorderBackgroundColor(BorderColor)
				.BodyBorderBackgroundColor(BorderColor)
				.InitiallyCollapsed(false)
				.Padding(StandardPadding)
				.HeaderContent()
				[
					SNew(SHeader)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Floors Section2", "Floors Section1", "Floors Section"))
					]
				]
				.BodyContent()
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.Padding(6.f, 3.f)
					.AutoHeight()
					[

						SNew(SExpandableArea)
						.InitiallyCollapsed(true)
						.Padding(StandardPadding)
						.HeaderContent()
						[
							SNew(SHeader)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Floor's changes"))
							]
						]
						.BodyContent()
						[
							SNew(SVerticalBox)

					
							+ SVerticalBox::Slot()
							.Padding(StandardPadding)
							.AutoHeight()
							[
								SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.Padding(StandardLeftPadding)
								.FillWidth(0.15f)
								[
									SNew(STextBlock)
									.Text(FText::FromString("Change Floor Actor Location:"))
								]


								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SBox)
									.WidthOverride(160.f)
									[
										SNew(SVectorInputBox)
										.bColorAxisLabels(true)
										.AllowResponsiveLayout(true)
										.OnXChanged(this, &FModularBuildingPluginEdModeToolkit::SetFloorChangeVector, EAxis::X)
										.OnYChanged(this, &FModularBuildingPluginEdModeToolkit::SetFloorChangeVector, EAxis::Y)
										.OnZChanged(this, &FModularBuildingPluginEdModeToolkit::SetFloorChangeVector, EAxis::Z)
										.X(this, &FModularBuildingPluginEdModeToolkit::GetFloorChangeVector, EAxis::X)
										.Y(this, &FModularBuildingPluginEdModeToolkit::GetFloorChangeVector, EAxis::Y)
										.Z(this, &FModularBuildingPluginEdModeToolkit::GetFloorChangeVector, EAxis::Z)
										.Font(IDetailLayoutBuilder::GetDetailFont())
									]

								]
							]
					
							+SVerticalBox::Slot()
							.Padding(StandardPadding)
							.AutoHeight()
							[
								SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.Padding(StandardLeftPadding)
								.FillWidth(0.15f)
								[
									SNew(STextBlock)
									.Text(FText::FromString("Change Floor Actor Postion to:"))
								]


								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SBox)
									.WidthOverride(30.f)
									[
										SNew(SButton)
										.Text(FText::FromString("Up"))
										.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ChangeFloorPosition, true)
									]
								]

								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SBox)
									.WidthOverride(45.f)
									[
										SNew(SButton)
										.Text(FText::FromString("Down"))
										.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ChangeFloorPosition, false)
									]
								]
							]
						]
					]

					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.Padding(StandardPadding)
					.AutoHeight()
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Center)
						.AutoWidth()
						[
							SNew(SButton)
							.Text(LOCTEXT("Creat new floor", "NewFloor"))
							.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SpawnNewChildFloorActor)
						]

						+ SHorizontalBox::Slot()
						.Padding(15.f, 0, 0, 0)
						.HAlign(HAlign_Right)
						.AutoWidth()
						[
							SNew(SButton)
							.Text(LOCTEXT("Copy Floor", "CopyFloor"))
							.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CopySelectedFloor)
						]


					]

					
// List Of Floors Actors
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHeader)
						.ToolTipText(LOCTEXT("List Of Floors", "List Of Floors Actors"))
						[
							SNew(STextBlock)
							.Text(LOCTEXT("List Of Floors1", "List Of Floors Actors:"))
						]
						
						
					]

					+ SVerticalBox::Slot()
					.Padding(StandardPadding)
					.AutoHeight()
					[
						ListOfChildFloorWidget	
					]
					
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]

//Select Building Part~~~~~~~~~~~

			+ SVerticalBox::Slot()
			//.HAlign(HAlign_Left)
			.Padding(StandardPadding)
			.AutoHeight()
			[
				
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(0.f, 3.f, 0.f, 0.f)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("SelectBuilding", "Select Building", "Select Building:"))

					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.f, 0.f)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.ForegroundColor(FLinearColor(22.f / 255.f, 160.f / 255.f, 133.f / 255.f))
						[
							SNew(SObjectPropertyEntryBox)
							.AllowedClass(AMBPBuilding::StaticClass())
							.ObjectPath(this, &FModularBuildingPluginEdModeToolkit::GetSelectedBuildingPath)
							.OnObjectChanged(this, &FModularBuildingPluginEdModeToolkit::SetNewSelectedBuilding)
							.AllowClear(true)
						]
					]

					+ SHorizontalBox::Slot()
					.FillWidth(0.1)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(LOCTEXT("CreatNewBuilding", "Creat New Building"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SpawnNewBuilding)
					]
				
			]

			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SSeparator)
				]
			]
		
	];

	FModeToolkit::Init(InitToolkitHost);

}

void FModularBuildingPluginEdModeToolkit::StartFunction()
{
		ButtonTurnOfFastBuildMode();
		SetDeleteOnlyComponentsInList_Mode(GetDeleteOnlyComponentsInList_Mode() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

		if (SelectedBuilding.IsValid() ? SelectedBuilding->GetSelectedChildFloorActor().IsValid() : false)
		{
			SetBuildingMode(SelectedBuilding->GetSelectedChildFloorActor()->GetSelectedBuildingMode());
		}
}

//About Building. List Of Building.
FReply FModularBuildingPluginEdModeToolkit::SpawnNewBuilding()
{
		UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
		FVector const ObjLocation = FVector::ZeroVector;

		AMBPBuilding* SpawningActor = (AMBPBuilding*)MyWorld->SpawnActor(AMBPBuilding::StaticClass(), &ObjLocation);
		GEditor->MoveActorInFrontOfCamera(*SpawningActor, GCurrentLevelEditingViewportClient->GetViewLocation(), GCurrentLevelEditingViewportClient->GetViewRotation().Vector() / 2);
		GEditor->MoveViewportCamerasToActor(*SpawningActor, true);

		SetNewSelectedBuilding(SpawningActor); 
		return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::SpawnNewChildFloorActor()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->SpawnNewChildFloorActor();
		ReloadMenuWidget();
		SetNewSelectedFloorActor(SelectedBuilding->GetSelectedChildFloorActor());
	}
	return FReply::Handled();	
}

FReply FModularBuildingPluginEdModeToolkit::ReloadMenuWidget()
{
	// Delete All Old List Of Floor
	ListOfChildFloor.Empty();
	ListOfChildFloorWidget->ClearChildren();
	
	if(SelectedBuilding.IsValid())
	ListOfChildFloor = SelectedBuilding->GetChildFloorsActors();

//--------- Save all object into  the level  ------------------

	for (int32 it = ListOfChildFloor.Num() -1 ; it >= 0; it--)
	{
		if (ListOfChildFloor[it].IsValid())
		{
			FLinearColor SelectColor = FLinearColor::White;//FLinearColor::Black;

			if (SelectedBuilding->IsItSelectedFloorActor(ListOfChildFloor[it]))
				SelectColor = FLinearColor(22.f / 255.f, 160.f / 255.f, 133.f / 255.f);	//.f / 255.f

			ListOfChildFloorWidget->AddSlot()
			[
				SNew(SButton)
				.ButtonColorAndOpacity(SelectColor)
				//.ForegroundColor(SelectColor)
				.Text(FText::FromString(*ListOfChildFloor[it]->GetName()))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SetNewSelectedFloorActor, ListOfChildFloor[it])
			];
		}
	}

	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		AMBPFloorActor* CastToIt = SelectedBuilding->GetSelectedChildFloorActor().Get();
		CheckBoxes[0]->SetIsChecked(GetDeleteOnlyComponentsInList_Mode() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		CheckBoxes[1]->SetIsChecked(SelectedBuilding->GetIsVisibilityOfCellsInRadius() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		CheckBoxes[4]->SetIsChecked(SelectedBuilding->GetSelectedChildFloorActor()->GetOneWallMode() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		CheckBoxes[5]->SetIsChecked(SelectedBuilding->GetSelectedChildFloorActor()->GetFloorUnderGridCellMode() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}

	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::SetNewSelectedFloorActor(TWeakObjectPtr<AMBPFloorActor> NewSet)
{
	ButtonTurnOfFastBuildMode();
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->SelectChildFloorActor(NewSet);
	}
	StartFunction();
	ReloadMenuWidget();
	return FReply::Handled();
}

void FModularBuildingPluginEdModeToolkit::SetNewSelectedBuilding(const FAssetData& AssetData)
{
	TWeakObjectPtr<AMBPBuilding> NewSet = Cast<AMBPBuilding>(AssetData.GetAsset());
	
	if (SelectedBuilding != NewSet)
	{
		if (SelectedBuilding.IsValid())
		{
			SelectedBuilding->SetVisionForAllFloorsActors(ESelectBuildingParts::WallAndFloor, true);
			SelectedBuilding->SetVisionForAllFloorsActors(ESelectBuildingParts::Grid, false);
		}
		if (NewSet.IsValid())
		{
			SelectedBuilding = NewSet;

			ReloadMenuWidget();
			GEditor->SelectNone(true, true);
			GEditor->SelectActor(NewSet.Get(), true, true);
			GEditor->MoveViewportCamerasToActor(*NewSet, true);
		}
	}
	ReloadMenuWidget();

}

FString FModularBuildingPluginEdModeToolkit::GetSelectedBuildingPath() const
{
	return SelectedBuilding.IsValid() ? SelectedBuilding->GetPathName() : FString("");
}

void FModularBuildingPluginEdModeToolkit::CheckSomeFloorWasDelete(UObject* SomeObject)
{
	UE_LOG(LogTemp, Log, TEXT("Something delete"));
	if (Cast<AMBPFloorActor>(SomeObject))
		ReloadMenuWidget();

	if (SomeObject == SelectedBuilding)
		SelectNone();
}

//Grid
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::GetGridWidget()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.Padding(6.f, 3.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(15.f, 0.f)
			.HAlign(HAlign_Right)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("Reload Actor", "Reload"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ReloadMenuWidget)
			]

			+ SHorizontalBox::Slot()
			//.HAlign(HAlign_Center)
			.AutoWidth()
			[
				SNew(SButton)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("ClearGrid", "ClearGrid"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ClearGrid)
			]

		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15)
			.MaxWidth(200.f)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				//.WrapTextAt(250.f)
				.Text(LOCTEXT("HelperLabel", "GridXY"))
			]


			+ SHorizontalBox::Slot()
			.Padding(0, 0, 6.f, 0)
			.FillWidth(0.35)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(StandardRightPadding)
				.FillWidth(0.2)
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<int32>)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.AllowSpin(true)
					.MinValue(0)
					//.MaxValue(500)
					.MaxSliderValue(1000)
					.SliderExponent(1)
					.Value(this, &FModularBuildingPluginEdModeToolkit::GetGridX)
					.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetGridX)
				]
	
				+ SHorizontalBox::Slot()
				.Padding(StandardRightPadding)
				.FillWidth(0.005)

				+ SHorizontalBox::Slot()
				.Padding(StandardRightPadding)
				.FillWidth(0.2)
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<int32>)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.AllowSpin(true)
					.MinValue(0)
					.MaxSliderValue(1000)
					.SliderExponent(1)
					.Value(this, &FModularBuildingPluginEdModeToolkit::GetGridY)
					.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetGridY)
				]
			]
		]

		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[
			SNew(SExpandableArea)
			.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
			.InitiallyCollapsed(true)
			.Padding(8.0f)
			.HeaderContent()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Advanced options", "GridHeaderOptions", "Advanced Options"))
			]
			.BodyContent()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.f, 3.f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(StandardLeftPadding)
					.FillWidth(0.15)
					.MaxWidth(200.f)
					[
						SNew(STextBlock)
						.AutoWrapText(true)
						//.WrapTextAt(250.f)
						.Text(LOCTEXT("Grid Scale", "GridScale"))
					]


				+ SHorizontalBox::Slot()
				.Padding(30.f, 0.f, 5.f, 0.f)
				.FillWidth(0.15)
				[

					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.MinValue(0.f)
					.MaxValue(500.f)
					//.MaxSliderValue(2000)
					//.SliderExponent(1)
					.Value(this, &FModularBuildingPluginEdModeToolkit::GetGridScale)
					.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetGridScale)
				]
			]

			+ SVerticalBox::Slot()
			.Padding(6.f, 3.f)
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.FillWidth(0.16)
				.MaxWidth(200.f)
				[
					CheckBoxes[1] = SNew(SCheckBox)
					.ToolTipText(LOCTEXT("To create a new object, even if this place was a different", " If ON - Visibility Of Cells within Radius only "))
					.IsChecked(ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &FModularBuildingPluginEdModeToolkit::SetIsVisibilityOfCellsInRadius)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(30.f, 0, 8.f, 0)
				.FillWidth(0.15)
				[
					SNew(SNumericEntryBox<int>)
					.AllowSpin(true)
					.MinValue(0)
					.MaxValue(40)
					//.MaxSliderValue(2000)
					//.SliderExponent(1)
					.Value(this, &FModularBuildingPluginEdModeToolkit::GetSelectGridRad)
					.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetSelectGridRad)
				]
			]
		]
	]
	;
}
	//Function For Widget 
void FModularBuildingPluginEdModeToolkit::SetGridX( int32 GridX_NE)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{	
		SelectedBuilding->GetSelectedChildFloorActor()->GridX = GridX_NE;
		SelectedBuilding->GetSelectedChildFloorActor()->SpawnGrid();
	}
}

TOptional<int32> FModularBuildingPluginEdModeToolkit::GetGridX() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		return SelectedBuilding->GetSelectedChildFloorActor()->GridX;
	}
	return 0;
}

void FModularBuildingPluginEdModeToolkit::SetGridY( int32 GridY_NE)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->GridY = GridY_NE;
		SelectedBuilding->GetSelectedChildFloorActor()->SpawnGrid();
		//AMBPFloorActor* CastToPar = Cast<AMBPFloorActor>(SelectParentFloor.Get());
		//SelectedBuilding->GetSelectedChildFloorActor()->FloorMeshHeight = CastToPar->FloorMeshHeight ;
	}
}

TOptional<int32> FModularBuildingPluginEdModeToolkit::GetGridY() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		
		return SelectedBuilding->GetSelectedChildFloorActor()->GridY;
	}
	return 0;
}

TOptional<float> FModularBuildingPluginEdModeToolkit::GetGridScale() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		return SelectedBuilding->GetSelectedChildFloorActor()->GridScale;
	}
	return 0;
}

void FModularBuildingPluginEdModeToolkit::SetGridScale(float GridScale)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		float ChangeScale = SelectedBuilding->GetSelectedChildFloorActor()->GridScale / GridScale;

		SelectedBuilding->GetSelectedChildFloorActor()->GridScale = GridScale;
		
		int32 X = SelectedBuilding->GetSelectedChildFloorActor()->GridX;
		int32 Y = SelectedBuilding->GetSelectedChildFloorActor()->GridY;
		//SelectedBuilding->GetSelectedChildFloorActor()->ClearGrid();
		SelectedBuilding->GetSelectedChildFloorActor()->GridX = (int)((float)X * ChangeScale);
		SelectedBuilding->GetSelectedChildFloorActor()->GridY = (int)((float)Y * ChangeScale);
		SelectedBuilding->GetSelectedChildFloorActor()->SpawnGrid();

		//ClearCopiedBuildingParts();
	}
}

void FModularBuildingPluginEdModeToolkit::SetIsVisibilityOfCellsInRadius(ECheckBoxState newState)
{
	if (SelectedBuilding.IsValid())
		SelectedBuilding->SetIsVisibilityOfCellsInRadius(newState == ECheckBoxState::Checked);
}

TOptional<int32> FModularBuildingPluginEdModeToolkit::GetSelectGridRad() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		
		return SelectedBuilding->GetSelectedChildFloorActor()->SelectGridRad;
	}
	return 0;
}

void FModularBuildingPluginEdModeToolkit::SetSelectGridRad( int32 Loc_SelectGridRad)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		
		 SelectedBuilding->GetSelectedChildFloorActor()->SelectGridRad = Loc_SelectGridRad;
	}
}	

//Creating
FReply FModularBuildingPluginEdModeToolkit::CreatingFloor(int32 Num)
{
	CheckSelectedComponentsForCreating();
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid() && SelectedComponentsForCreating->GetObjectByIndex(Num).IsValid())
	{
		if (SelectedComponentsForCreating->GetObjectByIndex(Num)->GetName().Contains(FString("Floor")) ||
			SelectedComponentsForCreating->GetObjectByIndex(Num)->GetName().Contains(FString("Ceiling")))
		{
			SelectedComponentForCreatingIndex = Num;
			SelectedBuilding->GetSelectedChildFloorActor()->RotationForNewBuildingFloors = SelectedComponentsForCreating->GetChangeRotationByIndex(Num);
			SelectedBuilding->CreateFloorsByObject(Num, SelectedComponentsForCreating);
			ReloadListOfObjectForCreating();
		}
		SelectedBuilding->SetFastBuildMode(false);
	}

	return FReply::Handled();
}

//FastBuildFun
FReply FModularBuildingPluginEdModeToolkit::SelectObjectsForFBM(int32 Num)
{
	CheckSelectedComponentsForCreating();
	if (SelectedComponentsForCreating->GetObjectByIndex(Num).IsValid() && SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->SetFastBuildMode(true);
		SelectedComponentForCreatingIndex = Num;

		SelectedBuilding->SetSelectedComponentsForFBM(Num, SelectedComponentsForCreating);

		ReloadListOfObjectForCreating();
	}
	return FReply::Handled();
}

//List of Object For Creating
FReply FModularBuildingPluginEdModeToolkit::CreatNewSlotToLisftOfObjectsForCreating()
{
	SelectedComponentsForCreating->AddNewSlot();

	AddSlotToLisftOfObjectsForCreating(SelectedComponentsForCreating->GetNumOfSlotsInSelectedListOfObjects() - 1);

	return FReply::Handled();
}

void FModularBuildingPluginEdModeToolkit::AddSlotToLisftOfObjectsForCreating(int32 Num)
{
	if (!ButtonsForCreatingWidget.IsValidIndex(Num))
		ButtonsForCreatingWidget.Add(SNew(SHorizontalBox));

	FLinearColor SelectColor = FLinearColor::White;
	if (SelectedComponentForCreatingIndex == Num)

		SelectColor = FLinearColor::Green;

		LisftOfObjectsForCreatingWidget->AddSlot()
		.Padding(1)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.ColorAndOpacity(FLinearColor::White)
			.BorderBackgroundColor(SelectColor)
			.Content()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[

					ButtonsForCreatingWidget[Num]

				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UObject::StaticClass())
					.ObjectPath(this, &FModularBuildingPluginEdModeToolkit::GetObjectForCreatingPath, Num)
					.OnObjectChanged(this, &FModularBuildingPluginEdModeToolkit::OnObjectForCreatingChanged, Num)
					.AllowClear(true)
				]
			]
		];

	if (SelectedComponentsForCreating->GetObjectByIndex(Num).IsValid())
	{
		if (SelectedComponentsForCreating->GetObjectByIndex(Num)->GetName().Contains(FString("Floor")) || 
			SelectedComponentsForCreating->GetObjectByIndex(Num)->GetName().Contains(FString("Ceiling")))
		{

			ButtonsForCreatingWidget[Num]->ClearChildren();

			ButtonsForCreatingWidget[Num]->AddSlot()
				.Padding(0.f, 0.f)
				.AutoWidth()
				[
					
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.ColorAndOpacity(FLinearColor::White)
						.BorderBackgroundColor(FLinearColor::Blue * 0.2f)
						.Content()
						[
							SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							.Padding(StandardRightPadding)
							.AutoWidth()
							[
								SNew(STextBlock)
								//.Text(NSLOCTEXT("Additional", "0", "X"))
								.Text(FText::FromString("Yaw"))
							]

							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SBox)
								.WidthOverride(80)
								.HeightOverride(20)
								[
									SNew(SNumericEntryBox<int>)
									.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
									.AllowSpin(true)
									.MinValue(-360)
									.MaxValue(360)
									//.SliderExponent(10)
									.Value(this, &FModularBuildingPluginEdModeToolkit::GetAddToObjectsYawRotation, Num)
									.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetAddToObjectsYawRotation, Num)
								]
							]
						]
					
				];

				ButtonsForCreatingWidget[Num]->AddSlot()
				.AutoWidth()
				.Padding(7.f, 0.f)
				[
					SNew(SButton)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(LOCTEXT("CreatItFloor", "Create"))
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CreatingFloor, Num)
				];

		}
		else
		{
			ButtonsForCreatingWidget[Num]->ClearChildren();

			ButtonsForCreatingWidget[Num]->AddSlot()
				.AutoWidth()
				[
					SNew(SButton)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(LOCTEXT("Select This Mesh For FastBuildMode. And included FBM ", "Select(FBM)"))
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SelectObjectsForFBM, Num)
				];

			ButtonsForCreatingWidget[Num]->AddSlot()
				.Padding(7.f, 0.f)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(60)
					.HeightOverride(20)
					[
						SNew(SNumericEntryBox<float>)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.AllowSpin(true)
						.MinValue(-2000.f)
						.MaxValue(2000.f)
						.MinSliderValue(-500.f)
						.MaxSliderValue(500.f)
						//.SliderExponent(10)
						.Value(this, &FModularBuildingPluginEdModeToolkit::GetOffsetForCreatingObject, Num)
						.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetOffsetForCreatingObject, Num)
					]
				];
		}
	}
	else
	{
		ButtonsForCreatingWidget[Num]->ClearChildren();

		ButtonsForCreatingWidget[Num]->AddSlot()
			.Padding(0.f, 0.f, 5.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("SeletSlot", "Selet Slot"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SetSelectedComponentForCreatingIndex, Num)
			];
	}

}

void FModularBuildingPluginEdModeToolkit::CheckSelectedComponentsForCreating()
{
	if (SelectedComponentsForCreating.IsValid() == false)
	{
		for (TActorIterator<AMBPComponentsForCreating> ActorItr(GEditor->GetEditorWorldContext().World()); ActorItr; ++ActorItr)
		{
			TWeakObjectPtr<AMBPComponentsForCreating> CheckComponentsForCreating = *ActorItr;

			if (CheckComponentsForCreating.IsValid())
			{
				SelectedComponentsForCreating = CheckComponentsForCreating;
				break;
			}
		}

		if (SelectedComponentsForCreating.IsValid() == false)
		{
			CreateNewComponentsForCreating();
		}
		else
			ReloadListOfObjectForCreating();
	}
}

void FModularBuildingPluginEdModeToolkit::ReloadListOfObjectForCreating()
{
	if (SelectedComponentsForCreating.IsValid())
	{
		SetSelectLisftOfObjectsForCreating(SelectedComponentsForCreating->SelectedList);
		ReloadListOfObjectComboButton();
	}
	else
		SetSelectLisftOfObjectsForCreating(0);
}

void FModularBuildingPluginEdModeToolkit::ReloadListOfObjectComboButton()
{
	ListOfObjectComboButton->ClearChildren();
	ListOfObjectComboButton->AddSlot()
		.AutoWidth()
		[
			//SNew(SBox)
			//.WidthOverride(110)
			//.HeightOverride(20)
			//[
				SNew(SComboButton)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &FModularBuildingPluginEdModeToolkit::GetNameOfListOfObjects)
				]
				.ContentPadding(FMargin(6.0f, 2.0f))
				.MenuContent()
				[
					GetListOfObjectsContent()
				]
			//]
		];

	//		//	SNew(SNumericEntryBox<int>)
	//		//	.AllowSpin(true)
	//		//	.MinValue(0)
	//		//	.MaxValue(19)
	//		//	.SliderExponent(1)
	//		//	.MaxSliderValue(9)
	//		//	.Value(this, &FModularBuildingPluginEdModeToolkit::GetSelectLisftOfObjectsForCreating)
	//		//	.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetSelectLisftOfObjectsForCreating)

}
	//Function For Widget 
FString FModularBuildingPluginEdModeToolkit::GetObjectForCreatingPath(int32 Num) const
{
	if(SelectedComponentsForCreating.IsValid())
	return SelectedComponentsForCreating->GetObjectByIndex(Num).IsValid() ? SelectedComponentsForCreating->GetObjectByIndex(Num)->GetPathName() : FString("");
	else return FString("");
}

void FModularBuildingPluginEdModeToolkit::OnObjectForCreatingChanged(const FAssetData& AssetData, int32 Num)
{
	CheckSelectedComponentsForCreating();
	if (SelectedComponentsForCreating->IsValidSlotIndex(Num))
	{
		TWeakObjectPtr<UObject> NewObject = Cast<UObject>(AssetData.GetAsset());
		SelectedComponentsForCreating->SetObjectByIndex(NewObject, Num);

		ReloadListOfObjectForCreating();
	}


}

void FModularBuildingPluginEdModeToolkit::SetSelectLisftOfObjectsForCreating(int32 SelectListLoc, bool bIsUpdateListForDelete)
{
	CheckSelectedComponentsForCreating();

	LisftOfObjectsForCreatingWidget->ClearChildren();

	for (int32 it = 0; it < ButtonsForCreatingWidget.Num(); it++)
	{
		ButtonsForCreatingWidget[it]->ClearChildren();
	}

	ButtonsForCreatingWidget.Empty();

	SelectedComponentsForCreating->SelectedList = SelectListLoc;

	for (int32 it = 0; it < SelectedComponentsForCreating->GetSelectedListOfObjects().GetNumOfComponents(); it++)
	{
		AddSlotToLisftOfObjectsForCreating(it);
	}

	if (SelectedBuilding.IsValid() && bIsUpdateListForDelete)
		SelectedBuilding->SetSelectedComponentsForFBM(-1, SelectedComponentsForCreating);
}

int32 FModularBuildingPluginEdModeToolkit::GetSelectLisftOfObjectsForCreating() const
{
	if (SelectedComponentsForCreating.IsValid())
	return SelectedComponentsForCreating->SelectedList;
	else return 0;
}

void FModularBuildingPluginEdModeToolkit::SetDeleteOnlyComponentsInList_Mode(ECheckBoxState newState)
{
	CheckSelectedComponentsForCreating();
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->SetDeleteOnlyFromListMode(newState == ECheckBoxState::Checked);

		if (newState == ECheckBoxState::Checked)
		{
			if (SelectedBuilding->GetFastBuildMode())
				SelectedBuilding->SetSelectedComponentsForFBM(SelectedComponentForCreatingIndex, SelectedComponentsForCreating);
			else
				SelectedBuilding->SetSelectedComponentsForFBM(-1, SelectedComponentsForCreating);
		}

		if (newState == ECheckBoxState::Unchecked)
			SetBuildingMode(SelectedBuilding->GetSelectedChildFloorActor()->GetSelectedBuildingMode());

		ReloadMenuWidget();
	}
}

bool FModularBuildingPluginEdModeToolkit::GetDeleteOnlyComponentsInList_Mode()
{

	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		return SelectedBuilding->GetSelectedChildFloorActor()->GetDeleteOnlyFromListMode();
	}
	return false;
}

FReply FModularBuildingPluginEdModeToolkit::ButtonTurnOfFastBuildMode()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{	
		SelectedBuilding->SetFastBuildMode(false);
	//	SelectedBuilding->GetSelectedChildFloorActor()->FastBuildModBP = false;
	}
	return FReply::Handled();
}

void FModularBuildingPluginEdModeToolkit::SetAddToObjectsYawRotation(int32 Rotation, int32 Num)
{
	CheckSelectedComponentsForCreating();
	SelectedComponentsForCreating->SetChangeRotationByIndex(FRotator(0.f, Rotation, 0.f), Num);
}

TOptional<int32> FModularBuildingPluginEdModeToolkit::GetAddToObjectsYawRotation(int32 Num) const
{
	if (SelectedComponentsForCreating.IsValid())
	return (int)SelectedComponentsForCreating->GetChangeRotationByIndex(Num).Yaw;
	else return 0;
}

void FModularBuildingPluginEdModeToolkit::SetOffsetForCreatingObject(float Offset, int32 Num)
{
	CheckSelectedComponentsForCreating();
	SelectedComponentsForCreating->SetOffsetByIndex(Offset, Num);
	
}

TOptional<float> FModularBuildingPluginEdModeToolkit::GetOffsetForCreatingObject(int32 Num) const
{
	if (SelectedComponentsForCreating.IsValid())
	return SelectedComponentsForCreating->GetOffsetByIndex(Num);
	else return 0.f;
}
	//List of Object For Creating - Combo Button
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::GetListOfObjectsContent() const
{

	FMenuBuilder MenuBuilder(true, NULL);
	{
		if (SelectedComponentsForCreating.IsValid())
		{
			for (int it0 = 0; it0 < SelectedComponentsForCreating->ListsOfObjects.Num(); it0++)
			{
				FUIAction CreatSome(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetSelectLisftOfObjectsForCreating, it0, true));
				MenuBuilder.AddMenuEntry(SelectedComponentsForCreating->GetNameOfListByIndex(it0), LOCTEXT("AUpIt", "Creat over object with down attrib. Exmpl: Picture"), FSlateIcon(), CreatSome);
			}
		}
	}

	return MenuBuilder.MakeWidget();
}

//Slot Change
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::SlotChangeOfListOfObjectsForCreatingWighet()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Add during creating:"))
			]


			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(160.f)
				[
					SNew(SVectorInputBox)
					.bColorAxisLabels(true)
					.AllowResponsiveLayout(true)
					.OnXChanged(this, &FModularBuildingPluginEdModeToolkit::SetChangesVectorsPart, EAxis::X)
					.OnYChanged(this, &FModularBuildingPluginEdModeToolkit::SetChangesVectorsPart, EAxis::Y)
					.OnZChanged(this, &FModularBuildingPluginEdModeToolkit::SetChangesVectorsPart, EAxis::Z)
					.X(this, &FModularBuildingPluginEdModeToolkit::GetChangesVectorsPart, EAxis::X)
					.Y(this, &FModularBuildingPluginEdModeToolkit::GetChangesVectorsPart, EAxis::Y)
					.Z(this, &FModularBuildingPluginEdModeToolkit::GetChangesVectorsPart, EAxis::Z)
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			]

		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Change Slot Position:"))
			]

			+ SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 10.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ToUp", "Up"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ChangeSlotPosition, true)
			]

			+ SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 10.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ToDown", "Down"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ChangeSlotPosition, false)
			]

			+ SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 10.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("DeleteSlot", "Delete Slot"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteSelectedSlot)
			]
		]
	;
}

TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::VectorWidgetPart(EVectorPart Selectpart)
{
	FLinearColor SelectColor = FLinearColor::Black;
	FString Name = FString("0");

	switch (Selectpart)
	{
	case EVectorPart::X:
		SelectColor = FLinearColor::Red;
		Name = FString("X");
		break;
	case EVectorPart::Y:
		SelectColor = FLinearColor::Green;
		Name = FString("Y");
		break;
	case EVectorPart::Z:
		SelectColor = FLinearColor::Blue;
		Name = FString("Z");
		break;
	default:
		break;
	}

	return SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder") )
		.ColorAndOpacity(FLinearColor::White)
		.BorderBackgroundColor(SelectColor )
		.Content()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardRightPadding)
			.AutoWidth()
			[
				SNew(STextBlock)
				//.Text(NSLOCTEXT("Additional", "0", "X"))
				.Text(FText::FromString(Name))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(50.f)
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.SliderExponent(1.f)
					.MaxSliderValue(500.f)
					.MinSliderValue(-500.f)
				//	.Value(this, &FModularBuildingPluginEdModeToolkit::GetChangesVectorsPart, Selectpart)
				//	.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetChangesVectorsPart, Selectpart)
				]
			]
		]
	;
}

void FModularBuildingPluginEdModeToolkit::SetChangesVectorsPart(float NewPart, EAxis::Type Axis)
{
	if (SelectedComponentsForCreating.IsValid())
	{
		FVector NewVector = SelectedComponentsForCreating->GetChangeVectorByIndex(SelectedComponentForCreatingIndex);
		
		NewVector.SetComponentForAxis(Axis, NewPart);

		SelectedComponentsForCreating->SetChangeVectorByIndex(NewVector, SelectedComponentForCreatingIndex);
	}
}

TOptional<float> FModularBuildingPluginEdModeToolkit::GetChangesVectorsPart(EAxis::Type Axis) const
{
	if (SelectedComponentsForCreating.IsValid())
	{
		return SelectedComponentsForCreating->GetChangeVectorByIndex(SelectedComponentForCreatingIndex).GetComponentForAxis(Axis);
	}
	return 0.f;
}

FReply FModularBuildingPluginEdModeToolkit::CreateNewComponentsForCreating()
{
	UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
	FVector const ObjLocation = FVector::ZeroVector;
	SelectedComponentsForCreating = (AMBPComponentsForCreating*)MyWorld->SpawnActor(AMBPComponentsForCreating::StaticClass(), &ObjLocation);
	ReloadListOfObjectForCreating();
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::ChangeSlotPosition(bool ToUp)
{
	if (SelectedComponentsForCreating.IsValid())
	{
		int32 NewIndex = SelectedComponentsForCreating->ChangeSlotPosition(ToUp, SelectedComponentForCreatingIndex);

		SelectedComponentForCreatingIndex = NewIndex;

		ReloadListOfObjectForCreating();
		ReloadListOfObjectComboButton();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteSelectedSlot()
{
	if (SelectedComponentsForCreating.IsValid())
	{
		SelectedComponentsForCreating->DeleteSlotByIndexInSelectedList(SelectedComponentForCreatingIndex);
		ReloadListOfObjectForCreating();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::SetSelectedComponentForCreatingIndex(int32 NewIndex)
{
	SelectedComponentForCreatingIndex = NewIndex;

	ReloadListOfObjectForCreating();

	return FReply::Handled();
}

//List Change
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::ListChangeOfListsOfObjectsForCreatingWighet()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Set Selected List Name:"))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				//SNew(SBox)
			//	.MaxDesiredWidth
			//	.WidthOverride(80)
			//	.HeightOverride(17)
			//	[
				SNew(SEditableTextBox)
				.Text(this, &FModularBuildingPluginEdModeToolkit::GetNameOfListOfObjects)
				.OnTextCommitted(this, &FModularBuildingPluginEdModeToolkit::SetNameOfListOfObjects)

			]
		]
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("New List Of Object name:"))
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SEditableTextBox)
				.Text(this, &FModularBuildingPluginEdModeToolkit::GetNameOfNewList)
				.OnTextCommitted(this, &FModularBuildingPluginEdModeToolkit::SetNameOfNewList)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.f, 0.f, 10.f, 0.f)
			[
				SNew(SButton)
				.Text(LOCTEXT("DeleteList", "Delete List"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteList)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("CreateNewList", "Create New List"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CreateNewList)
			]
		]
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSeparator)
		]

		+ SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		.AutoHeight()
		[

			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(StandardLeftPadding)
			.FillWidth(0.15f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SelectedComponentsForCreating1", "SelectedComponentsForCreating", "Components For Creating:"))

			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(AMBPComponentsForCreating::StaticClass())
				.ObjectPath(this, &FModularBuildingPluginEdModeToolkit::GetSelectedComponentsForCreatingPath)
				.OnObjectChanged(this, &FModularBuildingPluginEdModeToolkit::SetNewComponentsForCreating)
				.AllowClear(true)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(StandardPadding)
		.AutoHeight()
		[

			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(0.15f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Or you can create another1", "Or you can create another:", "Or you can create another:"))

			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f)
			[

				SNew(SButton)
				.Text(LOCTEXT("Create New Components For Creating ", "Create New"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CreateNewComponentsForCreating)
			]
		]
	;
}

FReply FModularBuildingPluginEdModeToolkit::CreateNewList()
{
	if (SelectedComponentsForCreating.IsValid())
	{
		SelectedComponentsForCreating->AddNewList(1);

		int32 NewListIndex = SelectedComponentsForCreating->GetNumOfListsOfObjects() - 1;

		SelectedComponentsForCreating->SetNameOfListByIndex(NewListOfObjectsForCreatingName, NewListIndex);

		SetSelectLisftOfObjectsForCreating(NewListIndex);

		ReloadListOfObjectComboButton();
	}

	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteList()
{
	if (SelectedComponentsForCreating.IsValid())
	{
		SelectedComponentsForCreating->DeleteListByIndex(GetSelectLisftOfObjectsForCreating());

		SetSelectLisftOfObjectsForCreating(0);

		ReloadListOfObjectComboButton();
	}

	return FReply::Handled();
}
	//Function For Widget 
FString FModularBuildingPluginEdModeToolkit::GetSelectedComponentsForCreatingPath() const
{
	return SelectedComponentsForCreating.IsValid() ? SelectedComponentsForCreating->GetPathName() : FString("");
}

void FModularBuildingPluginEdModeToolkit::SetNewComponentsForCreating(const FAssetData& AssetData)
{
	SelectedComponentsForCreating = Cast<AMBPComponentsForCreating>(AssetData.GetAsset());
	ReloadListOfObjectForCreating();
}

FText FModularBuildingPluginEdModeToolkit::GetNameOfNewList() const
{
	return NewListOfObjectsForCreatingName;
}

void FModularBuildingPluginEdModeToolkit::SetNameOfNewList(const FText& NewName, ETextCommit::Type CommitInfo)
{
	NewListOfObjectsForCreatingName = NewName;
}

FText FModularBuildingPluginEdModeToolkit::GetNameOfListOfObjects() const
{
	if (SelectedComponentsForCreating.IsValid())
		return SelectedComponentsForCreating->GetNameOfListByIndex(GetSelectLisftOfObjectsForCreating());
	else return FText::FromString("NULL");
}

void FModularBuildingPluginEdModeToolkit::SetNameOfListOfObjects(const FText& NewName, ETextCommit::Type CommitInfo)
{
	if (SelectedComponentsForCreating.IsValid())
		SelectedComponentsForCreating->SetNameOfListByIndex(NewName, GetSelectLisftOfObjectsForCreating());

	ReloadListOfObjectComboButton();
}

//Material Change
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::MaterialChangeOfListOfObjectsForCreatingWighet()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.Padding(StandardPadding)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Change Material2", "Outer side", "Outer side | Index: "))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				/*SNew(SNumericEntryBox<int32>)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.AllowSpin(true)
				.MinValue(0)
				.MaxValue(100)
				.MaxSliderValue(10)
				.SliderExponent(1)
				.Value(this, &FModularBuildingPluginEdModeToolkit::GetMaterialSlotName, 0)
				.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetMaterialIndex, 0)*/

				SNew(SEditableTextBox)
				.Text(this, &FModularBuildingPluginEdModeToolkit::GetMaterialSlotName, 0)
				.OnTextCommitted(this, &FModularBuildingPluginEdModeToolkit::SetMaterialSlotName, 0)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f)
			.HAlign(HAlign_Left)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UMaterialInterface::StaticClass())
				.ObjectPath(this, &FModularBuildingPluginEdModeToolkit::GetWallMaterialPath, 0)
				.OnObjectChanged(this, &FModularBuildingPluginEdModeToolkit::SetWallMaterial, 0)
				.AllowClear(true)
				]
			]

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.Padding(StandardPadding)
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("Change Material2", "Inner side", "Inner side | Index: "))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[
					/*SNew(SNumericEntryBox<int32>)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.AllowSpin(true)
					.MinValue(0)
					.MaxValue(100)
					.MaxSliderValue(10)
					.SliderExponent(1)
					.Value(this, &FModularBuildingPluginEdModeToolkit::GetMaterialSlotName, 1)
					.OnValueChanged(this, &FModularBuildingPluginEdModeToolkit::SetMaterialIndex, 1)*/
					SNew(SEditableTextBox)
					.Text(this, &FModularBuildingPluginEdModeToolkit::GetMaterialSlotName, 1)
					.OnTextCommitted(this, &FModularBuildingPluginEdModeToolkit::SetMaterialSlotName, 1)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 0.f)
				.HAlign(HAlign_Left)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMaterialInterface::StaticClass())
					.ObjectPath(this, &FModularBuildingPluginEdModeToolkit::GetWallMaterialPath, 1)
					.OnObjectChanged(this, &FModularBuildingPluginEdModeToolkit::SetWallMaterial, 1)
					.AllowClear(true)
				]
			]

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.Padding(StandardPadding)
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(LOCTEXT("SetMaterials1", "SetMaterials"))
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ButtonSetNewMaterialToWall)
				]
			];
}
	//Function For Widget 
void FModularBuildingPluginEdModeToolkit::SetWallMaterial(const FAssetData& AssetData, int32 it)
{
	//if (Cast<UMaterialInterface>(AssetData.GetAsset()))
	//{
		MaterialAssets[it] = Cast<UMaterialInterface>(AssetData.GetAsset());
	//}
}

FString FModularBuildingPluginEdModeToolkit::GetWallMaterialPath(int32 it) const
{
	return MaterialAssets[it].IsValid() ? MaterialAssets[it]->GetPathName() : FString("");
}

void FModularBuildingPluginEdModeToolkit::SetMaterialSlotName(const FText& NewName, ETextCommit::Type CommitInfo, int32 Num)
{
	MaterialSlotNames[Num] = NewName.ToString();
}

FText FModularBuildingPluginEdModeToolkit::GetMaterialSlotName(int32 Num) const
{
	return FText::FromString(MaterialSlotNames[Num]);
}

FReply FModularBuildingPluginEdModeToolkit::ButtonSetNewMaterialToWall()
{
	if (SelectedBuilding.IsValid())
	{		
		FMaterialStruct OuterSideMaterial;
		OuterSideMaterial.Material = MaterialAssets[0].Get();
		//OuterSideMaterial.Index = MaterialIndex[0];
		OuterSideMaterial.SlotName = MaterialSlotNames[0];

		FMaterialStruct InnerSideMaterial;
		InnerSideMaterial.Material = MaterialAssets[1].Get();
	//	InnerSideMaterial.Index = MaterialIndex[1];
		InnerSideMaterial.SlotName = MaterialSlotNames[1];
	

		SelectedBuilding->SetWallMaterialOnSelectCell(OuterSideMaterial, InnerSideMaterial);
	}

	return FReply::Handled();
}

//ChangeFunctions
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::Changes()
{
	FText T1 = NSLOCTEXT("Move Meshes On Floor2", "Move Meshes On Floor1", "Move Meshes On Floor");
	FText B1 = LOCTEXT("Move all floor X", "X");
	FText B2 = LOCTEXT("Move all floor -X", "-X");
	FText B3 = LOCTEXT("Move all floor Y", "Y");
	FText B4 = LOCTEXT("Move all floor -Y", "-Y");
	
	FText B10 = LOCTEXT("Deselect all actors", "SelectNone");
	FText B11 = LOCTEXT("Static to Inst", "Replace to InstMesh");
	FText B12 = LOCTEXT("Inst to Static", "Replace to StaticMesh");


	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.Padding(6.f, 1.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			//.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SButton)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("Delete Buttom's ", "Delete Last Wall"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteLastWall)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.f, 0.f)
			[
				SNew(SButton)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("Delete Buttom's Wall", "Delete Wall"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteWallsOnSelectedCells)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("Delete Buttom's Floor", "Delete Floor"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteFloorsOnSelectedCells)
			]
	
		]

		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.Padding(6.f, 1.f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("DeleteAllStaticMeshes", "Delete All Wall And Floor"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteAllBuildingParts)
			]
			
			+ SHorizontalBox::Slot()
			.Padding(5.f, 0.f, 0.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton) .IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(LOCTEXT("DeleteAll", "Delete All"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DeleteAll)
			]
			
		]		
	// MOVE PARTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
		.Padding(6.f, 3.f)
		.AutoHeight()
		[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				//.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(T1)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(0.32)

				+ SHorizontalBox::Slot()
				.Padding(4.f, 0.f)
				.HAlign(HAlign_Right)
				.AutoWidth()
				[
					SNew(SButton) .IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(B1)
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::MoveBuildingParts, EToMove::X)
				]
				+ SHorizontalBox::Slot()
				.Padding(0.f, 0.f, 6.f, 0.f)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(SButton) .IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(B2)
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::MoveBuildingParts, EToMove::InvertX)
				]

				+ SHorizontalBox::Slot()
				.Padding(4.f, 0.f)
				.HAlign(HAlign_Right)
				.AutoWidth()
				[
					SNew(SButton) .IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(B3)
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::MoveBuildingParts, EToMove::Y)
				]
				+ SHorizontalBox::Slot()
				.Padding(0.f, 0.f)
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(SButton) .IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.Text(B4)
					.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::MoveBuildingParts, EToMove::InvertY)
				]	
		]
	//TURN SELECTED NONE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
			.Padding(6.f, 3.f)
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 6.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton).IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
				.Text(FText::FromString("Turn Wall"))
				.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::TurnWallOnSelectedCells)
			]

		+ SHorizontalBox::Slot()
			.FillWidth(0.2f)

			+ SHorizontalBox::Slot()
			.Padding(0.f, 0.f, 6.f, 0.f)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(B10)
			.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::SelectNone)
			]
			]
// Copy/ Paste ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
		.Padding(6.f, 3.f)
		.AutoHeight()
		[
			SNew(SExpandableArea)
			.InitiallyCollapsed(true)
			.Padding(StandardPadding)
			.HeaderContent()
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Copy/Paste"))
				]
			]
			.BodyContent()
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.Padding(6.f, 3.f)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 10.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(FText::FromString("Copy"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CopySelectedBuildingParts)
					]

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 10.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(FText::FromString("Paste"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::PasteCopiedBuildingParts)
					]

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 10.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(FText::FromString("Cut"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::CutSelectedBuildingParts)
					]
				]

				+ SVerticalBox::Slot()
				.Padding(6.f, 0.f)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 10.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(FText::FromString("Turn"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::TurnCopiedBuildingParts)
					]

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 10.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(FText::FromString("Clear"))
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::ClearCopiedBuildingParts)
					]
				]

				+ SVerticalBox::Slot()
				.Padding(6.f, 3.f)
				.AutoHeight()			
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(0.2f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Selecte Copy Part:"))
					]

					+SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 5.f, 0.f)
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(110)
						//.HeightOverride(20)
						[
							SNew(SComboButton)
							.ButtonContent()
							[
								SNew(STextBlock)
								.Text(this, &FModularBuildingPluginEdModeToolkit::GetSelectedCopyPartName)
							]
							.HAlign(HAlign_Center)
							.ContentPadding(FMargin(2.f, 0.f))
							.MenuContent()
							[
								GetCopyPartContent()
							]
						]
					]
				]
			]
				
		]
	// Modes/Instanced meshes/Other ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		+ SVerticalBox::Slot()
		.Padding(6.f, 3.f)
		.AutoHeight()
		[
			SNew(SExpandableArea)
			.InitiallyCollapsed(true)
			.Padding(StandardPadding)
			.HeaderContent()
			[
				SNew(SHeader)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Modes/Instanced meshes/Other"))
				]
			]
			.Padding(FMargin(3.f, 0.f, 0.f, 0.f))
			.BodyContent()
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.Padding(6.f, 5.f)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(0.2f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Selecte Building Mode:"))
					]


					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.Padding(10.f, 0.f)
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(120)
						[
							SNew(SComboButton)
							.ButtonContent()
							[
								SNew(STextBlock)
								.Text(this, &FModularBuildingPluginEdModeToolkit::GetSelectedBuildingModeName)
							]
							.HAlign(HAlign_Center)
							.ContentPadding(FMargin(2.f, 0.f))
							.MenuContent()
							[
								GetBuildingModeContent()
							]
						]
					]
				]
//Only One Wall Mode
				+SVerticalBox::Slot()
				.Padding(6.f, 3.f)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.Padding(0.f, 0.f, 6.f, 0.f)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Only One Wall Mode", "Only One Wall Mode:"))
					]

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.AutoWidth()
					[
						CheckBoxes[4] = SNew(SCheckBox)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsCustomBuildingMode)
						.ToolTipText(LOCTEXT("Only One Wall Mode1", "You want to use Only One Wall Mode ? "))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged(this, &FModularBuildingPluginEdModeToolkit::SetOneWallMode)
					]
				//]

//FloorUnderGridCell

				//+ SVerticalBox::Slot()
				//.Padding(6.f, 0.f)
				//.HAlign(HAlign_Center)
				//.AutoHeight()
				//[
				//	SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.Padding(6.f, 0.f, 6.f, 0.f)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("FloorUnderGridCell", "Floor Under Grid Cell:"))
					]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					CheckBoxes[5] = SNew(SCheckBox)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
					.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsCustomBuildingMode)
					.ToolTipText(LOCTEXT("FloorUnderGridCell1", "Selecte Floor Under Grid Cell"))
					.IsChecked(ECheckBoxState::Unchecked)
					.OnCheckStateChanged(this, &FModularBuildingPluginEdModeToolkit::SetFloorUnderGridCellMode)
					]
				]
//Inst\Static Mesh
				+ SVerticalBox::Slot()
				.Padding(6.f, 4.f)
				.AutoHeight()
				[
					SNew(SSeparator)
				]

				+ SVerticalBox::Slot()
				.Padding(6.f, 3.f)
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 12.f, 0.f)
					.AutoWidth()
					[
						SNew(SCheckBox)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.ToolTipText(LOCTEXT("Static Mesh -> Inst Mesh c: ", " Auto replace Static Mesh to Inst Mesh"))
						.IsChecked(ECheckBoxState::Checked)
						.OnCheckStateChanged(this, &FModularBuildingPluginEdModeToolkit::SetIsAllToInstAuto)
					]

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 6.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(B11)
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::RunAllToInstMesh)
					]

					+ SHorizontalBox::Slot()
					.FillWidth(0.2f)

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 6.f, 0.f)
					.AutoWidth()
					[
						SNew(SButton)
						.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
						.Text(B12)
						.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::RunAllToStaticMesh)
					]
				]

				+ SVerticalBox::Slot()
				.Padding(6.f, 4.f)
				.AutoHeight()
				[
					SNew(SSeparator)
				]
//Other
				+ SVerticalBox::Slot()
				.Padding(6.f, 3.f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.Padding(0.f, 0.f, 6.f, 0.f)
					.FillWidth(0.5)
					[
						SNew(STextBlock)
						.Text(FText::FromString("If you have bugs try:"))
					]

					+ SHorizontalBox::Slot()
					.Padding(0.f, 0.f, 6.f, 0.f)
					.HAlign(HAlign_Right)
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(50)
						[
							SNew(SButton)
							.IsEnabled(this, &FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid)
							.Text(FText::FromString("Debug"))
							.OnClicked_Raw(this, &FModularBuildingPluginEdModeToolkit::DebugButton)
						]
					]
				]
			]
		];
}

FReply FModularBuildingPluginEdModeToolkit::MoveBuildingParts(EToMove ToMove)
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->MoveBuildingPartsOnSelectedFloorActor(ToMove);
	}

	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::TurnWallOnSelectedCells()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{	
		SelectedBuilding->TurnWallOnSelectCell();
	}
	return FReply::Handled(); 
}

FReply FModularBuildingPluginEdModeToolkit::DeleteLastWall()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid() )
	{
		SelectedBuilding->GetSelectedChildFloorActor()->DeleteLastWall();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteWallsOnSelectedCells()
{
	if (SelectedBuilding.IsValid() )
	{
		SelectedBuilding->DeleteOnSelectCells(ESelectBuildingParts::Wall);
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteFloorsOnSelectedCells()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->DeleteOnSelectCells(ESelectBuildingParts::Floor);
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteAllBuildingParts()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->DeleteAllWallsAndFloors();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::ClearGrid()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		
		SelectedBuilding->GetSelectedChildFloorActor()->ClearGrid();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::DeleteAll()
{
	DeleteAllBuildingParts();
	ClearGrid();
	return FReply::Handled();
}

	//Modes\Inst\Othet
TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::GetBuildingModeContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);
	{
		FUIAction OutsideAndInside(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetBuildingMode, EBuildingMode::OutsideAndInside));
		MenuBuilder.AddMenuEntry(FText::FromString("OutsideAndInside"), FText::FromString("OutsideAndInside"), FSlateIcon(), OutsideAndInside);

		FUIAction InsideOnly(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetBuildingMode, EBuildingMode::InsideOnly));
		MenuBuilder.AddMenuEntry(FText::FromString("InsideOnly"), FText::FromString("InsideOnly"), FSlateIcon(), InsideOnly);

		FUIAction Custom(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetBuildingMode, EBuildingMode::Custom));
		MenuBuilder.AddMenuEntry(FText::FromString("Custom"), FText::FromString("Custom"), FSlateIcon(), Custom);
	}
	return MenuBuilder.MakeWidget();
}

bool FModularBuildingPluginEdModeToolkit::IsCustomBuildingMode() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		return (SelectedBuilding->GetSelectedChildFloorActor()->GetSelectedBuildingMode() == EBuildingMode::Custom);
	}

	return false;
}

void FModularBuildingPluginEdModeToolkit::SetBuildingMode(EBuildingMode NewMode)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->SetBuildingMode(NewMode);

		switch (NewMode)
		{
		case EBuildingMode::OutsideAndInside:
		{
			SetOneWallMode(ECheckBoxState::Checked);
			SetFloorUnderGridCellMode(ECheckBoxState::Unchecked);
			break;
		}
		case EBuildingMode::InsideOnly:
		{
			SetOneWallMode(ECheckBoxState::Unchecked);
			SetFloorUnderGridCellMode(ECheckBoxState::Checked);
			break;
		}
		default:
			break;
		}

		ReloadMenuWidget();
	}
}

FText FModularBuildingPluginEdModeToolkit::GetSelectedBuildingModeName() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		EBuildingMode SelectedBuildingMode = SelectedBuilding->GetSelectedChildFloorActor()->GetSelectedBuildingMode();

		switch (SelectedBuildingMode)
		{
		case EBuildingMode::OutsideAndInside:
			return FText::FromString("OutsideAndInside");
			break;
		case EBuildingMode::InsideOnly:
			return FText::FromString("InsideOnly");
			break;
		case EBuildingMode::Custom:
			return FText::FromString("Custom");
			break;
		default:
			break;
		}
	}
	return FText::FromString("NoFloorActor");
}

void FModularBuildingPluginEdModeToolkit::SetOneWallMode(ECheckBoxState newState)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->SetOneWallMode( newState == ECheckBoxState::Checked) ;
		ReloadMenuWidget();
	}
}

void FModularBuildingPluginEdModeToolkit::SetFloorUnderGridCellMode(ECheckBoxState newState)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->SetFloorUnderGridCellMode(newState == ECheckBoxState::Checked);
		ReloadMenuWidget();
	}
}

FReply FModularBuildingPluginEdModeToolkit::DebugButton()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->DebugOnSelectedBuildingFloor();
	}
	ReloadMenuWidget();
	ReloadListOfObjectForCreating();
	ReloadListOfObjectComboButton();

	return FReply::Handled();
}

	//Copy\Paste
FReply FModularBuildingPluginEdModeToolkit::CopySelectedBuildingParts()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->CopySelectedBuildingParts();
	}

	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::PasteCopiedBuildingParts()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->PasteCopiedBuildingParts();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::CutSelectedBuildingParts()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->CutSelectedBuildingParts();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::TurnCopiedBuildingParts()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->TurnCopiedParts();
	}
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::ClearCopiedBuildingParts()
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->ClearCopiedParts();
	}
	return FReply::Handled();
}

TSharedRef<SWidget> FModularBuildingPluginEdModeToolkit::GetCopyPartContent() const
{
	FMenuBuilder MenuBuilder(true, NULL);
	{
		FUIAction CreatWallAndFloor(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetNewCopyPart, ESelectBuildingParts::WallAndFloor));
		MenuBuilder.AddMenuEntry(FText::FromString("WallAndFloor"), FText::FromString("WallAndFloor"), FSlateIcon(), CreatWallAndFloor);

		FUIAction CreatWall(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetNewCopyPart, ESelectBuildingParts::Wall));
		MenuBuilder.AddMenuEntry(FText::FromString("Wall"), FText::FromString("Wall"), FSlateIcon(), CreatWall);

		FUIAction CreatFloor(FExecuteAction::CreateSP(this, &FModularBuildingPluginEdModeToolkit::SetNewCopyPart, ESelectBuildingParts::Floor));
		MenuBuilder.AddMenuEntry(FText::FromString("Floor"), FText::FromString("Floor"), FSlateIcon(), CreatFloor);
	}
	return MenuBuilder.MakeWidget();
}

FText FModularBuildingPluginEdModeToolkit::GetSelectedCopyPartName() const
{
	if (SelectedBuilding.IsValid())
	{
		//SelectedBuilding->GetSelectedCopyPart()
		ESelectBuildingParts ItPart = SelectedBuilding->GetSelectedCopyPart();

		switch (ItPart)
		{
		case ESelectBuildingParts::Floor:
			return FText::FromString("Floor");
			break;
		case ESelectBuildingParts::Wall:
			return FText::FromString("Wall");
			break;
		case ESelectBuildingParts::WallAndFloor:
			return FText::FromString("WallAndFloor");
			break;
		default:
			break;
		}
	}
	return FText::FromString("No Building");
}

void FModularBuildingPluginEdModeToolkit::SetNewCopyPart(ESelectBuildingParts NewCopyPart)
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->SetSelectedCopyPart(NewCopyPart);
	}
}

//InstMesh \ StaticMesh
FReply FModularBuildingPluginEdModeToolkit::RunAllToInstMesh()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->AllToInstMesh();
	}
	else ReloadMenuWidget();
	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::RunAllToStaticMesh()
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
	{
		SelectedBuilding->GetSelectedChildFloorActor()->AllToStaticMesh();
	}
	else ReloadMenuWidget();
	return FReply::Handled();
}

void FModularBuildingPluginEdModeToolkit::SetIsAllToInstAuto(ECheckBoxState newState)
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid())
		SelectedBuilding->bAutoReplaceStaticInst = (newState == ECheckBoxState::Checked);
}

//Floor
FReply  FModularBuildingPluginEdModeToolkit::CopySelectedFloor()
{
	UObject* SelectActorObj = GEditor->GetSelectedActors()->GetSelectedObject(0);
	TWeakObjectPtr<AMBPFloorActor> SelectedFloorActor = Cast<AMBPFloorActor>(SelectActorObj);

	if (SelectedFloorActor.IsValid() && SelectedBuilding.IsValid())
	{
		SelectedBuilding->CopyThisChildFloorActor(SelectedFloorActor);
	}
	StartFunction();
	ReloadMenuWidget();

	return FReply::Handled();
}

FReply  FModularBuildingPluginEdModeToolkit::SelectNone()
{

	if (SelectedBuilding.IsValid())
		SelectedBuilding->SelectNone();

	SelectedBuilding = NULL;
	GEditor->SelectNone(true, true);
	ReloadMenuWidget();

	return FReply::Handled();
}

FReply FModularBuildingPluginEdModeToolkit::ChangeFloorPosition(bool bIsToUp)
{
	if (SelectedBuilding.IsValid())
	{
		SelectedBuilding->ChangeSelectedFloorPosition(bIsToUp);

		ReloadMenuWidget();
	}
		

	return FReply::Handled();
}
	//Function For Widget 
void FModularBuildingPluginEdModeToolkit::SetFloorChangeVector(float NewVal, EAxis::Type Axis)
{
	
	if (SelectedBuilding.IsValid())
	{
		FVector NewVector = SelectedBuilding->GetFloorChangeVectorForSelectedFloorActor();
		NewVector.SetComponentForAxis(Axis, NewVal);

		SelectedBuilding->SetFloorChangeVectorForSelectedFloorActor(NewVector);
	}
}

TOptional<float> FModularBuildingPluginEdModeToolkit::GetFloorChangeVector(EAxis::Type Axis) const
{
	if (SelectedBuilding.IsValid())
	{
		return SelectedBuilding->GetFloorChangeVectorForSelectedFloorActor().GetComponentForAxis(Axis);
	}
	return 0.f;
}

//Is Valid Check
bool FModularBuildingPluginEdModeToolkit::IsSelectedNeedsActorsValid() const
{
	if (SelectedBuilding.IsValid() && SelectedBuilding->GetSelectedChildFloorActor().IsValid()) return true;
	else return false;
}


FName FModularBuildingPluginEdModeToolkit::GetToolkitFName() const
{
	return FName("ModularBuildingPluginEdMode");
}

FText FModularBuildingPluginEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("ModularBuildingPluginEdModeToolkit", "DisplayName", "ModularBuildingPluginEdMode Tool");
}

class FEdMode* FModularBuildingPluginEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FModularBuildingPluginEdMode::EM_ModularBuildingPluginEdModeId);
}
#undef NSLOCTEXT_NAMESPACE
#undef LOCTEXT_NAMESPACE
