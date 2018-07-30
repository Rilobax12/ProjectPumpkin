// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularBuildingPluginPrivatePCH.h"
#include "MBPFloorActor.h"
#include "MBPListOfFurnit.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstance.h"
//#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
//#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/ChildActorComponent.h"
#include "Editor/UnrealEd/Public/Editor.h"
//#include "Runtime/Engine/Classes/Engine/DestructibleMesh.h"

// Sets default values
AMBPFloorActor::AMBPFloorActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	auto NewRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	NewRoot->SetMobility(EComponentMobility::Static);
	this->RootComponent = NewRoot;

	GridScale = 1.0f;

	SelectedBuildingMode = EBuildingMode::OutsideAndInside;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Grid_Mesh(TEXT("/ModularBuildingPlugin/Grid"));
	Grid_StaticMesh = Grid_Mesh.Object;
	
	SelectGridRad = 15;
}

// Called when the game starts or when spawned
void AMBPFloorActor::BeginPlay()
{
	Super::BeginPlay();
	SetNewVision(ESelectBuildingParts::WallAndFloor, true);
	//SetNewVision(ESelectBuildingParts::Grid, false);
}

void AMBPFloorActor::PostActorCreated() 
{
	Super::PostActorCreated();
	
	SelecetSpawnParam = NewObject<UInstancedStaticMeshComponent>(this, TEXT("Selecet Spawn Param Comp"));
	
	if (this->RootComponent)
		SelecetSpawnParam->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SelecetSpawnParam->SetMobility(EComponentMobility::Static);
	SelecetSpawnParam->RegisterComponent();
	
	//FTimerHandle UnusedHandle;
	//UE_LOG(LogTemp, Log, TEXT("Parant This %d"), InstMeshes.Num());
  //  GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMBPFloorActor::CopyHome, 0.1f, false);
}

// Called every frame
void AMBPFloorActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMBPFloorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

//Grid
void AMBPFloorActor::SpawnGrid()
{
	 int32 X = GridX;
	 int32 Y = GridY;
	 ClearGrid();
	 GridX = X;
	 GridY = Y;

	 FVector NewLocation = FVector(50.f * GridScale, 50.f * GridScale, HeightOfGrid) ;

	for (int32 it = 0; it < GridX; it++)
	{
		ThisGrid.AddSetOfGridByX();

		for (int32 it0 = 0; it0 < GridY; it0++)
		{
			ThisGrid.SetsOfGridsByX[it].GridsByY.Add(NewObject<UBuildingCell>(this, UBuildingCell::StaticClass()));
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			if (Grid_StaticMesh != NULL)
				ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->SetStaticMesh(Grid_StaticMesh);
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->SetRelativeScale3D(FVector(GridScale, GridScale, 1.f));
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->SetCellLocation(NewLocation);
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->RegisterComponent();
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->SetHiddenInGame(true);
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->Y = it0;
			ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->X = it;

			for (UBuildingWall* ItWall : BuildingWalls)
			{
				if (ItWall->CellLocation.Equals( FVector(NewLocation.X, NewLocation.Y , 0.f) , 5))
					ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->AddBuildingWall(ItWall);
			}
			for (UBuildingFloor* ItFloor : BuildingFloors)
			{
				if (ItFloor->CellLocation.Equals(FVector(NewLocation.X, NewLocation.Y, 0.f), 5))
					ThisGrid.SetsOfGridsByX[it].GridsByY.Top()->AddBuildingFloor(ItFloor);
			}

			NewLocation.Y += 100.f * GridScale;
		}

		NewLocation.Y = 50.f * GridScale;
		NewLocation.X += 100.f * GridScale;
	}
	RegisterAllComponents();
}

void AMBPFloorActor::VisibilityOfCellsInRadius(UBuildingCell* SelectedCell)
{
	int32 NewCellX = 0, NewCellY = 0;
	UBuildingCell* FirstCell; UBuildingCell* SecondCell;

	if ((SelectedCell->X - SelectGridRad) > NewCellX)
		NewCellX = SelectedCell->X - SelectGridRad;
	if ((SelectedCell->Y - SelectGridRad) > NewCellY)
		NewCellY = SelectedCell->Y - SelectGridRad;

	FirstCell = ThisGrid.Get(NewCellX, NewCellY).Get();

	NewCellX = ThisGrid.GetNumByX() - 1, NewCellY = ThisGrid.GetNumByY() - 1;

	if ((SelectedCell->X + SelectGridRad) < NewCellX)
		NewCellX = SelectedCell->X + SelectGridRad;
	if ((SelectedCell->Y + SelectGridRad) < NewCellY)
		NewCellY = SelectedCell->Y + SelectGridRad;

	SecondCell = ThisGrid.Get(NewCellX, NewCellY).Get();
	
	int32 MinX = FirstCell->X;
	int32 MaxX = SecondCell->X;

	int32 MinY = FirstCell->Y;
	int32 MaxY = SecondCell->Y;

	SetNewVision(ESelectBuildingParts::Grid, false);

	for (int32 itX = MinX; itX <= MaxX; itX++)
	{
		for (int32 itY = MinY; itY <= MaxY; itY++)
		{
			ThisGrid.Get(itX, itY)->SetVisibility(true);
		}
	}
}

void AMBPFloorActor::ClearGrid()
{
	TArray<UBuildingCell*> Components;
	this->GetComponents<UBuildingCell>(Components);
	for (UBuildingCell* itCell : Components)
	{
		itCell->UnregisterComponent();
		itCell->DestroyComponent(true);
	}

	GridX = 0;
	GridY = 0;
	ThisGrid.SetsOfGridsByX.Empty();

}

void AMBPFloorActor::UpdateGridLocation(bool bCheckAllFloors, float NewFloorHeight)
{
	float MaxHeightOfGrid = 0.f;

	if (bCheckAllFloors)
	{
		for (UBuildingFloor* ItFloor : BuildingFloors)
		{
			if (MaxHeightOfGrid < ItFloor->BuildingFloorHeight )
				MaxHeightOfGrid = ItFloor->BuildingFloorHeight;
		}
	}
	else MaxHeightOfGrid = (NewFloorHeight > GetHeightOfGrid()) ? NewFloorHeight : GetHeightOfGrid();
	

	if (MaxHeightOfGrid != GetHeightOfGrid())
	{
		HeightOfGrid = MaxHeightOfGrid;

		TArray<UBuildingCell*> Components;
		this->GetComponents<UBuildingCell>(Components);

		for (UBuildingCell* itCell : Components)
		{
			itCell->SetCellLocation(FVector(itCell->RelativeLocation.X, itCell->RelativeLocation.Y, HeightOfGrid));
		}
	}
}

float AMBPFloorActor::GetHeightOfGrid()
{
	return HeightOfGrid;
}

//InstancedStaticMesh
void AMBPFloorActor::AllToInstMesh()
{
	SetNewVision(ESelectBuildingParts::Grid, false);
	TArray<TWeakObjectPtr<UInstancedStaticMeshComponent>> InstancedMeshes;
	TArray<TArray<FVector>> CellsLocationsForInstancedComponents;

	bool New = false;

	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWalls)
	{
		if (ItWall.IsValid() ? ItWall.Get()->GetStaticMeshComponent().IsValid() : false)
		{
			New = true;
			for (int32 it = 0; it < InstancedMeshes.Num(); it++)
			{
				if (InstancedMeshes[it]->GetStaticMesh() == ItWall.Get()->GetStaticMeshComponent()->GetStaticMesh())
				{
					New = false;
					for (int32 it1 = 0; it1 < InstancedMeshes[it]->GetMaterials().Num(); it1++)
					{
						if (InstancedMeshes[it]->GetMaterials()[it1] != ItWall.Get()->GetStaticMeshComponent()->GetMaterials()[it1])
						{
							New = true;
							break;
						}
					}
					if (!New)
					{
						InstancedMeshes[it]->AddInstance(ItWall.Get()->GetStaticMeshComponent()->GetRelativeTransform());
						CellsLocationsForInstancedComponents[it].Add(ItWall->CellLocation);
						break;
					}
				}
			}
			if (New)
			{
				 InstancedMeshes.AddDefaulted(1);
				 InstancedMeshes.Top() = DuplicateObject(SelecetSpawnParam, this);
				 InstancedMeshes.Top()->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

				 InstancedMeshes.Top()->SetStaticMesh(ItWall.Get()->GetStaticMeshComponent()->GetStaticMesh());
				 InstancedMeshes.Top()->AddInstance(ItWall.Get()->GetStaticMeshComponent()->GetRelativeTransform());
				for (int32 it1 = 0; it1 < ItWall.Get()->GetStaticMeshComponent()->GetMaterials().Num(); it1++)
				{
					 InstancedMeshes.Top()->SetMaterial(it1, ItWall.Get()->GetStaticMeshComponent()->GetMaterials()[it1]);
				}
				CellsLocationsForInstancedComponents.AddDefaulted(1);
				CellsLocationsForInstancedComponents.Top().Add(ItWall->CellLocation);
			}
			DeferredDeleteAdd(ItWall , ESelectBuildingParts::Wall);
		}
	}
	for (int32 it = 0; it < InstancedMeshes.Num(); it++)
	{
		BuildingWalls.Add(NewObject<UBuildingWall>(this, UBuildingWall::StaticClass()));
		BuildingWalls.Top()->SetNewComponent(InstancedMeshes[it].Get());
		BuildingWalls.Top()->SetCellsLocationsForInstancedComponents(CellsLocationsForInstancedComponents[it]);
		BuildingWalls.Top()->SetTypeOfThisWall(GetTypeOfThisObject(InstancedMeshes[it]->GetStaticMesh()));
	}
	InstancedMeshes.Empty(); CellsLocationsForInstancedComponents.Empty();

	//Floor
	for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloors)
	{
		if (ItFloor.IsValid() ? ItFloor.Get()->GetStaticMeshComponent().IsValid() : false)
		{
			New = true;
			for (int32 it = 0; it < InstancedMeshes.Num(); it++)
			{
				if (InstancedMeshes[it]->GetStaticMesh() == ItFloor.Get()->GetStaticMeshComponent()->GetStaticMesh())
				{
					New = false;
					for (int32 it1 = 0; it1 < InstancedMeshes[it]->GetMaterials().Num(); it1++)
					{
						if (InstancedMeshes[it]->GetMaterials()[it1] != ItFloor.Get()->GetStaticMeshComponent()->GetMaterials()[it1])
						{
							New = true;
							break;
						}
					}
					if (!New)
					{
						InstancedMeshes[it]->AddInstance(ItFloor.Get()->GetStaticMeshComponent()->GetRelativeTransform());
						//for (int32 it1 = 0; it1 < ItFloor.Get()->GetStaticMeshComponent()->GetMaterials().Num(); it1++)
						//{
						//	InstancedMeshes[it]->SetMaterial(it1, ItFloor.Get()->GetStaticMeshComponent()->GetMaterials()[it1]);
						//}
						CellsLocationsForInstancedComponents[it].Add(ItFloor->CellLocation);
						break;
					}
				}
			}
			if (New)
			{
				 InstancedMeshes.AddDefaulted(1);
				 InstancedMeshes.Top() = DuplicateObject(SelecetSpawnParam, this);
				 InstancedMeshes.Top()->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

				 InstancedMeshes.Top()->SetStaticMesh(ItFloor.Get()->GetStaticMeshComponent()->GetStaticMesh());
				 InstancedMeshes.Top()->AddInstance(
					ItFloor.Get()->GetStaticMeshComponent()->GetRelativeTransform()
					);
				for (int32 it1 = 0; it1 < ItFloor.Get()->GetStaticMeshComponent()->GetMaterials().Num(); it1++)
				{
					 InstancedMeshes.Top()->SetMaterial(it1, ItFloor.Get()->GetStaticMeshComponent()->GetMaterials()[it1]);
				}
				CellsLocationsForInstancedComponents.AddDefaulted(1);
				CellsLocationsForInstancedComponents.Top().Add(ItFloor->CellLocation);
			}
			DeferredDeleteAdd(ItFloor, ESelectBuildingParts::Floor);
		}
	}
	for (int32 it = 0; it < InstancedMeshes.Num(); it++)
	{
		BuildingFloors.Add(NewObject<UBuildingFloor>(this, UBuildingFloor::StaticClass()));
		BuildingFloors.Top()->SetNewComponent(InstancedMeshes[it].Get());
		BuildingFloors.Top()->SetCellsLocationsForInstancedComponents(CellsLocationsForInstancedComponents[it]);
	}
	DeferredDeleteStart();
}

