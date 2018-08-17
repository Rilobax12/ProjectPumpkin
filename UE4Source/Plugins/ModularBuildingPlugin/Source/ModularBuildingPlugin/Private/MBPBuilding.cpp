// Fill out your copyright notice in the Description page of Project Settings.SetNewVision(ESelectBuildingParts::Grid,

#include "ModularBuildingPluginPrivatePCH.h"
#include "MBPBuilding.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/UnrealEd/Public/Editor.h"


// Sets default values
AMBPBuilding::AMBPBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	auto NewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	NewRoot->SetMobility(EComponentMobility::Static);
	this->RootComponent = NewRoot;

	NameOfHome = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameOfHome"));
	NameOfHome->SetHiddenInGame(true);
	NameOfHome->AttachTo(this->RootComponent);
	NameOfHome->SetRelativeRotation(FRotator(90.f, 90.f, 0.f));
	NameOfHome->SetTextRenderColor(FColor::Red);
	NameOfHome->SetText(FText::FromString(this->GetName()));
	NameOfHome->SetXScale(1.f);
	NameOfHome->SetYScale(1.f);
	NameOfHome->SetWorldSize(200);

#if WITH_EDITOR
	USelection::SelectionChangedEvent.AddUObject(this, &AMBPBuilding::OnClick);
#endif
}

// Called when the game starts or when spawned
void AMBPBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBPBuilding::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMBPBuilding::PostActorCreated()
{
	Super::PostActorCreated();

	FTimerHandle UnusedHandle;
	
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMBPBuilding::AfterCopiedUpdate, 0.1f, false);
}

//Get
TArray<TWeakObjectPtr<AMBPFloorActor>> AMBPBuilding::GetChildFloorsActors()
{
	ListOfChildFloor.Empty();

	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);

	for (AActor* ItActor : AttachedActors)
	{
		TWeakObjectPtr<AMBPFloorActor> ItFloorActor = Cast<AMBPFloorActor>(ItActor);

		if (ItFloorActor.IsValid())
		{
			ListOfChildFloor.Add(ItFloorActor);
		}
	}
	//Sort by Height
	ListOfChildFloor.Sort([](const TWeakObjectPtr<AMBPFloorActor>& A, const TWeakObjectPtr<AMBPFloorActor>& B) {

		int32 NumA = A->GetActorLocation().Z;
		int32 NumB = B->GetActorLocation().Z;

		return NumA < NumB;
	});
	return ListOfChildFloor;
}

TWeakObjectPtr<AMBPFloorActor> AMBPBuilding::GetSelectedChildFloorActor()
{
	return SelectedChildFloorActor ;
}

//Spawn 
TWeakObjectPtr<AMBPFloorActor> AMBPBuilding::SpawnNewChildFloorActor()
{
	UpdateChildFloorsActors();

	UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
	FVector const ObjLocation = this->GetActorLocation();
	AMBPFloorActor* SpawningActor = (AMBPFloorActor*)MyWorld->SpawnActor(AMBPFloorActor::StaticClass(), &ObjLocation);

	SpawningActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	SpawningActor->SetActorLocation(
		ListOfChildFloor.IsValidIndex(ListOfChildFloor.Num() - 1) ? (ListOfChildFloor.Top()->GetActorLocation() + FVector(0.f, 0.f, ListOfChildFloor.Top()->GetComponentsBoundingBox().GetSize().Z))
		: this->GetActorLocation(), true);

	if (GetSelectedChildFloorActor().IsValid())
	{
		SpawningActor->GridScale = GetSelectedChildFloorActor()->GridScale;
		SpawningActor->SetFloorUnderGridCellMode(GetSelectedChildFloorActor()->GetFloorUnderGridCellMode());
		SpawningActor->SetOneWallMode(GetSelectedChildFloorActor()->GetOneWallMode());
		SpawningActor->SetBuildingMode(GetSelectedChildFloorActor()->GetSelectedBuildingMode());
	}

	SelectChildFloorActor(SpawningActor);

	return SpawningActor;
}