void AMBPFloorActor::AllToStaticMesh()
{
	FTransform InstanceTransform; 
	TArray<UBuildingWall*> BuildingWallsLocal = BuildingWalls;
	TArray<UBuildingFloor*> BuildingFloorsLocal = BuildingFloors;
	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWallsLocal)
	{
		if (ItWall.IsValid() ? ItWall->GetInstancedStaticMeshComponent().IsValid() : false)
		{
			for (int32 it = 0; it < ItWall->GetInstancedStaticMeshComponent()->GetInstanceCount(); it++)
			{
				ItWall->GetInstancedStaticMeshComponent()->GetInstanceTransform(it, InstanceTransform, false);

				CreatWallByStaticMesh( ItWall->GetInstancedStaticMeshComponent()->GetStaticMesh(),
					0.f ,
					FVector::ZeroVector,
					FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector),
					NULL , 
					ItWall->GetTypeOfThisWall());

				BuildingWalls.Top()->CellLocation = ItWall->GetCellsLocationsForInstancedComponents().IsValidIndex(it) ? ItWall->GetCellsLocationsForInstancedComponents()[it] : FVector::ZeroVector;
				BuildingWalls.Top()->GetPrimitiveComponent()->SetRelativeTransform(InstanceTransform);

				for (int32 it1 = 0; it1 < ItWall->GetInstancedStaticMeshComponent()->GetMaterials().Num(); it1++)
				{
					BuildingWalls.Top()->GetPrimitiveComponent()->SetMaterial(it1, ItWall->GetInstancedStaticMeshComponent()->GetMaterials()[it1]);
				}
			}
			DeferredDeleteAdd(ItWall, ESelectBuildingParts::Wall);
		}
	}

	for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloorsLocal)
	{
		if (ItFloor.IsValid() ? ItFloor->GetInstancedStaticMeshComponent().IsValid() : false)
		{
			for (int32 it = 0; it < ItFloor->GetInstancedStaticMeshComponent()->GetInstanceCount(); it++)
			{
				ItFloor->GetInstancedStaticMeshComponent()->GetInstanceTransform(it, InstanceTransform, false);

				CreatFloorByStaticMesh(ItFloor->GetInstancedStaticMeshComponent()->GetStaticMesh(),
					FVector::ZeroVector,
					FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector), 
					NULL);

				BuildingFloors.Top()->CellLocation = ItFloor->GetCellsLocationsForInstancedComponents().IsValidIndex(it) ? ItFloor->GetCellsLocationsForInstancedComponents()[it] : FVector::ZeroVector;
				BuildingFloors.Top()->GetPrimitiveComponent()->SetRelativeTransform(InstanceTransform);

				for (int32 it1 = 0; it1 < ItFloor->GetInstancedStaticMeshComponent()->GetMaterials().Num(); it1++)
				{
					BuildingFloors.Top()->GetPrimitiveComponent()->SetMaterial(it1, ItFloor->GetInstancedStaticMeshComponent()->GetMaterials()[it1]);
				}
			}
			DeferredDeleteAdd(ItFloor, ESelectBuildingParts::Floor);
		}
	}
	DeferredDeleteStart();

	SpawnGrid();
}

//Change Functions
void AMBPFloorActor::TurnWallOnSelectCell(UBuildingCell* SelectCell)
{
	if (SelectCell->GetBuildingWallsOnIt().Num() > 0)
	{
		TWeakObjectPtr<UBuildingWall> ItWall = SelectCell->GetBuildingWallsOnIt().Top();
		if (ItWall.IsValid())
		{
			ItWall->SetBuildingPartRotation(ItWall->GetRelativeRotation() - FRotator(0.f, 90.f, 0.f));
		}
	}
}

void AMBPFloorActor::MoveBuildingParts(EToMove ToMove)
{
	FVector AddVector = FVector::ZeroVector;

	switch (ToMove)
	{
	case EToMove::X:
		AddVector = FVector(100.f * GridScale, 0.f, 0.f);
		break;
	case EToMove::InvertX:
		AddVector = FVector(100.f * GridScale * -1.f, 0.f, 0.f);
		break;
	case EToMove::Y:
		AddVector = FVector(0.f, 100.f * GridScale, 0.f);
		break;
	case EToMove::InvertY:
		AddVector = FVector(0.f, 100.f * GridScale * -1.f, 0.f);
		break;
	default:
		break;
	}

	for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWalls )
	{
		if (ItWall.IsValid())
		{
			ItWall->AddVectorToLocation( AddVector);
			ItWall->CellLocation = ItWall->CellLocation + AddVector;
		}
	}
	for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloors)
	{
		if (ItFloor.IsValid())
		{
			ItFloor->AddVectorToLocation( AddVector);
			ItFloor->CellLocation = ItFloor->CellLocation + AddVector;
		}
	}
	SpawnGrid();
}

void AMBPFloorActor::TurnCeiling(bool TurnSet)
{
	for (UBuildingFloor* ItFloor : BuildingFloors)
	{
		TWeakObjectPtr<UObject> TestObj = ItFloor->UsedObject;
		if (TestObj.IsValid())
		{
			if ((ItFloor->GetName().Contains("Ceiling") || ItFloor->UsedObject->GetName().Contains("Ceiling"))
				&& ItFloor->bTurnCeiling != TurnSet)
			{
				FVector AddVector = FVector(0.f, 0.f, ItFloor->Center.Z * 2);
				if (TurnSet == true)
				{
					ItFloor->SetRelativeScale(FVector(1.f, 1.f, -1.f));
					//ItFloor->SetBuildingPartLocation(ItFloor->GetRelativeLocation() + AddVector);
				}
				else
				{
					ItFloor->SetRelativeScale(FVector(1.f, 1.f, 1.f));
					//ItFloor->SetBuildingPartLocation(ItFloor->GetRelativeLocation() - AddVector);
				}

				ItFloor->bTurnCeiling = TurnSet;
			
			}
		}
	}
}

void AMBPFloorActor::CopyBuildingPartsOnSelectedCells(TArray<UBuildingCell*> SelectedCells)
{
	TWeakObjectPtr<UBuildingCell> FirstCell;

	CopiedCells.Empty();

	int32 MinX = SelectedCells[0]->X <= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;
	int32 MaxX = SelectedCells[0]->X >= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;

	int32 MinY = SelectedCells[0]->Y <= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;
	int32 MaxY = SelectedCells[0]->Y >= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;

	for (int32 itX = MinX; itX <= MaxX; itX++)
	{
		for (int32 itY = MinY; itY <= MaxY; itY++)
		{
			if(FirstCell.IsValid() == false)
				FirstCell = ThisGrid.Get(itX, itY);

			bool bWallCheck = (SelectedCopyPart == ESelectBuildingParts::WallAndFloor || SelectedCopyPart == ESelectBuildingParts::Wall);
			bool bFloorCheck = (SelectedCopyPart == ESelectBuildingParts::WallAndFloor || SelectedCopyPart == ESelectBuildingParts::Floor);

			if (bWallCheck && ThisGrid.Get(itX, itY)->GetBuildingWallsOnIt().Num() != 0 || bFloorCheck  && ThisGrid.Get(itX, itY)->GetBuildingFloorsOnIt().Num() != 0)
			{
				CopiedCells.Add(FCopiedCell());

				if (bWallCheck)
					CopiedCells.Top().BuildingWallsOnIt = ThisGrid.Get(itX, itY)->GetBuildingWallsOnIt();

				if (bFloorCheck)
					CopiedCells.Top().BuildingFloorsOnIt = ThisGrid.Get(itX, itY)->GetBuildingFloorsOnIt();

				CopiedCells.Top().DefAddX = ThisGrid.Get(itX, itY)->X - FirstCell->X;
				CopiedCells.Top().DefAddY = ThisGrid.Get(itX, itY)->Y - FirstCell->Y;

				CopiedCells.Top().AddX = CopiedCells.Top().DefAddX;
				CopiedCells.Top().AddY = CopiedCells.Top().DefAddY;
				CopiedCells.Top().SelectedDeletePart = SelectedCopyPart;
			}
		}
	}
}

void AMBPFloorActor::PasteCopiedBuildingParts(UBuildingCell* SelectedCell)
{
	bool bOldOneWallBuildMode = GetOneWallMode();
	SetOneWallMode(false);

	for (FCopiedCell ItCell : CopiedCells)
	{
		TWeakObjectPtr<UBuildingCell> UseCell = ThisGrid.Get(SelectedCell->X + ItCell.AddX, SelectedCell->Y + ItCell.AddY);

		if (UseCell.IsValid())
		{
			DeleteOnCell(UseCell, ItCell.SelectedDeletePart);

			for (TWeakObjectPtr<UBuildingWall> ItWall : ItCell.BuildingWallsOnIt)
			{
				if (ItWall.IsValid())
				{
					bool CreateResult = CreatBuildingPart(
						ItWall->UsedObject,
						0.f,
						FVector::ZeroVector,
						FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.f, 1.f, 1.f)),
						UseCell,
						ESelectBuildingParts::Wall,
						ItWall->GetTypeOfThisWall());

					if (CreateResult)
					{
						FTransform NewTransform = ItWall->GetRelativeTransform();

						FVector ChangeVector = ItWall->GetRelativeLocation() - ItWall->CellLocation;
						NewTransform.SetLocation(BuildingWalls.Top()->CellLocation + ChangeVector);

						BuildingWalls.Top()->SetRelativeTransform(NewTransform);
						BuildingWalls.Top()->SetBuildingPartRotation(BuildingWalls.Top()->GetRelativeRotation() + AddCopiedRotation);
						BuildingWalls.Top()->SetCreateWithOneWallMode(ItWall->GetCreateWithOneWallMode());

						BuildingWalls.Top()->SetArrayOfMaterials(ItWall->GetMaterials());
					}

					if (bIsCutBuildingParts)
						DeleteBuildingWall(ItWall);
				}
			}

			for (TWeakObjectPtr<UBuildingFloor> ItFloor : ItCell.BuildingFloorsOnIt)
			{
				if (ItFloor.IsValid())
				{
					bool CreateResult = CreatBuildingPart(
						ItFloor->UsedObject,
						0.f,
						FVector::ZeroVector,
						FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.f, 1.f, 1.f)),
						UseCell,
						ESelectBuildingParts::Floor);

					if (CreateResult)
					{
						FTransform NewTransform = ItFloor->GetRelativeTransform();

						FVector ChangeVector = ItFloor->GetRelativeLocation() - ItFloor->CellLocation;
						FVector NewLocation = (UKismetMathLibrary::FindLookAtRotation(ItFloor->CellLocation, ItFloor->GetRelativeLocation()) + AddCopiedRotation).Vector() * ChangeVector.Size();
						NewTransform.SetLocation(NewLocation + BuildingFloors.Top()->CellLocation);

						NewTransform.SetRotation((ItFloor->GetRelativeRotation() + AddCopiedRotation).Quaternion());

						BuildingFloors.Top()->SetRelativeTransform(NewTransform);
						BuildingFloors.Top()->bTurnCeiling = ItFloor->bTurnCeiling;
						BuildingFloors.Top()->BuildingFloorHeight = ItFloor->BuildingFloorHeight;

						BuildingFloors.Top()->SetArrayOfMaterials(ItFloor->GetMaterials());
					}

					if (bIsCutBuildingParts)
						DeleteBuildingFloor(ItFloor);
				}
			}			
		}
	}
	
	if (bIsCutBuildingParts)
	{
		bIsCutBuildingParts = false;
		CopiedCells.Empty();
	}

	SetOneWallMode(bOldOneWallBuildMode);
}

void AMBPFloorActor::AllotCellsForCopy(UBuildingCell* SelectedCell)
{
	//Remove Alloted Cells
	for (TWeakObjectPtr<UBuildingCell> ItCell : UseForCopyCells)
	{
		if(ItCell.IsValid())
			ItCell->CreateDynamicMaterialInstance(0)->SetScalarParameterValue(FName("RedGrid"), 0);
	}

	UseForCopyCells.Empty();

	//Allot new Cells
	TWeakObjectPtr<UBuildingCell> CheckCell = SelectedCell;
	if (CheckCell.IsValid())
	{
		for (FCopiedCell ItCell : CopiedCells)
		{
			TWeakObjectPtr<UBuildingCell> UseCell = ThisGrid.Get(SelectedCell->X + ItCell.AddX, SelectedCell->Y + ItCell.AddY);

			if (UseCell.IsValid())
			{
				UseCell->CreateDynamicMaterialInstance(0)->SetScalarParameterValue(FName("RedGrid"), 1);
				UseForCopyCells.Add(UseCell);
			}
		}
	}
}

void AMBPFloorActor::TurnCopiedParts()
{
	AddCopiedRotation += FRotator(0.f, 90.f, 0.f);
	UE_LOG(LogTemp, Log, TEXT("SHIIIT TURN %s"), *AddCopiedRotation.ToString());

	for (int32 it = 0; it < CopiedCells.Num(); it++)
	{
		if (AddCopiedRotation.Equals(FRotator(0.f, 0.f, 0.f), 10.f))
		{
			UE_LOG(LogTemp, Log, TEXT("Equals(FRotator(0.f, 0.f, 0.f)"));
			CopiedCells[it].AddX = CopiedCells[it].DefAddX;
			CopiedCells[it].AddY = CopiedCells[it].DefAddY;
		}
		if (AddCopiedRotation.Equals(FRotator(0.f, 90.f, 0.f), 10.f))
		{
			UE_LOG(LogTemp, Log, TEXT("Equals(FRotator(0.f, 90.f, 0.f)"));
			CopiedCells[it].AddX = CopiedCells[it].DefAddY * -1;
			CopiedCells[it].AddY = CopiedCells[it].DefAddX;
		}
		if (AddCopiedRotation.Equals(FRotator(0.f, 180.f, 0.f), 10.f))
		{
			UE_LOG(LogTemp, Log, TEXT("Equals(FRotator(0.f, 180.f, 0.f)"));
			CopiedCells[it].AddX = CopiedCells[it].DefAddX * -1;
			CopiedCells[it].AddY = CopiedCells[it].DefAddY * -1;
		}
		if (AddCopiedRotation.Equals(FRotator(0.f, 270.f, 0.f), 10.f))
		{
			UE_LOG(LogTemp, Log, TEXT("Equals(FRotator(0.f, 270.f, 0.f)"));
			CopiedCells[it].AddX = CopiedCells[it].DefAddY;
			CopiedCells[it].AddY = CopiedCells[it].DefAddX * -1;
		}
	}
}

void AMBPFloorActor::Debug()
{
	TArray<USceneComponent*> Components;
	TArray<USceneComponent*> DeletedComponents;
	this->GetComponents<USceneComponent>(Components);

	bool bIsValid = false;

	for (USceneComponent* ItComponent : Components)
	{
		TWeakObjectPtr<UBuildingCell> TestItCell = Cast<UBuildingCell>(ItComponent);

		if (TestItCell.IsValid() == false && ItComponent != this->RootComponent && ItComponent != SelecetSpawnParam)
		{
			bIsValid = false;

			//Finding
			for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWalls)
			{
				if (ItWall.IsValid() ? ItWall->GetSceneComponent().IsValid() : true)
				{
					if (ItWall->GetSceneComponent() == ItComponent)
					{
						bIsValid = true;
						break;
					}
				}
				else
					DeferredDeleteAdd(ItWall, ESelectBuildingParts::Wall);
			}
			if (bIsValid == false)
			{
				for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloors)
				{
					if (ItFloor.IsValid() && ItFloor->GetSceneComponent().IsValid())
					{
						if (ItFloor->GetSceneComponent() == ItComponent)
						{
							bIsValid = true;
							break;
						}
					}
					else
						DeferredDeleteAdd(ItFloor, ESelectBuildingParts::Floor);
				}
			}

			//Result
			if (bIsValid == false)
				DeletedComponents.Add(ItComponent);

			DeferredDeleteStart();
		}
	}

	//Deleted
	for (int32 it = 0; it < DeletedComponents.Num(); it++)
	{
		DeletedComponents[it]->UnregisterComponent();
		DeletedComponents[it]->DestroyComponent(true);
	}
	RegisterAllComponents();

	//SpawnGrid();
}

//Material
void AMBPFloorActor::SetWallMaterial(TArray<UBuildingCell*> SelectedCells)
{
	int32 MinX = SelectedCells[0]->X <= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;
	int32 MaxX = SelectedCells[0]->X >= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;

	int32 MinY = SelectedCells[0]->Y <= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;
	int32 MaxY = SelectedCells[0]->Y >= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;

	for (int32 itX = MinX; itX <= MaxX; itX++)
	{
		for (int32 itY = MinY; itY <= MaxY; itY++)
		{
			for (TWeakObjectPtr<UBuildingWall> ItWall : ThisGrid.Get(itX, itY)->GetBuildingWallsOnIt())
			{
				/*ItWall->SetMaterialByIndex(OuterMaterial.Index, OuterMaterial.Material);
				ItWall->SetMaterialByIndex(InnerMaterial.Index, InnerMaterial.Material);*/
				ItWall->SetMaterialBySlotName(OuterMaterial.SlotName, OuterMaterial.Material);
				ItWall->SetMaterialBySlotName(InnerMaterial.SlotName, InnerMaterial.Material);
			}

			if (itX == MinX)
			{
				SetWallMaterialMacros(ThisGrid.Get(itX, itY), FRotator(0.f, 0.f, 0.f));
			}
			if (itX == MaxX)
			{
				SetWallMaterialMacros(ThisGrid.Get(itX, itY), FRotator(0.f, 180.f, 0.f));
			}
			if (itY == MinY)
			{
				SetWallMaterialMacros(ThisGrid.Get(itX, itY), FRotator(0.f, 90.f, 0.f));
			}
			if (itY == MaxY)
			{
				SetWallMaterialMacros(ThisGrid.Get(itX, itY), FRotator(0.f, -90.f, 0.f));
			}
		}
	}
}

void AMBPFloorActor::SetWallMaterialMacros(TWeakObjectPtr<UBuildingCell> SelectedCell, FRotator SelectedRotation)
{
	if (SelectedCell.IsValid())
	{	
		TWeakObjectPtr<UBuildingCell> UseCell = GetCellAroundByDirection(EDirection::Down, SelectedCell, SelectedRotation);
		if (UseCell.IsValid())
		{
			for (TWeakObjectPtr<UBuildingWall> ItWall : UseCell->GetBuildingWallsByRotation(SelectedRotation + FRotator(0.f, 180.f, 0.f)))
			{
				//ItWall->SetMaterialByIndex(OuterMaterial.Index, InnerMaterial.Material);
				ItWall->SetMaterialBySlotName(OuterMaterial.SlotName, InnerMaterial.Material);
			}
		}
	}
}