TWeakObjectPtr<AMBPFloorActor> AMBPBuilding::CopyThisChildFloorActor(TWeakObjectPtr<AMBPFloorActor> ThisChildFloorActor)
{
	UpdateChildFloorsActors();

	if (ThisChildFloorActor.IsValid())
	{
		FActorSpawnParameters Parameters;
		Parameters.Template = ThisChildFloorActor.Get();

		UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
		FVector const ObjLocation = this->GetActorLocation();
		AMBPFloorActor* SpawningActor = (AMBPFloorActor*)MyWorld->SpawnActor(AMBPFloorActor::StaticClass(), &ObjLocation, &FRotator::ZeroRotator, Parameters);

		SpawningActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		SpawningActor->SetActorLocation(
			ListOfChildFloor.IsValidIndex(ListOfChildFloor.Num() - 1) ? (ListOfChildFloor.Top()->GetActorLocation() + FVector(0.f, 0.f, ListOfChildFloor.Top()->GetComponentsBoundingBox().GetSize().Z))
			: this->GetActorLocation(), true);

		SpawningActor->SpawnGrid();
		UpdateBuildingPartOnFloorActor(SpawningActor, ThisChildFloorActor);

		SelectChildFloorActor(SpawningActor);
		return SpawningActor;
	}
	else return NULL;
}

void AMBPBuilding::AfterCopiedUpdate()
{
	if (BuildingLinkForCopy.IsValid() == false)
		BuildingLinkForCopy = this;

	UpdateChildFloorsActors();

	for (int32 it = 0; it < ListOfChildFloor.Num(); it++)
	{
		if(BuildingLinkForCopy->ListOfChildFloor.IsValidIndex(it))
			UpdateBuildingPartOnFloorActor(ListOfChildFloor[it], BuildingLinkForCopy->ListOfChildFloor[it]);
	}
	
}

//Select New 
void AMBPBuilding::SelectChildFloorActor(TWeakObjectPtr<AMBPFloorActor> NewSet, bool bIgnoreSelfLock)
{
	UpdateChildFloorsActors();

	if (SelectedChildFloorActor != NewSet || bIgnoreSelfLock)
	{
		SetVisionForAllFloorsActors(ESelectBuildingParts::Grid, false);

		if (SelectedChildFloorActor.IsValid())
		{
			SelectedChildFloorActor->TurnCeiling(false);

			if (bAutoReplaceStaticInst)  SelectedChildFloorActor->AllToInstMesh();
		}

		if (NewSet.IsValid())
		{
			SelectedChildFloorActor = NewSet;

			SetVisionForSelectedFloorActor(ESelectBuildingParts::Grid, true);
			SetVisionForSeveralFloorsActors(SelectedChildFloorActor, ESelectBuildingParts::WallAndFloor);


			if (bAutoReplaceStaticInst)
			{
				SelectedChildFloorActor->AllToStaticMesh();
			}

			GEditor->SelectNone(true, true);
			GEditor->SelectActor(NewSet.Get(), true, true);

			SelectedChildFloorActor->TurnCeiling(true);

			if (IsVisibilityOfCellsInRadius
				&& (SelectedChildFloorActor->ThisGrid.Get(0, 0).IsValid()))
			{
				GEditor->SelectComponent(SelectedChildFloorActor->ThisGrid.Get(0,0).Get(), true, true);
			}
		}
	}


}

void AMBPBuilding::SelectNone()
{
	SetVisionForSelectedFloorActor(ESelectBuildingParts::Grid, false);

	if (GetSelectedChildFloorActor().IsValid())
	{
		SelectedChildFloorActor->TurnCeiling(false);

		if(bAutoReplaceStaticInst)
			GetSelectedChildFloorActor()->AllToInstMesh();
	}

	SetVisionForAllFloorsActors(ESelectBuildingParts::WallAndFloor, true);

	UpdateChildFloorsActors();

	SelectedChildFloorActor = NULL;
}