//Create
void AMBPFloorActor::CreatWallsByObjectsFBM(FObjectsForCreating NewObjects)
{
	if ((NewObjects.SelectedCells[0]->X == NewObjects.SelectedCells[1]->X || NewObjects.SelectedCells[0]->Y == NewObjects.SelectedCells[1]->Y ) && NewObjects.MainObject.NewObject.IsValid())
	{
		bool bEqualX = NewObjects.SelectedCells[0]->X == NewObjects.SelectedCells[1]->X;
		bool bFirstCell = (NewObjects.bSelectOneCell == false) ;
		bool bCreateLeft = false;

		int32 CellX = NewObjects.SelectedCells[0]->X ;
		int32 CellY = NewObjects.SelectedCells[0]->Y ;
		FTransform NewTransform;
		ETypeOfWalls SelectType;
		int32 AddIt = bEqualX ? (NewObjects.SelectedCells[0]->Y < NewObjects.SelectedCells[1]->Y ? 1 : -1) : (NewObjects.SelectedCells[0]->X < NewObjects.SelectedCells[1]->X ? 1 : -1);


		for ( ; ( bEqualX ? CellY : CellX )  != (bEqualX ? NewObjects.SelectedCells[1]->Y : NewObjects.SelectedCells[1]->X) + AddIt;  (bEqualX ? CellY += AddIt : CellX += AddIt))
		{	
			NewTransform = FTransform(FRotator::ZeroRotator, ThisGrid.Get(CellX, CellY)->GetCellLocation(), FVector(1.f, 1.f, 1.f));
		//Creat Rotation
			if (NewObjects.bSelectOneCell)
			{
				if(ThisGrid.Get(CellX, CellY)->GetBuildingWallsOnIt().Num() != 0)
					NewTransform.SetRotation( ThisGrid.Get(CellX, CellY)->GetBuildingWallsOnIt().Top()->GetRelativeRotation().Quaternion());
			}
			else
				NewTransform.SetRotation(
					(UKismetMathLibrary::FindLookAtRotation(NewObjects.SelectedCells[0]->GetCellLocation(), NewObjects.SelectedCells[1]->GetCellLocation()) - FRotator(0.f, 90.f, 0.f)).Quaternion());

		//Check First Cell
			if (bFirstCell && ThisGrid.Get(CellX, CellY)->GetBuildingWallsOnIt().Num())
			{
				FRotator OldRotation = ThisGrid.Get(CellX, CellY)->GetBuildingWallsOnIt().Top()->GetRelativeRotation();
				if ((NewTransform.Rotator() - OldRotation).Equals(FRotator(0.f, -90.f, 0.f), 10.f))
					SelectType = ETypeOfWalls::Left;
				else
					if((NewTransform.Rotator() - OldRotation).Equals(FRotator(0.f, 90.f, 0.f), 10.f))
						SelectType = ETypeOfWalls::Right;
					else  SelectType = ETypeOfWalls::Standard;

					bFirstCell = false;
			}
			else  SelectType = ETypeOfWalls::Standard;

		//Delete
			if ((GetOneWallMode() || NewObjects.LeftObject.NewObject.IsValid() == false) && SelectType == ETypeOfWalls::Standard)
			{
				bool bIsNotDeleted = false, IsFindLeftWall = false;

				for (TWeakObjectPtr<UBuildingWall> ItWall : ThisGrid.Get(CellX, CellY)->GetBuildingWallsOnIt())				
				{
					if (ItWall->GetTypeOfThisWall() == ETypeOfWalls::Left)
						IsFindLeftWall = true;

					if (ItWall->GetRelativeRotation().Equals(NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f)) == false && ItWall->GetTypeOfThisWall() != ETypeOfWalls::Left)
					{
						DeleteSelectedWallOnSelectedCell(ThisGrid.Get(CellX, CellY), ItWall);				
					}
					else
						bIsNotDeleted = true;
				}

				bCreateLeft = (bIsNotDeleted == true && IsFindLeftWall == false);
			}
			else
				DeleteOnCell(ThisGrid.Get(CellX, CellY), ESelectBuildingParts::Wall);

		
			//Check New Wall Size
			if (NewObjects.bSelectOneCell)
			{
				FVector ItBoundSize = GetSizeOfThisObject(NewObjects.MainObject.NewObject) * NewTransform.GetScale3D();
				int32 NumOfDeletingWalls = ItBoundSize.Y / (100.f * GridScale) + 0.1f;

				TWeakObjectPtr<UBuildingCell> SelectedCell = ThisGrid.Get(CellX, CellY),
					EndCell = ThisGrid.Get(CellX, CellY);			

				for (int32 it = 1; it < NumOfDeletingWalls; it++)
				{
					SelectedCell = GetCellAroundByDirection(EDirection::Right, SelectedCell, NewTransform.Rotator());

					if (SelectedCell.IsValid())
					{
						DeleteOnCell(SelectedCell, ESelectBuildingParts::Wall);
						EndCell = SelectedCell;
					}
					else
						break;
				}

				if (NumOfDeletingWalls > 1)
				{
					FVector NewGridLocation = (EndCell->GetCellLocation() + NewTransform.GetLocation()) / 2.f;
					NewTransform.SetLocation(NewGridLocation);
				}
			}
			
		//Create
			switch (SelectType)
			{
			case ETypeOfWalls::Left:
			{
				CreateLeftTypeWall(NewObjects, NewTransform, ThisGrid.Get(CellX, CellY));
				break;
			}
			case ETypeOfWalls::Right:
			{
				NewTransform.SetRotation((NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Quaternion());

				CreatBuildingPart(NewObjects.RightObject.NewObject, 
					NewObjects.RightObject.WallOffset + NewObjects.MainObject.WallOffset,
					NewObjects.RightObject.ObjectChangeVector,
					NewTransform,
					ThisGrid.Get(CellX, CellY),
					ESelectBuildingParts::Wall,
					ETypeOfWalls::Right);

				if (GetOneWallMode() == false && NewObjects.MainObject.WallOffset != 0.f)
				{
					FRotator ToBackCellRotator = NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f);

					//Delete Last Standart Object For Creating New 
					CellX += (int32)ToBackCellRotator.Vector().X;	CellY += (int32)ToBackCellRotator.Vector().Y;	NewTransform.SetLocation(ThisGrid.Get(CellX, CellY)->GetCellLocation());

					if (ThisGrid.Get(CellX, CellY)->GetBuildingWallsByRotation(NewTransform.Rotator()).Num() > 0)
					{
						DeleteOnCellByRotation(ThisGrid.Get(CellX, CellY), NewTransform.Rotator(), ESelectBuildingParts::Wall);

						NewTransform = ShiftInDirection(ThisGrid.Get(CellX, CellY)->GetCellLocation(), NewTransform, NewObjects.MainObject.WallOffset, EDirection::Left, false);		//It must be invert

						CreatBuildingPart(NewObjects.MainObject.NewObject,
							NewObjects.MainObject.WallOffset,
							NewObjects.MainObject.ObjectChangeVector,
							NewTransform,
							ThisGrid.Get(CellX, CellY),
							ESelectBuildingParts::Wall,
							ETypeOfWalls::Standard);
					}

					//Create Next Wall. It's need for compatibility with Right Wall
					CellX -= (int32)ToBackCellRotator.Vector().X;	CellY -= (int32)ToBackCellRotator.Vector().Y;

					bEqualX ? CellY += AddIt : CellX += AddIt;	NewTransform.SetLocation(ThisGrid.Get(CellX, CellY)->GetCellLocation());	NewTransform.SetRotation((NewTransform.Rotator() + FRotator(0.f, 90.f, 0.f)).Quaternion());

					NewTransform = ShiftInDirection(ThisGrid.Get(CellX, CellY)->GetCellLocation(), NewTransform, NewObjects.MainObject.WallOffset, EDirection::Right, false);

					DeleteOnCellByRotation(ThisGrid.Get(CellX, CellY), NewTransform.Rotator(), ESelectBuildingParts::Wall);

					CreatBuildingPart(NewObjects.MainObject.NewObject, 
						NewObjects.MainObject.WallOffset, 
						NewObjects.MainObject.ObjectChangeVector,
						NewTransform, 
						ThisGrid.Get(CellX, CellY),
						ESelectBuildingParts::Wall,
						ETypeOfWalls::Standard);
				}
				break;
			}
			case ETypeOfWalls::Standard:
			{
				if (GetOneWallMode())
					DeleteInOneWallMode(ThisGrid.Get(CellX, CellY), NewTransform.Rotator());	

				CreatBuildingPart(NewObjects.MainObject.NewObject,
					NewObjects.MainObject.WallOffset,
					NewObjects.MainObject.ObjectChangeVector,
					NewTransform,
					ThisGrid.Get(CellX, CellY),
					ESelectBuildingParts::Wall,
					ETypeOfWalls::Standard);
				break;
			}
			default:
				break;
			}

			if (bCreateLeft && GetOneWallMode())
			{	
				NewTransform.SetRotation((NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Quaternion());
				CreateLeftTypeWall(NewObjects, NewTransform, ThisGrid.Get(CellX, CellY), true);
			}

		}

		
	}
}

void AMBPFloorActor::CreateLeftTypeWall(FObjectsForCreating NewObjects, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> ItCell, bool bIsCreateOnlyLeftWall)
{
	NewTransform.SetRotation((NewTransform.Rotator() + FRotator(0.f, 90.f, 0.f)).Quaternion());

	//Left Object Valid. Start Creating with Left Object
	if (NewObjects.LeftObject.NewObject.IsValid())
	{
		//Create Left Wall In Only One Wall Mode.
		if (GetOneWallMode())
		{
			//Creat Transform for Left Wall creating in OneWallMode.
			FTransform OneWallModeTransform = NewTransform;

			FVector ChangeVector = (NewTransform.Rotator().Vector() * (GetSizeOfThisObject(NewObjects.MainObject.NewObject).X * 0.5f))
				+ ((NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Vector() * (GetSizeOfThisObject(NewObjects.MainObject.NewObject).X * 0.5f));

			OneWallModeTransform.SetLocation(NewTransform.GetLocation() - ChangeVector);

			//Chek: Is New Left Wall will overlap with other Walls ? 
			TWeakObjectPtr<UBuildingCell> FirstCell = GetCellAroundByDirection(EDirection::Right, ItCell, NewTransform.Rotator())
				, SecondCell = GetCellAroundByDirection(EDirection::Down, ItCell, NewTransform.Rotator());

			if ((FirstCell.IsValid() ? (FirstCell->GetBuildingWallsByTypeAndRotation(ETypeOfWalls::Standard, NewTransform.Rotator(), true).Num() == 0) : true)
				&&
				(SecondCell.IsValid() ? (SecondCell->GetBuildingWallsByTypeAndRotation(ETypeOfWalls::Standard, NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f), true).Num() == 0) : true))


				CreatBuildingPart(NewObjects.LeftObject.NewObject,
					NewObjects.LeftObject.WallOffset + NewObjects.MainObject.WallOffset,
					NewObjects.LeftObject.ObjectChangeVector,
					OneWallModeTransform,
					ItCell,
					ESelectBuildingParts::Wall,
					ETypeOfWalls::Left);
		}

		//Create Wall In Standart Mode
		else
			CreatBuildingPart(NewObjects.LeftObject.NewObject,
				NewObjects.LeftObject.WallOffset + NewObjects.MainObject.WallOffset,
				NewObjects.LeftObject.ObjectChangeVector,
				NewTransform,
				ItCell,
				ESelectBuildingParts::Wall,
				ETypeOfWalls::Left);
	}

	//Left Object Is Not Valid. Start Create two wall. 
	if ((NewObjects.LeftObject.NewObject.IsValid() == false || GetOneWallMode()) && bIsCreateOnlyLeftWall == false)
	{
		//1 Wall
		if (GetOneWallMode() == false)
			NewTransform = ShiftInDirection(ItCell->GetCellLocation(), NewTransform, NewObjects.MainObject.WallOffset, EDirection::Right);

		CreatBuildingPart(NewObjects.MainObject.NewObject,
			NewObjects.MainObject.WallOffset,
			NewObjects.MainObject.ObjectChangeVector,
			NewTransform,
			ItCell,
			ESelectBuildingParts::Wall,
			ETypeOfWalls::Standard);

		//2 Wall
		NewTransform.SetRotation((NewTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Quaternion());

		if (GetOneWallMode() == false)
			NewTransform = ShiftInDirection(ItCell->GetCellLocation(), NewTransform, NewObjects.MainObject.WallOffset, EDirection::Left);

		CreatBuildingPart(NewObjects.MainObject.NewObject,
			NewObjects.MainObject.WallOffset,
			NewObjects.MainObject.ObjectChangeVector,
			NewTransform,
			ItCell,
			ESelectBuildingParts::Wall,
			ETypeOfWalls::Standard);
	}

}

void AMBPFloorActor::CreatFloorsByComponent(UObject* NewComponent, FVector ChangeVector, TArray<UBuildingCell*> SelectedCells)
{
	bool bIsCeilingCreate = false;

	int32 MinX = SelectedCells[0]->X <= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;
	int32 MaxX = SelectedCells[0]->X >= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;

	int32 MinY = SelectedCells[0]->Y <= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;
	int32 MaxY = SelectedCells[0]->Y >= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;

	for (int32 itX = MinX; itX <= MaxX; itX++)
	{
		for (int32 itY = MinY; itY <= MaxY; itY++)
		{
			DeleteOnCell(ThisGrid.Get(itX, itY) , ESelectBuildingParts::Floor);

			FTransform NewTransform = FTransform(RotationForNewBuildingFloors, ThisGrid.Get(itX, itY)->GetCellLocation(), FVector(1.f, 1.f, 1.f));

			if (NewComponent->GetName().Contains("Ceiling"))
				bIsCeilingCreate = true;

			CreatBuildingPart(NewComponent, 
				NULL,
				ChangeVector,
				NewTransform,
				ThisGrid.Get(itX, itY), 
				ESelectBuildingParts::Floor);
		}
	}

	if (bIsCeilingCreate)
		TurnCeiling(true);
}

bool AMBPFloorActor::CreatBuildingPart(TWeakObjectPtr<UObject> NewComponent , float NewOffset, FVector ChangeVector, FTransform NewTransform , TWeakObjectPtr<UBuildingCell> OnItCell, ESelectBuildingParts SelectPart, ETypeOfWalls SelectTypeOfWall)
{
	UENUM(BlueprintType)
	enum class EObjectType : uint8
	{
		StaticMesh 	UMETA(DisplayName = "StaticMesh"),
		Blueprint 	UMETA(DisplayName = "Blueprint"),
		Actor 	UMETA(DisplayName = "Actor"),
		DestructibleMesh	UMETA(DisplayName = "DestructibleMesh")
	};
	EObjectType NewCreateType;

	if (NewComponent.IsValid())
	{

		TWeakObjectPtr<UStaticMesh> TestStaticMesh = Cast<UStaticMesh>(NewComponent.Get());
		if (TestStaticMesh.IsValid())
			NewCreateType = EObjectType::StaticMesh;

		TWeakObjectPtr<AActor> TestActor = Cast<AActor>(NewComponent.Get());
		if (TestActor.IsValid())
			NewCreateType = EObjectType::Actor;

		TWeakObjectPtr<UBlueprint> TestBlueprint = Cast<UBlueprint>(NewComponent.Get());
		if (TestBlueprint.IsValid())
			NewCreateType = EObjectType::Blueprint;

		TWeakObjectPtr<UDestructibleMesh> TestDestructibleMesh = Cast<UDestructibleMesh>(NewComponent.Get());
		if (TestDestructibleMesh.IsValid())
			NewCreateType = EObjectType::DestructibleMesh;



		switch (SelectPart)
		{
		case  ESelectBuildingParts::Wall:
			if (GetOneWallMode() ? IsOverlapeWithWallAround(OnItCell, NewTransform.Rotator()) == false : true)
			{
				switch (NewCreateType)
				{
				case EObjectType::StaticMesh:
					return CreatWallByStaticMesh(TestStaticMesh.Get(), NewOffset, ChangeVector, NewTransform, OnItCell, SelectTypeOfWall);
					break;

				case EObjectType::Blueprint:
					return CreatWallByBlueprint(TestBlueprint.Get(), NewOffset, ChangeVector, NewTransform, OnItCell, ETypeOfWalls::Standard);
					break;

				case EObjectType::Actor:
					return CreatWallByActor(TestActor.Get(), NewOffset, ChangeVector, NewTransform, OnItCell, ETypeOfWalls::Standard);
					break;
				
				case EObjectType::DestructibleMesh:
					return CreatWallByDestructibleMesh(TestDestructibleMesh.Get(), NewOffset, ChangeVector, NewTransform, OnItCell, SelectTypeOfWall);
					break;
				}					
			}
			break;
		case  ESelectBuildingParts::Floor:
			switch (NewCreateType)
			{
			case EObjectType::StaticMesh:
				return CreatFloorByStaticMesh(TestStaticMesh.Get(), ChangeVector, NewTransform, OnItCell);
				break;

			case EObjectType::Blueprint:
				return CreatFloorByBlueprint(TestBlueprint.Get(), ChangeVector, NewTransform, OnItCell);
				break;

			case EObjectType::Actor:
				return CreatFloorByActor(TestActor.Get(), ChangeVector, NewTransform, OnItCell);
				break;

			case EObjectType::DestructibleMesh:
				return CreatFloorByDestructibleMesh(TestDestructibleMesh.Get(), ChangeVector, NewTransform, OnItCell);
				break;
			}
			break;
		}

	}
	return false;
}

bool AMBPFloorActor::ReplaceSelectedWallToNewWall(TWeakObjectPtr<UBuildingWall> SelectedWall, FObjectsForCreating NewObjects)
{
	if (SelectedWall.IsValid() && NewObjects.MainObject.NewObject.IsValid())
	{
		TWeakObjectPtr<UBuildingCell> UseCell = GetCellByCellLocation(SelectedWall->CellLocation);

		if (UseCell.IsValid())
		{
			FTransform NewTransform = FTransform(SelectedWall->GetRelativeRotation(), UseCell->GetCellLocation(), FVector(1.f, 1.f, 1.f));

			DeleteSelectedWallOnSelectedCell(UseCell, SelectedWall);

			CreatBuildingPart(NewObjects.MainObject.NewObject,
				NewObjects.MainObject.WallOffset,
				NewObjects.MainObject.ObjectChangeVector,
				NewTransform,
				UseCell,
				ESelectBuildingParts::Wall,
				ETypeOfWalls::Standard);

			return true;
		}
	}
	
	return false;
}

//Use Static Mesh
bool AMBPFloorActor::CreatWallByStaticMesh(UStaticMesh* SelectStaticMesh, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType)
{
	TWeakObjectPtr<UStaticMesh> CheckObj = SelectStaticMesh;
	if (CheckObj.IsValid())
	{
		NewTransform = ChangeTransformForWall(SelectStaticMesh->GetBoundingBox(), NewOffset,ChangeVector, NewTransform,  NewType);

		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetRelativeTransform(NewTransform);
		NewComponent->SetStaticMesh(SelectStaticMesh);
		NewComponent->SetMobility(EComponentMobility::Static);

		BuildingWalls.Add(NewObject<UBuildingWall>(this, UBuildingWall::StaticClass()));
		BuildingWalls.Top()->SetNewComponent(NewComponent);
		BuildingWalls.Top()->UsedObject = SelectStaticMesh;
		BuildingWalls.Top()->SetTypeOfThisWall(NewType);
		BuildingWalls.Top()->SetCreateWithOneWallMode(GetOneWallMode());
		if (OnItCell != NULL)
		{
			BuildingWalls.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingWall(BuildingWalls.Top());
		}
		return true;
	}
	return false;
		
}

bool AMBPFloorActor::CreatFloorByStaticMesh(UStaticMesh* SelectStaticMesh, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell)
{
	TWeakObjectPtr<UStaticMesh> CheckObj = SelectStaticMesh;
	if (CheckObj.IsValid())
	{
		FBox ItBox = SelectStaticMesh->GetBoundingBox();
		NewTransform = ChangeTransformForFloor(ItBox, ChangeVector, NewTransform);

		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetRelativeTransform(NewTransform);
		NewComponent->SetStaticMesh(SelectStaticMesh);
		NewComponent->SetMobility(EComponentMobility::Static);

		
		BuildingFloors.Add(NewObject<UBuildingFloor>(this, UBuildingFloor::StaticClass()));
		BuildingFloors.Top()->SetNewComponent(NewComponent);
		BuildingFloors.Top()->UsedObject = SelectStaticMesh;

		BuildingFloors.Top()->Center = ItBox.GetCenter();
		BuildingFloors.Top()->Size = ItBox.GetSize();

		//BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : SelectStaticMesh->GetBoundingBox().GetSize().Z) + (ChangeVector.Z * 2);
		BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : (ItBox.GetSize().Z / 2.f)) + ChangeVector.Z;

		if (OnItCell.IsValid())
		{
			BuildingFloors.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingFloor(BuildingFloors.Top());
		}

		UpdateGridLocation(false, BuildingFloors.Top()->BuildingFloorHeight);
		return true;
	}
	return false;
}

//Use Destructible Mesh
bool AMBPFloorActor::CreatWallByDestructibleMesh(UDestructibleMesh* SelectDestructibleMesh, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType)
{
	if (SelectDestructibleMesh != NULL)
	{
		NewTransform = ChangeTransformForWall(SelectDestructibleMesh->GetBounds().GetBox(), NewOffset, ChangeVector, NewTransform, NewType);

		UDestructibleComponent* NewComponent = NewObject<UDestructibleComponent>(this, UDestructibleComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetRelativeTransform(NewTransform);
		NewComponent->SetDestructibleMesh(SelectDestructibleMesh);
		NewComponent->SetMobility(EComponentMobility::Static);


		BuildingWalls.Add(NewObject<UBuildingWall>(this, UBuildingWall::StaticClass()));
		BuildingWalls.Top()->GetStaticMeshComponent();
		BuildingWalls.Top()->SetNewComponent(NewComponent);
		BuildingWalls.Top()->UsedObject = SelectDestructibleMesh;
		BuildingWalls.Top()->SetTypeOfThisWall(NewType);
		BuildingWalls.Top()->SetCreateWithOneWallMode(GetOneWallMode());
		if (OnItCell != NULL)
		{
			BuildingWalls.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingWall(BuildingWalls.Top());
		}
		return true;
	}
	return false;
}

bool AMBPFloorActor::CreatFloorByDestructibleMesh(UDestructibleMesh* SelectDestructibleMesh, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell)
{
	TWeakObjectPtr<UDestructibleMesh> CheckObj = SelectDestructibleMesh;
	if (CheckObj.IsValid())
	{
		FBox ItBox = SelectDestructibleMesh->GetBounds().GetBox();
		NewTransform = ChangeTransformForFloor(ItBox, ChangeVector, NewTransform);

		UDestructibleComponent* NewComponent = NewObject<UDestructibleComponent>(this, UDestructibleComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetRelativeTransform(NewTransform);
		NewComponent->SetDestructibleMesh(SelectDestructibleMesh);
		NewComponent->SetMobility(EComponentMobility::Static);
		

		BuildingFloors.Add(NewObject<UBuildingFloor>(this, UBuildingFloor::StaticClass()));
		BuildingFloors.Top()->SetNewComponent(NewComponent);
		BuildingFloors.Top()->UsedObject = SelectDestructibleMesh;

		BuildingFloors.Top()->Center = ItBox.GetCenter();
		BuildingFloors.Top()->Size = ItBox.GetSize();

		//BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : ItBox.GetSize().Z) + (ChangeVector.Z * 2);
		BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : (ItBox.GetSize().Z / 2.f)) + ChangeVector.Z;

		if (OnItCell.IsValid())
		{
			BuildingFloors.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingFloor(BuildingFloors.Top());
		}
		UpdateGridLocation(false, BuildingFloors.Top()->BuildingFloorHeight);
		return true;
	}
	return false;
}

//Use Actor
bool AMBPFloorActor::CreatWallByActor(AActor* SelectActor, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType)
{
	TWeakObjectPtr<AActor> CheckObj = SelectActor;
	if (CheckObj.IsValid())
	{
		UChildActorComponent* NewComponent = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());//UActorComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetWorldLocation(FVector::ZeroVector);
		NewComponent->SetChildActorClass(SelectActor->GetClass());
		NewComponent->SetMobility(EComponentMobility::Static);
		NewComponent->RegisterComponent();
		
		NewTransform = ChangeTransformForWall(NewComponent->GetChildActor()->GetComponentsBoundingBox(), NewOffset, ChangeVector, NewTransform, NewType);

		NewComponent->SetRelativeTransform(NewTransform);
		

		BuildingWalls.Add(NewObject<UBuildingWall>(this, UBuildingWall::StaticClass()));
		BuildingWalls.Top()->SetNewComponent(NewComponent);
		BuildingWalls.Top()->UsedObject = SelectActor;
		BuildingWalls.Top()->SetTypeOfThisWall(NewType);
		BuildingWalls.Top()->SetCreateWithOneWallMode(GetOneWallMode());

		if (OnItCell != NULL)
		{
			BuildingWalls.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingWall(BuildingWalls.Top());
		}
		return true;
	}
	return false;
}