//Update
void AMBPBuilding::UpdateChildFloorsActors()
{
	ListOfChildFloor.Empty();

	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);

	for (AActor* ItActor : AttachedActors)
	{
		TWeakObjectPtr<AMBPFloorActor> ItFloorActor = Cast<AMBPFloorActor>(ItActor);

		if (ItFloorActor.IsValid())
		{
			ListOfChildFloor.Add(ItFloorActor);
		}
	}
	//for (TActorIterator<AMBPFloorActor> ActorItr(GEditor->GetEditorWorldContext().World()); ActorItr; ++ActorItr)
	//{	
	//	AMBPFloorActor* ForGetParentActor  = *ActorItr;
	//	AMBPBuilding* TestIt = Cast<AMBPBuilding>(ForGetParentActor->GetAttachParentActor());
	//	if (this == TestIt)
	//	{
	//		ListOfChildFloor.Add(*ActorItr);
	//	}
	//}

	//Sort by Height
	ListOfChildFloor.Sort([](const TWeakObjectPtr<AMBPFloorActor>& A, const TWeakObjectPtr<AMBPFloorActor>& B) {

		int32 NumA = A->GetActorLocation().Z;
		int32 NumB = B->GetActorLocation().Z;

		return NumA < NumB;
	});

	FVector NewLocation = FVector::ZeroVector;
	for (int32 it = 0; it < ListOfChildFloor.Num(); it++)
	{
		if (it != 0)
		{
			FVector HeightOfFloor = FVector::ZeroVector, HeightOfFloor2 = FVector::ZeroVector;
			
			HeightOfFloor.Z = (int32)ListOfChildFloor[it-1 ]->GetComponentsBoundingBox().GetSize().Z / 2.f - (ListOfChildFloor[it-1]->GetComponentsBoundingBox().GetCenter().Z - ListOfChildFloor[it - 1]->GetActorLocation().Z);

			HeightOfFloor2.Z = (int32)ListOfChildFloor[it]->GetComponentsBoundingBox().GetSize().Z / 2.f - (ListOfChildFloor[it]->GetComponentsBoundingBox().GetCenter().Z - ListOfChildFloor[it]->GetActorLocation().Z);

			NewLocation = ListOfChildFloor[it - 1]->GetActorLocation()
				+ FVector(0.f, 0.f, (int32)ListOfChildFloor[it - 1]->GetComponentsBoundingBox().GetSize().Z)
				- HeightOfFloor
				+ ListOfChildFloor[it]->ChangeFloorActorLocation;

			if(ListOfChildFloor[it]->GetFloorUnderGridCellMode())
				NewLocation += HeightOfFloor2;
		}
		else
			NewLocation = this->GetActorLocation();
	
		ListOfChildFloor[it]->SetActorLocation(	NewLocation, true);
	}
}