bool AMBPFloorActor::CreatFloorByActor(AActor* SelectActor, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell)
{
	TWeakObjectPtr<AActor> CheckObj = SelectActor;
	if (CheckObj.IsValid())
	{
		UChildActorComponent* NewComponent = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetChildActorClass(SelectActor->GetClass());
		NewComponent->SetMobility(EComponentMobility::Static);

		FBox ItBox = NewComponent->GetChildActor()->GetComponentsBoundingBox();
		NewTransform = ChangeTransformForFloor(ItBox, ChangeVector, NewTransform);
		NewComponent->SetRelativeTransform(NewTransform);


		BuildingFloors.Add(NewObject<UBuildingFloor>(this, UBuildingFloor::StaticClass()));
		BuildingFloors.Top()->SetNewComponent(NewComponent);
		BuildingFloors.Top()->UsedObject = SelectActor;

		BuildingFloors.Top()->Center = ItBox.GetCenter();
		BuildingFloors.Top()->Size = ItBox.GetSize();

		//BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : ItBox.GetSize().Z) + (ChangeVector.Z * 2);
		BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : (ItBox.GetSize().Z / 2.f)) + ChangeVector.Z ;

		if (OnItCell.IsValid())
		{
			BuildingFloors.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingFloor(BuildingFloors.Top());
		}

		UpdateGridLocation(false, BuildingFloors.Top()->BuildingFloorHeight);

		return true;
	}
	return false;
}

//Use Blueprint
bool AMBPFloorActor::CreatWallByBlueprint(UBlueprint* SelectBlueprint, float NewOffset, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell, ETypeOfWalls NewType)
{
	TWeakObjectPtr<UBlueprint> CheckObj = SelectBlueprint;
	if (CheckObj.IsValid())
	{
		AActor* SelectedActor = Cast<AActor>(SelectBlueprint->GeneratedClass->GetDefaultObject<UObject>());

		UChildActorComponent* NewComponent = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());//UActorComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetWorldLocation(FVector::ZeroVector);
		NewComponent->SetChildActorClass(SelectedActor->GetClass());
		NewComponent->SetMobility(EComponentMobility::Static);
		NewComponent->RegisterComponent();

		NewTransform = ChangeTransformForWall(NewComponent->GetChildActor()->GetComponentsBoundingBox(), NewOffset, ChangeVector, NewTransform, NewType);

		NewComponent->SetRelativeTransform(NewTransform);


		BuildingWalls.Add(NewObject<UBuildingWall>(this, UBuildingWall::StaticClass()));
		BuildingWalls.Top()->SetNewComponent(NewComponent);
		BuildingWalls.Top()->UsedObject = SelectBlueprint;
		BuildingWalls.Top()->SetTypeOfThisWall(NewType);
		BuildingWalls.Top()->SetCreateWithOneWallMode(GetOneWallMode());

		if (OnItCell != NULL)
		{
			BuildingWalls.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingWall(BuildingWalls.Top());
		}
		return true;
	}
	return false;
}

bool AMBPFloorActor::CreatFloorByBlueprint(UBlueprint* SelectBlueprint, FVector ChangeVector, FTransform NewTransform, TWeakObjectPtr<UBuildingCell> OnItCell)
{
	TWeakObjectPtr<UBlueprint> CheckObj = SelectBlueprint;
	if (CheckObj.IsValid())
	{
		AActor* SelectedActor = Cast<AActor>(SelectBlueprint->GeneratedClass->GetDefaultObject<UObject>());
		
		UChildActorComponent* NewComponent = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
		NewComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		NewComponent->SetWorldLocation(FVector::ZeroVector);
		NewComponent->SetChildActorClass(SelectedActor->GetClass());
		NewComponent->SetMobility(EComponentMobility::Static);
		NewComponent->RegisterComponent();

		FBox ItBox = NewComponent->GetChildActor()->GetComponentsBoundingBox();
		NewTransform = ChangeTransformForFloor(ItBox, ChangeVector, NewTransform);
		NewComponent->SetRelativeTransform(NewTransform);


		BuildingFloors.Add(NewObject<UBuildingFloor>(this, UBuildingFloor::StaticClass()));
		BuildingFloors.Top()->SetNewComponent(NewComponent);
		BuildingFloors.Top()->UsedObject = SelectBlueprint;

		BuildingFloors.Top()->Center = ItBox.GetCenter();
		BuildingFloors.Top()->Size = ItBox.GetSize();

		//BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : ItBox.GetSize().Z) + (ChangeVector.Z * 2);
		BuildingFloors.Top()->BuildingFloorHeight = (GetFloorUnderGridCellMode() ? 0.f : (ItBox.GetSize().Z / 2.f)) + ChangeVector.Z;

		if (OnItCell.IsValid())
		{
			BuildingFloors.Top()->CellLocation = OnItCell->GetCellLocation();
			OnItCell->AddBuildingFloor(BuildingFloors.Top());
		}

		UpdateGridLocation(false, BuildingFloors.Top()->BuildingFloorHeight);
		return true;
	}
	return false;
}

//Change transform
FTransform AMBPFloorActor::ChangeTransformForWall(FBox ItBox, float NewOffset, FVector ChangeVector, FTransform NewTransform, ETypeOfWalls NewType)
{
	FVector BoundCenter, FromRootToCenterOfBound, NewScale, CellBorderLocation;
	FTransform ReturnTransform = NewTransform;

	switch (NewType)
	{
	case ETypeOfWalls::Left:

		if (GetOneWallMode())
		{
			ReturnTransform.SetLocation(GetCellBorderLocation(ReturnTransform, NewOffset, EOrientationByX::RightBottom)
				+ GetLocationByOrientation(ItBox, ChangeVector, ReturnTransform, EOrientationByX::RightBottom));
		}
		else
		{
			NewScale = FVector((100.f * GridScale + NewOffset) / (100.f * GridScale), (100.f * GridScale + NewOffset) / (100.f * GridScale), ReturnTransform.GetScale3D().Z);
			ReturnTransform.SetScale3D(NewScale);
			ReturnTransform.SetLocation(GetCellBorderLocation(ReturnTransform, NewOffset , EOrientationByX::Center)
				+ GetLocationByOrientation(ItBox, ChangeVector, ReturnTransform, EOrientationByX::Center));
		}
		break;
	case ETypeOfWalls::Right:
		ReturnTransform.SetLocation(GetCellBorderLocation(ReturnTransform, NewOffset, EOrientationByX::LeftBottom)
			+ GetLocationByOrientation(ItBox, ChangeVector, ReturnTransform, EOrientationByX::LeftBottom));
		break;
	case ETypeOfWalls::Standard:
		ReturnTransform.SetLocation(GetCellBorderLocation(ReturnTransform, GetOneWallMode() ? 0.f : NewOffset, EOrientationByX::CenterBottom)
			+ GetLocationByOrientation(ItBox, ChangeVector, ReturnTransform, GetOneWallMode() ? EOrientationByX::Center : EOrientationByX::CenterBottom));
		break;
	case  ETypeOfWalls::Nothing:
		break;
	default:
		break;
	}

	return ReturnTransform;
}

FTransform AMBPFloorActor::ChangeTransformForFloor(FBox ItBox, FVector ChangeVector, FTransform NewTransform)
{
	FTransform ReturnTransform = NewTransform;
	FVector BoundCenter = ItBox.GetCenter() + (bFloorUnderGridCell ? FVector(0.f, 0.f, ItBox.GetSize().Z * 0.5f) : FVector::ZeroVector);
	FVector FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + NewTransform.Rotator()).Vector() * BoundCenter.Size();

	ReturnTransform.SetLocation(NewTransform.GetLocation() - FromRootToCenterOfBound + ChangeVector);
	return ReturnTransform;
}

FTransform AMBPFloorActor::ShiftInDirection(FVector FirstLocation, FTransform ItTransform, int32 ItOffset, EDirection ItDirection, bool bForthMeansLarger)
{
	FRotator ShistRotation = ItTransform.Rotator();
	switch (ItDirection)
	{
	case EDirection::Left:
		ShistRotation -= FRotator(0.f, 90.f, 0.f);
		break;
	case EDirection::Right:
		ShistRotation += FRotator(0.f, 90.f, 0.f);
		break;
	default:break;
	}

	ItTransform.SetLocation(FirstLocation + ShistRotation.Vector() *  ItOffset * 0.5f);
	ItTransform.SetScale3D(FVector(ItTransform.GetScale3D().X, (100.f * GridScale + (bForthMeansLarger ? ItOffset : -ItOffset)) / (100.f * GridScale), ItTransform.GetScale3D().Z));
	return ItTransform;
}

//Delete
void AMBPFloorActor::DeleteOnCell(TWeakObjectPtr<UBuildingCell> ItCell , ESelectBuildingParts ItSelectPart, bool bUseDeleteOnlyFromListMode )
{
	if (ItSelectPart == ESelectBuildingParts::Wall)
	{
		for (TWeakObjectPtr<UBuildingWall> ItWall : ItCell->GetBuildingWallsOnIt())
		{
			if (ItWall.IsValid())
			{
				if (bUseDeleteOnlyFromListMode ? (GetDeleteOnlyFromListMode() ? (ListOfObjectToDelete.Find(ItWall->UsedObject) != INDEX_NONE) : true) : true)
				{
					DeleteBuildingWall(ItWall);
					ItCell->DeleteSelectBuildingWallOnIt(ItWall.Get());
				}
			}
		}
	}
	if (ItSelectPart == ESelectBuildingParts::Floor)
	{
		for (TWeakObjectPtr<UBuildingFloor> ItFloor : ItCell->GetBuildingFloorsOnIt())
		{
			if (ItFloor.IsValid())
			{
				DeleteBuildingFloor(ItFloor);
				ItCell->DeleteSelectBuildingFloorOnIt(ItFloor.Get());
			}
		}
	}
	if (ItSelectPart == ESelectBuildingParts::WallAndFloor)
	{
		DeleteOnCell(ItCell, ESelectBuildingParts::Wall);
		DeleteOnCell(ItCell, ESelectBuildingParts::Floor);
	}
}

void AMBPFloorActor::DeleteOnCellByRotation(TWeakObjectPtr<UBuildingCell> ItCell, FRotator ItRotation, ESelectBuildingParts ItSelectPart, bool bUseDeleteOnlyFromListMode)
{
	if (ItCell.IsValid())
	{
		if (ItSelectPart == ESelectBuildingParts::Wall)
		{
			TArray<TWeakObjectPtr<UBuildingWall>> ItWalls = ItCell->GetBuildingWallsByRotation(ItRotation);
			for (TWeakObjectPtr<UBuildingWall> ItWall : ItWalls)
			{
				if (ItWall.IsValid())
				{
					if (bUseDeleteOnlyFromListMode ? (GetDeleteOnlyFromListMode() ? (ListOfObjectToDelete.Find(ItWall->UsedObject) != INDEX_NONE) : true) : true)
					{
						DeleteBuildingWall(ItWall);
						ItCell->DeleteSelectBuildingWallOnIt(ItWall.Get());
					}
				}
			}
		}
		if (ItSelectPart == ESelectBuildingParts::Floor)
		{
			TArray<TWeakObjectPtr<UBuildingFloor>> ItFloors = ItCell->GetBuildingFloorsByRotation(ItRotation);
			for (TWeakObjectPtr<UBuildingFloor> ItFloor : ItFloors)
			{
				if (ItFloor.IsValid() ? (GetDeleteOnlyFromListMode() ? (ListOfObjectToDelete.Find(ItFloor->UsedObject) != INDEX_NONE) : true) : false)
				{
					DeleteBuildingFloor(ItFloor);
					ItCell->DeleteSelectBuildingFloorOnIt(ItFloor.Get());
				}
				if (ItFloor.IsValid() == false) break;
			}
		}
		if (ItSelectPart == ESelectBuildingParts::WallAndFloor)
		{
			DeleteOnCellByRotation(ItCell, ItRotation, ESelectBuildingParts::Wall);
			DeleteOnCellByRotation(ItCell, ItRotation, ESelectBuildingParts::Floor);
		}
	}
}

void AMBPFloorActor::DeleteWallsOnCellByTypeAndRotation(TWeakObjectPtr<UBuildingCell> ItCell, ETypeOfWalls SelectedType, FRotator ItRotation, bool bUseDeleteOnlyFromListMode)
{

	TArray<TWeakObjectPtr<UBuildingWall>> ItWalls = ItCell->GetBuildingWallByTypeOfWalls(SelectedType);
	for (TWeakObjectPtr<UBuildingWall> ItWall : ItWalls)
	{
		if (ItWall.IsValid())
		{
			if ((bUseDeleteOnlyFromListMode ? (GetDeleteOnlyFromListMode() ? (ListOfObjectToDelete.Find(ItWall->UsedObject) != INDEX_NONE) : true) : true) 
				&& ItWall->GetRelativeRotation().Equals(ItRotation, 5))
			{
				DeleteBuildingWall(ItWall);
				ItCell->DeleteSelectBuildingWallOnIt(ItWall.Get());
			}
		}
	}	
}

void AMBPFloorActor::DeleteSelectedWallOnSelectedCell(TWeakObjectPtr<UBuildingCell> ItCell, TWeakObjectPtr<UBuildingWall> ItWall)
{
	if (ItCell.IsValid() && ItWall.IsValid())
	{
		ItCell->DeleteSelectBuildingWallOnIt(ItWall);
		DeleteBuildingWall(ItWall);
	}
}

void AMBPFloorActor::DeleteBuildingWall(TWeakObjectPtr<UBuildingWall> DeletingWall)
{
	if (DeletingWall.IsValid())
	{
		DeletingWall->DestroyComponent();
	}
	BuildingWalls.RemoveAt(BuildingWalls.Find(DeletingWall.Get()));
}

void AMBPFloorActor::DeleteBuildingFloor(TWeakObjectPtr<UBuildingFloor> DeletingFloor)
{
	if (DeletingFloor.IsValid())
	{
		DeletingFloor->DestroyComponent();
		BuildingFloors.RemoveAt(BuildingFloors.Find(DeletingFloor.Get()));

		UpdateGridLocation();
	}
}

void AMBPFloorActor::SetNewListOfObjectToDelete(TArray<TWeakObjectPtr<UObject>> NewList)
{
	ListOfObjectToDelete = NewList;

	
}

void AMBPFloorActor::DeleteAllWallsAndFloors()
{
	for (int32 itX = 0; itX < GridX; itX++)
	{
		for (int32 itY = 0; itY < GridY; itY++)
		{
			DeleteOnCell(ThisGrid.Get(itX, itY) , ESelectBuildingParts::WallAndFloor);
		}
	}
}

void AMBPFloorActor::DeleteOnSelectedCells(ESelectBuildingParts SelectPart , TArray<UBuildingCell*> SelectedCells)
{
	int32 MinX = SelectedCells[0]->X <= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;
	int32 MaxX = SelectedCells[0]->X >= SelectedCells[1]->X ? SelectedCells[0]->X : SelectedCells[1]->X;

	int32 MinY = SelectedCells[0]->Y <= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;
	int32 MaxY = SelectedCells[0]->Y >= SelectedCells[1]->Y ? SelectedCells[0]->Y : SelectedCells[1]->Y;

	for (int32 itX = MinX; itX <= MaxX; itX++)
	{
		for (int32 itY = MinY; itY <= MaxY; itY++)
		{
			DeleteOnCell(ThisGrid.Get(itX, itY), SelectPart);
		}
	}
}

void AMBPFloorActor::DeferredDeleteAdd(TWeakObjectPtr<UBuildingPart> NewPart, ESelectBuildingParts SelectPart)
{
	if (NewPart.IsValid())
	{
		if (SelectPart == ESelectBuildingParts::Wall)
			DeferredDeleteWalls.Add(Cast<UBuildingWall>(NewPart.Get()));
		if (SelectPart == ESelectBuildingParts::Floor)
			DeferredDeleteFloors.Add(Cast<UBuildingFloor>(NewPart.Get()));
	}
}

void AMBPFloorActor::DeferredDeleteStart()
{
	for (TWeakObjectPtr<UBuildingWall> ItWall : DeferredDeleteWalls)
	{
		if (ItWall.IsValid())
			DeleteBuildingWall(ItWall);
	}
	DeferredDeleteWalls.Empty();
	for (TWeakObjectPtr<UBuildingFloor> ItFloor : DeferredDeleteFloors)
	{
		if (ItFloor.IsValid())
			DeleteBuildingFloor(ItFloor);
	}
	DeferredDeleteFloors.Empty();
}

void AMBPFloorActor::DeleteLastWall()
{
	if (BuildingWalls.Num() > 0)
	{
		TArray<UBuildingCell*> Components;
		this->GetComponents<UBuildingCell>(Components);
		for (UBuildingCell* itCell : Components)
		{
			if (itCell->RelativeLocation.Equals(BuildingWalls.Top()->CellLocation, 5.f))
			{
				itCell->DeleteSelectBuildingWallOnIt(BuildingWalls.Top());
				DeleteBuildingWall(BuildingWalls.Top());
				break;
			}
		}
	}
}

void AMBPFloorActor::DeleteInOneWallMode(TWeakObjectPtr<UBuildingCell> SelectedCell, FRotator ItRotation)
{
	//First Step
	TWeakObjectPtr<UBuildingCell> UseCell = GetCellAroundByDirection(EDirection::Left, SelectedCell, ItRotation);

	if (UseCell.IsValid())
		DeleteWallsOnCellByTypeAndRotation(UseCell, ETypeOfWalls::Left, ItRotation);

	UseCell = GetCellAroundByDirection(EDirection::Down, UseCell, ItRotation);

	if (UseCell.IsValid())
		DeleteWallsOnCellByTypeAndRotation(UseCell, ETypeOfWalls::Left, ItRotation - FRotator(0.f, 90.f, 0.f));

	//Second Step
	UseCell = GetCellAroundByDirection(EDirection::Right, SelectedCell, ItRotation);

	if (UseCell.IsValid())
		DeleteWallsOnCellByTypeAndRotation(UseCell, ETypeOfWalls::Left, ItRotation + FRotator(0.f, 90.f, 0.f));

	UseCell = GetCellAroundByDirection(EDirection::Down, UseCell, ItRotation);

	if (UseCell.IsValid())
		DeleteWallsOnCellByTypeAndRotation(UseCell, ETypeOfWalls::Left, ItRotation + FRotator(0.f, 180.f, 0.f));
}

bool AMBPFloorActor::DeleteBuildingPartBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent)
{
	if (ItComponent.IsValid() && ItComponent != this->RootComponent && ItComponent != SelecetSpawnParam)
	{

		TWeakObjectPtr<UBuildingWall> DeletingWall = GetBuildingWallBySceneComponent(ItComponent);
		if (DeletingWall.IsValid())
		{
			DeleteBuildingWall(DeletingWall);
			return true;
		}

		TWeakObjectPtr<UBuildingFloor> DeletingFloor = GetBuildingFloorBySceneComponent(ItComponent);
		if (DeletingFloor.IsValid())
		{
			DeleteBuildingFloor(DeletingFloor);
			return true;
		}
		
		return false;
	}
	else return true;
}