void AMBPBuilding::UpdateBuildingPartOnFloorActor(TWeakObjectPtr<AMBPFloorActor> SelectedFloorActor, TWeakObjectPtr<AMBPFloorActor> ParentFloorActor)
{
	TArray<USceneComponent*> ItComponents;
	SelectedFloorActor->GetComponents<USceneComponent>(ItComponents);

	for (int32 it = 0; it < SelectedFloorActor->BuildingWalls.Num(); it++)
	{
		TWeakObjectPtr<UBuildingWall> CheckWall = SelectedFloorActor->BuildingWalls[it];
		if (CheckWall.IsValid())
		{
			SelectedFloorActor->BuildingWalls[it] = DuplicateObject(SelectedFloorActor->BuildingWalls[it], SelectedFloorActor.Get());
			
			if (SelectedFloorActor->BuildingWalls[it]->GetSceneComponent().IsValid())
			{
				for (USceneComponent* ItComponent : ItComponents)
				{
					if (ItComponent->GetName() == SelectedFloorActor->BuildingWalls[it]->GetSceneComponent()->GetName())
						SelectedFloorActor->BuildingWalls[it]->SetNewComponent(ItComponent);
				}
			}

			if (SelectedFloorActor->BuildingWalls[it]->GetChildActorComponent().IsValid() && ParentFloorActor.IsValid())
			{
				if(ParentFloorActor->BuildingWalls.IsValidIndex(it))
					SelectedFloorActor->BuildingWalls[it]->SetArrayOfMaterials(ParentFloorActor->BuildingWalls[it]->GetMaterials());
			}
		}
	}
	for (int32 it = 0; it < SelectedFloorActor->BuildingFloors.Num(); it++)
	{
		TWeakObjectPtr<UBuildingFloor> CheckFloor = SelectedFloorActor->BuildingFloors[it];
		if (CheckFloor.IsValid())
		{
			SelectedFloorActor->BuildingFloors[it] = DuplicateObject(SelectedFloorActor->BuildingFloors[it], SelectedFloorActor.Get());

			if (SelectedFloorActor->BuildingFloors[it]->GetSceneComponent().IsValid())
			{
				for (USceneComponent* ItComponent : ItComponents)
				{
					if (ItComponent->GetName() == SelectedFloorActor->BuildingFloors[it]->GetSceneComponent()->GetName())
						SelectedFloorActor->BuildingFloors[it]->SetNewComponent(ItComponent);
				}
			}

			if (SelectedFloorActor->BuildingFloors[it]->GetChildActorComponent().IsValid() && ParentFloorActor.IsValid())
			{
				if (ParentFloorActor->BuildingFloors.IsValidIndex(it))
					SelectedFloorActor->BuildingFloors[it]->SetArrayOfMaterials(ParentFloorActor->BuildingFloors[it]->GetMaterials());
			}
		}
	}
}

//Vision
void AMBPBuilding::SetVisionForSelectedFloorActor( ESelectBuildingParts Select, bool bNewVision)
{
	if (SelectedChildFloorActor.IsValid())
	{
		SelectedChildFloorActor->SetNewVision(Select, bNewVision);
	}
}

void AMBPBuilding::SetVisionForThisFloorActor(TWeakObjectPtr<AMBPFloorActor> ThisFloorActor, ESelectBuildingParts Select, bool bNewVision)
{
	if (ThisFloorActor.IsValid())
	{
		ThisFloorActor->SetNewVision(Select, bNewVision);
	}
}

void AMBPBuilding::SetVisionForSeveralFloorsActors(TWeakObjectPtr<AMBPFloorActor> InitialFloorActor, ESelectBuildingParts Select)
{
	if (ListOfChildFloor.Find(InitialFloorActor) != INDEX_NONE)
	{
		for (int32 it = ListOfChildFloor.Find(InitialFloorActor) + 1; it < ListOfChildFloor.Num(); it++)
		{
				SetVisionForThisFloorActor(ListOfChildFloor[it], Select, false);
		}
		for (int32 it = ListOfChildFloor.Find(InitialFloorActor); it >= 0; it--)
		{
				SetVisionForThisFloorActor(ListOfChildFloor[it], Select, true);
		}
	}
	
}

void AMBPBuilding::SetVisionForAllFloorsActors(ESelectBuildingParts Select, bool bNewVision)
{
	for (int32 it = 0; it < ListOfChildFloor.Num(); it++)
	{
			SetVisionForThisFloorActor(ListOfChildFloor[it], Select, bNewVision);
	}
}

//Click
void AMBPBuilding::OnClick(UObject* ThisObject)
{
	
	NameOfHome->SetText(this->GetName());

	//for (int32 it = 0; it < GEditor->GetSelectedActorCount(); it++)
	//{
	//	TWeakObjectPtr<AMBPBuilding> ThisBuilding = Cast<AMBPBuilding>(GEditor->GetSelectedActors()->GetSelectedObject(it));
	//	if (ThisBuilding.IsValid() && bChangeLastSelectedBuilding)
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("Okey"));
	//		LastSelectedBuilding = ThisBuilding;
	//		UE_LOG(LogTemp, Log, TEXT(" LastSelectedBuilding %s"), *LastSelectedBuilding->GetName());
	//	}
	//}

	if (SelectedChildFloorActor.IsValid())
	{
		SelectedCells.Empty();
		SelectedBuildingParts.Empty();
		TWeakObjectPtr<UChildActorComponent> SelectedTestActor;

		for (int32 it = 0; it < GEditor->GetSelectedComponents()->Num(); it++)
		{
			TWeakObjectPtr<UBuildingCell> ThisCell = Cast<UBuildingCell>(GEditor->GetSelectedComponents()->GetSelectedObject(it));
			if (ThisCell.IsValid())
			{
				SelectedCells.Add(ThisCell.Get());
			}

			TWeakObjectPtr<USceneComponent> ThisSceneComponent = Cast<USceneComponent>(GEditor->GetSelectedComponents()->GetSelectedObject(it));
			if (ThisSceneComponent.IsValid())
			{
				SelectedBuildingParts.Add(ThisSceneComponent.Get());
			}
			/*UE_LOG(LogTemp, Log, TEXT(" Try "));
			TWeakObjectPtr<AMBPBuilding> ThisBuilding = Cast<AMBPBuilding>(GEditor->GetSelectedComponents()->GetSelectedObject(it));
			if (ThisBuilding.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Okey"));
				LastSelectedBuilding = ThisBuilding;
					UE_LOG(LogTemp, Log, TEXT(" LastSelectedBuilding %s"), *LastSelectedBuilding->GetName());
			}*/
			
		}
	

		if (SelectedCells.Num() > 0)
		{
			//Other
			if (SelectedCells.Num() == 1 && GetSelectedChildFloorActor().IsValid())
			{
				//Grid Vision within radius
				if (GetIsVisibilityOfCellsInRadius())
					GetSelectedChildFloorActor()->VisibilityOfCellsInRadius(SelectedCells.Top());

				//Allot cells for copy
				if (GetSelectedChildFloorActor()->CopiedCells.Num() > 0)
					GetSelectedChildFloorActor()->AllotCellsForCopy(SelectedCells.Top());
			}

			//FastBuildMode Selecte One Cell
			if (SelectedCells.Num() == 1 && bFastBuildMode && SelectedComponents.MainObject.NewObject.IsValid() && bIgnoreSelect == false)
			{
				/*FTransform NewTransform = FTransform( FRotator::ZeroRotator, SelectedCells.Top()->RelativeLocation, FVector(1.f, 1.f, 1.f));

				if(SelectedCells.Top()->GetBuildingWallsOnIt().Num() != 0)
				NewTransform.SetRotation(SelectedCells.Top()->GetBuildingWallsOnIt().Top()->GetRelativeRotation().Quaternion());

				SelectedChildFloorActor->DeleteOnCell(SelectedCells.Top(), ESelectBuildingParts::Wall);

				SelectedChildFloorActor->CreatBuildingPart(SelectedComponents.MainObject.NewObject, NewTransform, SelectedCells.Top(), ESelectBuildingParts::Wall);*/
				SelectedComponents.SelectedCells = SelectedCells;
				SelectedComponents.bSelectOneCell = true;
				SelectedComponents.SelectedCells.Add(SelectedCells.Top());

				SelectedChildFloorActor->CreatWallsByObjectsFBM(SelectedComponents);
			}
			else if (SelectedCells.Num() == 1) bIgnoreSelect = false;

			//FastBuildMode Selecte Two Cells
			if (SelectedCells.Num() == 2 && bFastBuildMode && SelectedComponents.MainObject.NewObject.IsValid())
			{
				UBuildingCell* SecodCell = SelectedCells[1];

				SelectedComponents.SelectedCells = SelectedCells;
				SelectedComponents.bSelectOneCell = false;

				SelectedChildFloorActor->CreatWallsByObjectsFBM(SelectedComponents);

				GEditor->SelectNone(false, false);
				GEditor->SelectActor(SelectedChildFloorActor.Get(), true, true);

				bIgnoreSelect = true;
				GEditor->SelectComponent(SecodCell, true, true);
			}
		}
		else
		{
			//ReplaceSelectedWallToNewWall
			if (GetSelectedChildFloorActor().IsValid() && SelectedBuildingParts.Num() > 0 && bFastBuildMode)
			{
				for (int32 it = SelectedBuildingParts.Num() - 1; it >= 0; it--)
				{
					TWeakObjectPtr<UBuildingWall> SelectedWall = GetSelectedChildFloorActor()->GetBuildingWallBySceneComponent(SelectedBuildingParts[it]);

					if (SelectedWall.IsValid())
					{
						GetSelectedChildFloorActor()->ReplaceSelectedWallToNewWall(SelectedWall, SelectedComponents);
					}
				}
				//GEditor->SelectNone(false, false);
				//GEditor->SelectActor(SelectedChildFloorActor.Get(), true, true);
			}
		}


	}
}