//Get
FVector AMBPFloorActor::GetLocationByOrientation(FBox ItBox, FVector ChangeVector, FTransform ItTransform, EOrientationByX SelectOrientation)
{
	FVector BoundCenter, FromRootToCenterOfBound, Center, Size;
	Center = ItBox.GetCenter() * ItTransform.GetScale3D() - ChangeVector; Size = ItBox.GetSize() * ItTransform.GetScale3D();

	switch (SelectOrientation)
	{
	case EOrientationByX::Center:
		BoundCenter = Center - FVector(0.f, 0.f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::CenterBottom:
		BoundCenter = Center - FVector(Size.X * 0.5f, 0.f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return  ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::CenterTop:
		BoundCenter = Center - FVector(Size.X * 0.5f, 0.f, Size.Z * -0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::LeftBottom:
		BoundCenter = Center - FVector(Size.X * 0.5f, Size.Y * 0.5f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::RightBottom:
		BoundCenter = Center - FVector(Size.X * 0.5f, Size.Y * -0.5f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return  ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::LeftTop:
		BoundCenter = Center - FVector(Size.X * -0.5f, Size.Y * 0.5f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	case EOrientationByX::RightTop:
		BoundCenter = Center + FVector(Size.X * 0.5f, Size.Y * 0.5f, Size.Z * 0.5f);
		FromRootToCenterOfBound = (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BoundCenter) + ItTransform.Rotator()).Vector() * BoundCenter.Size();
		return  ItTransform.GetLocation() - FromRootToCenterOfBound;
		break;
	default:
		break;
	}
	return FVector::ZeroVector;
}

FVector AMBPFloorActor::GetCellBorderLocation(FTransform ItTransform, float ItOffset, EOrientationByX SelectOrientation)
{
	switch (SelectOrientation)
	{
	case EOrientationByX::Center:
		return (ItTransform.Rotator().Vector()* -1 * (ItOffset * 0.5f)) + ((ItTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Vector() * -1 * (ItOffset * 0.5f));
		//	return FVector::ZeroVector;
		break;
	case EOrientationByX::CenterBottom:
		return ItTransform.Rotator().Vector()* -1 * (GridScale * 50.f + ItOffset);
		break;
	case EOrientationByX::CenterTop:
		return ItTransform.Rotator().Vector() *(GridScale * 50.f + ItOffset);
		break;
	case EOrientationByX::LeftBottom:
		return ItTransform.Rotator().Vector() * -1 * (GridScale * 50.f + ItOffset) + (ItTransform.Rotator() + FRotator(0.f, 90.f, 0.f)).Vector() * -1 * (GridScale * 50.f + ItOffset);
		break;
	case EOrientationByX::RightBottom:
		return ItTransform.Rotator().Vector()* -1 * (GridScale * 50.f + ItOffset) + (ItTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Vector()* -1 * (GridScale * 50.f + ItOffset);
		break;
	case EOrientationByX::LeftTop:
		return ItTransform.Rotator().Vector() *(GridScale * 50.f + ItOffset) + (ItTransform.Rotator() - FRotator(0.f, 90.f, 0.f)).Vector() *(GridScale * 50.f + ItOffset);
		break;
	case EOrientationByX::RightTop:
		return ItTransform.Rotator().Vector() *(GridScale * 50.f + ItOffset) + (ItTransform.Rotator() + FRotator(0.f, 90.f, 0.f)).Vector() *(GridScale * 50.f + ItOffset);
		break;
	default:
		break;
	}
	return FVector::ZeroVector;
}

ETypeOfWalls  AMBPFloorActor::GetTypeOfThisObject(TWeakObjectPtr<UObject> ItObject)
{
	if (ItObject->GetName().Contains(FString("Left")))
		return ETypeOfWalls::Left;
	else
		if (ItObject->GetName().Contains(FString("Right")))
			return ETypeOfWalls::Right;
		else return ETypeOfWalls::Standard;
}

FVector AMBPFloorActor::GetSizeOfThisObject(TWeakObjectPtr<UObject> ItObject)
{
	if (ItObject.IsValid())
	{
		TWeakObjectPtr<UStaticMesh> TestStaticMesh = Cast<UStaticMesh>(ItObject.Get());
		if (TestStaticMesh.IsValid())
		{
			return TestStaticMesh->GetBoundingBox().GetSize();
		}
		TWeakObjectPtr<UBlueprint> TestBlueprint = Cast<UBlueprint>(ItObject.Get());
		if (TestBlueprint.IsValid())
		{
			UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
			FVector const ObjLocation = FVector::ZeroVector;
			AActor* ItActor = (AActor*)MyWorld->SpawnActor(TestBlueprint.Get()->GeneratedClass, &ObjLocation);

			FVector Size = ItActor->GetComponentsBoundingBox(true).GetSize();

			ItActor->K2_DestroyActor();

			return Size;
		}
		TWeakObjectPtr<AActor> TestActor = Cast<AActor>(ItObject.Get());
		if (TestActor.IsValid())
		{
			UWorld* MyWorld = GEditor->GetEditorWorldContext().World();
			FVector const ObjLocation = FVector::ZeroVector;
			AActor* ItActor = (AActor*)MyWorld->SpawnActor(TestActor->StaticClass(), &ObjLocation);

			FVector Size = ItActor->GetComponentsBoundingBox(true).GetSize();

			ItActor->K2_DestroyActor();

			return Size;
		}
		TWeakObjectPtr<UDestructibleMesh> TestObj = Cast<UDestructibleMesh>(ItObject.Get());
		if (TestObj.IsValid())
		{
			return TestObj->GetBounds().GetBox().GetSize();
		}
	}
	return FVector::ZeroVector;
}

TWeakObjectPtr<UBuildingCell> AMBPFloorActor::GetCellAroundByDirection(EDirection SelectedDirection, TWeakObjectPtr<UBuildingCell> SelectedCell, FRotator RotationOfRoot)
{
	if (SelectedCell.IsValid())
	{
		int32 LocalCellX = SelectedCell->X, LocalCellY = SelectedCell->Y;

		switch (SelectedDirection)
		{
		case EDirection::Left:
			RotationOfRoot -= FRotator(0.f, 90.f, 0.f);
			break;
		case EDirection::Right:
			RotationOfRoot += FRotator(0.f, 90.f, 0.f);
			break;
		case EDirection::Up:	
			break;
		case EDirection::Down:
			RotationOfRoot += FRotator(0.f, 180.f, 0.f);
			break;
		default:
			break;
		}
		FVector ChangeVector = RotationOfRoot.Vector() * 1.1f;
		LocalCellX += (int32)ChangeVector.X;	LocalCellY += (int32)ChangeVector.Y;
		return ThisGrid.Get(LocalCellX , LocalCellY);
	}
	return NULL;
}

TWeakObjectPtr<UBuildingCell> AMBPFloorActor::GetCellByCellLocation(FVector CellLocation)
{
	TArray<UBuildingCell*> Components;
	this->GetComponents<UBuildingCell>(Components);

	for (UBuildingCell* itCell : Components)
	{
		if(itCell->GetCellLocation() == CellLocation)
			return itCell;
	}
	
	return NULL;
}

TWeakObjectPtr<UBuildingWall> AMBPFloorActor::GetBuildingWallBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent)
{
	if (ItComponent.IsValid())
	{
		TWeakObjectPtr<UBuildingCell> TestItCell = Cast<UBuildingCell>(ItComponent.Get());

		if (TestItCell.IsValid() == false && ItComponent != this->RootComponent && ItComponent != SelecetSpawnParam)
		{
			for (TWeakObjectPtr<UBuildingWall> ItWall : BuildingWalls)
			{
				if (ItWall.IsValid() ? ItWall->GetSceneComponent().IsValid() : true)
				{
					if (ItWall->GetSceneComponent() == ItComponent)
					{
						return ItWall;
					}
				}
			}
		}
	}
	return NULL;
}

TWeakObjectPtr<UBuildingFloor> AMBPFloorActor::GetBuildingFloorBySceneComponent(TWeakObjectPtr<USceneComponent> ItComponent)
{
	if (ItComponent.IsValid())
	{
		TWeakObjectPtr<UBuildingCell> TestItCell = Cast<UBuildingCell>(ItComponent.Get());

		if (TestItCell.IsValid() == false && ItComponent != this->RootComponent && ItComponent != SelecetSpawnParam)
		{
			for (TWeakObjectPtr<UBuildingFloor> ItFloor : BuildingFloors)
			{
				if (ItFloor.IsValid() ? ItFloor->GetSceneComponent().IsValid() : true)
				{
					if (ItFloor->GetSceneComponent() == ItComponent)
					{
						return ItFloor;
					}
				}
			}
		}
	}
	return NULL;
}

//Mode
bool AMBPFloorActor::GetDeleteOnlyFromListMode()
{
	return bDeleteOnlyFromList;
}

void AMBPFloorActor::SetDeleteOnlyFromListMode(bool bNewDeleteOnlyFromList)
{
	if (bNewDeleteOnlyFromList)
		SetOneWallMode(false);

	bDeleteOnlyFromList = bNewDeleteOnlyFromList;
}

bool AMBPFloorActor::GetOneWallMode()
{
	return bOneWallMode;
}

void AMBPFloorActor::SetOneWallMode(bool bNewOneWallMode)
{
	if(bNewOneWallMode)
		SetDeleteOnlyFromListMode(false);

	bOneWallMode = bNewOneWallMode;
}

bool AMBPFloorActor::GetFloorUnderGridCellMode()
{
	return bFloorUnderGridCell;
}

void AMBPFloorActor::SetFloorUnderGridCellMode(bool bNewFloorUnderGridCell)
{
	bFloorUnderGridCell = bNewFloorUnderGridCell;
}

EBuildingMode AMBPFloorActor::GetSelectedBuildingMode()
{
	return SelectedBuildingMode;
}

void AMBPFloorActor::SetBuildingMode(EBuildingMode NewBuildingMode)
{
	SelectedBuildingMode = NewBuildingMode;
}

//Castom
bool AMBPFloorActor::IsOverlapeWithWallAround(TWeakObjectPtr<UBuildingCell> ItCell, FRotator ItRotator)
{
	if (ItCell.IsValid())
	{
		TWeakObjectPtr<UBuildingCell> UseCell =  GetCellAroundByDirection(EDirection::Down, ItCell, ItRotator);

		if (UseCell.IsValid())
		{
			if(UseCell->GetBuildingWallsByRotation(ItRotator + FRotator(0.f, 180.f, 0.f), true).Num() > 0)
				return true;
		}
	}
	return false;
}

//Vision
void AMBPFloorActor::SetNewVision(ESelectBuildingParts SelectParts, bool bNewVision)
{
	if (SelectParts == ESelectBuildingParts::Grid)
	{
		TArray<UBuildingCell*> Components;
		this->GetComponents<UBuildingCell>(Components);

		for (UBuildingCell* itCell : Components)
		{
			itCell->SetVisibility(bNewVision);
		}

	}
	if (SelectParts == ESelectBuildingParts::WallAndFloor)
	{
		for (UBuildingWall* ItWall : BuildingWalls)
		{
			ItWall->SetNewVision(bNewVision);
		}
		for (UBuildingFloor* ItFloor : BuildingFloors)
		{
			ItFloor->SetNewVision(bNewVision);
		}

	}

	if (SelectParts == ESelectBuildingParts::All)
	{
		SetNewVision(ESelectBuildingParts::Grid, bNewVision);
		SetNewVision(ESelectBuildingParts::WallAndFloor, bNewVision);
	}
	RegisterAllComponents();
}