//Creat
void AMBPBuilding::CreateFloorsByObject(int32 SelectedComponentIndex, TWeakObjectPtr<AMBPComponentsForCreating> SelectedComponentsForCreating)
{
	if (SelectedCells.Num() == 1)
	{
		UBuildingCell* FirstCell = SelectedCells[0];
		SelectedCells.Add(FirstCell);
	}
	if (SelectedCells.Num() == 2 && SelectedComponentsForCreating->GetObjectByIndex(SelectedComponentIndex).IsValid() && GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->CreatFloorsByComponent(
			SelectedComponentsForCreating->GetObjectByIndex(SelectedComponentIndex).Get(),
			SelectedComponentsForCreating->GetChangeVectorByIndex(SelectedComponentIndex), 
			SelectedCells);
	}
}

//Fast Build Mode
void AMBPBuilding::SetSelectedComponentsForFBM(int32 SelectedComponentIndex, TWeakObjectPtr<AMBPComponentsForCreating> SelectedComponentsForCreating)
{
	SelectedComponents.Empty();

	if (SelectedComponentIndex != -1)
	{
		SelectedComponents.MainObject.NewObject = SelectedComponentsForCreating->GetObjectByIndex(SelectedComponentIndex);
		SelectedComponents.MainObject.WallOffset = SelectedComponentsForCreating->GetOffsetByIndex(SelectedComponentIndex);
		SelectedComponents.MainObject.ObjectChangeVector = SelectedComponentsForCreating->GetChangeVectorByIndex(SelectedComponentIndex);

		TArray<TWeakObjectPtr<UObject>> ObjectsForCreting = SelectedComponentsForCreating->GetSelectedListOfObjects().GetWeakObjectsForCreatingPtr();

		for (int32 it = 0; it < ObjectsForCreting.Num(); it++)
		{
			if (ObjectsForCreting[it].IsValid())
			{
				if (ObjectsForCreting[it]->GetName().Contains(FString("Left")))
				{
					SelectedComponents.LeftObject.NewObject = ObjectsForCreting[it];
					SelectedComponents.LeftObject.WallOffset = SelectedComponentsForCreating->GetOffsetByIndex(it);
					SelectedComponents.LeftObject.ObjectChangeVector = SelectedComponentsForCreating->GetChangeVectorByIndex(it);
				}
				else
					if (ObjectsForCreting[it]->GetName().Contains(FString("Right")))
					{
						SelectedComponents.RightObject.NewObject = ObjectsForCreting[it];
						SelectedComponents.RightObject.WallOffset = SelectedComponentsForCreating->GetOffsetByIndex(it);
						SelectedComponents.RightObject.ObjectChangeVector = SelectedComponentsForCreating->GetChangeVectorByIndex(it);
					}
			}
		}
	}
	
	if (GetSelectedChildFloorActor().IsValid())
	{
		if (GetSelectedChildFloorActor()->GetDeleteOnlyFromListMode())
			GetSelectedChildFloorActor()->SetNewListOfObjectToDelete(SelectedComponentsForCreating->GetSelectedListOfObjects().GetWeakObjectsForCreatingPtr());
	}
}

bool AMBPBuilding::GetFastBuildMode()
{
	return bFastBuildMode;
}

void AMBPBuilding::SetFastBuildMode(bool bNewFastBuildMode)
{
	bFastBuildMode = bNewFastBuildMode;
}

//Delete
void AMBPBuilding::DeleteOnSelectCells(ESelectBuildingParts SelectPart)
{
	if (SelectedCells.Num() == 0)
	{
		DeleteSelectedBuildingParts();
	}
	else
	{
		if (GetSelectedChildFloorActor().IsValid() && SelectedCells.Num() == 1)
			GetSelectedChildFloorActor()->DeleteOnCell(SelectedCells.Top(), SelectPart);

		if (GetSelectedChildFloorActor().IsValid() && SelectedCells.Num() == 2)
			GetSelectedChildFloorActor()->DeleteOnSelectedCells(SelectPart, SelectedCells);
	}
}

void AMBPBuilding::DeleteSelectedBuildingParts()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		for (TWeakObjectPtr<USceneComponent> ItSceneComponent : SelectedBuildingParts)
		{
			if (ItSceneComponent.IsValid())
			{
				bool bDeleteResult = GetSelectedChildFloorActor()->DeleteBuildingPartBySceneComponent(ItSceneComponent);

				if (bDeleteResult == false)
				{
					ItSceneComponent->UnregisterComponent();
					ItSceneComponent->DestroyComponent(true);
				}
			}
		}

		GetSelectedChildFloorActor()->RegisterAllComponents();
	}
}

//Change
void AMBPBuilding::TurnWallOnSelectCell()
{
	if (GetSelectedChildFloorActor().IsValid() && SelectedCells.Num() == 1)
		GetSelectedChildFloorActor()->TurnWallOnSelectCell( SelectedCells.Top() );
}

void AMBPBuilding::MoveBuildingPartsOnSelectedFloorActor(EToMove ToMove)
{
	if (GetSelectedChildFloorActor().IsValid())
		GetSelectedChildFloorActor()->MoveBuildingParts(ToMove);
}

void AMBPBuilding::DebugOnSelectedBuildingFloor()
{
	if (GetSelectedChildFloorActor().IsValid())
		GetSelectedChildFloorActor()->Debug();

	UpdateChildFloorsActors();
}
//Copy\Paste
void AMBPBuilding::CopySelectedBuildingParts()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		if (SelectedCells.Num() == 1)
		{
			UBuildingCell* FirstCell = SelectedCells[0];
			SelectedCells.Add(FirstCell);
		}
		if (SelectedCells.Num() == 2 && GetSelectedChildFloorActor().IsValid())
		{
			GetSelectedChildFloorActor()->bIsCutBuildingParts = false;
			GetSelectedChildFloorActor()->CopyBuildingPartsOnSelectedCells(SelectedCells);
		}
	}
}

void AMBPBuilding::PasteCopiedBuildingParts()
{
	if (SelectedCells.Num() == 1 && GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->PasteCopiedBuildingParts(SelectedCells.Top());
	}
}

void AMBPBuilding::CutSelectedBuildingParts()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		if (SelectedCells.Num() == 1)
		{
			UBuildingCell* FirstCell = SelectedCells[0];
			SelectedCells.Add(FirstCell);
		}
		if (SelectedCells.Num() == 2 && GetSelectedChildFloorActor().IsValid())
		{
			GetSelectedChildFloorActor()->bIsCutBuildingParts = true;
			GetSelectedChildFloorActor()->CopyBuildingPartsOnSelectedCells(SelectedCells);
		}
			
	}
}

ESelectBuildingParts AMBPBuilding::GetSelectedCopyPart()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		return GetSelectedChildFloorActor()->SelectedCopyPart;
	}
	return ESelectBuildingParts::WallAndFloor;
}

void AMBPBuilding::SetSelectedCopyPart(ESelectBuildingParts NewSelectedCopyPart)
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->SelectedCopyPart = NewSelectedCopyPart;
	}
}

void AMBPBuilding::TurnCopiedParts()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->TurnCopiedParts();

		if (SelectedCells.Num() > 0)
			GetSelectedChildFloorActor()->AllotCellsForCopy(SelectedCells.Top());
	}
}

void AMBPBuilding::ClearCopiedParts()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->CopiedCells.Empty();

		GetSelectedChildFloorActor()->AllotCellsForCopy(NULL);
	}
}

//Material
void AMBPBuilding::SetWallMaterialOnSelectCell(FMaterialStruct OuterMaterial_L, FMaterialStruct InnerMaterial_L)
{
	if (SelectedCells.Num() == 1)
	{
		UBuildingCell* FirstCell = SelectedCells[0];
		SelectedCells.Add(FirstCell);
	}

	if (SelectedCells.Num() == 2  && GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->OuterMaterial = OuterMaterial_L;
		GetSelectedChildFloorActor()->InnerMaterial = InnerMaterial_L;

		GetSelectedChildFloorActor()->SetWallMaterial(SelectedCells);
	}
}

//Mode
bool AMBPBuilding::GetIsVisibilityOfCellsInRadius()
{
	return IsVisibilityOfCellsInRadius;
}

void AMBPBuilding::SetIsVisibilityOfCellsInRadius(bool NewSet)
{
	IsVisibilityOfCellsInRadius = NewSet;

	if (NewSet == false)
		SetVisionForSelectedFloorActor(ESelectBuildingParts::Grid, true);
}

//Floor
void AMBPBuilding::ChangeSelectedFloorPosition(bool bIsToUp)
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		int32 ItIndex = ListOfChildFloor.Find(SelectedChildFloorActor);
		int32 AddIndex = bIsToUp ? 1 : -1;

		if (ListOfChildFloor.IsValidIndex(ItIndex + AddIndex))
		{
			/*FVector NewLocation = ((ListOfChildFloor[ItIndex + AddIndex]->GetActorLocation()
				+ ListOfChildFloor[ItIndex + AddIndex + AddIndex]->GetActorLocation())
				/ 2) * AddIndex;

			SelectedChildFloorActor->SetActorLocation(ListOfChildFloor[ItIndex + AddIndex]->GetActorLocation() + NewLocation);*/
			FVector OldLocation = SelectedChildFloorActor->GetActorLocation();

			SelectedChildFloorActor->SetActorLocation(ListOfChildFloor[ItIndex + AddIndex]->GetActorLocation());
			ListOfChildFloor[ItIndex + AddIndex]->SetActorLocation(OldLocation);

			SelectChildFloorActor(SelectedChildFloorActor, true);
		}
	}
}

void AMBPBuilding::SetFloorChangeVectorForSelectedFloorActor(FVector NewVector)
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		GetSelectedChildFloorActor()->ChangeFloorActorLocation = NewVector;

		UpdateChildFloorsActors();
	}
}

FVector AMBPBuilding::GetFloorChangeVectorForSelectedFloorActor()
{
	if (GetSelectedChildFloorActor().IsValid())
	{
		return GetSelectedChildFloorActor()->ChangeFloorActorLocation;
	}
	return FVector::ZeroVector;
}

//Check
bool AMBPBuilding::IsItSelectedFloorActor(TWeakObjectPtr<AMBPFloorActor> ItFloorActor)
{
	if (ItFloorActor.IsValid() && GetSelectedChildFloorActor().IsValid())
	{
		return (ItFloorActor == GetSelectedChildFloorActor());
	}
	return false;
}