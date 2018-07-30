// Fill out your copyright notice in the Description page of Project Settings.

#include "../Private/ModularBuildingPluginPrivatePCH.h"
#include "BuildingPart.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"


//Set
void UBuildingPart::SetNewComponent(TWeakObjectPtr<USceneComponent> NewComponent)
{
	if (NewComponent.IsValid())
	{
		ThisComponent = NewComponent.Get();
		ThisComponent->RegisterComponent();
	}
}

void UBuildingPart::SetNewVision(bool bNewVision)
{
	if (GetSceneComponent().IsValid())
		GetSceneComponent()->SetVisibility(bNewVision, true);
	
}
	//to change rotation, relative to cells
void UBuildingPart::SetBuildingPartRotation(FRotator NewRotator)
{
	if (GetSceneComponent().IsValid())
	{
		FRotator ChangeRotator = UKismetMathLibrary::FindLookAtRotation(CellLocation, GetSceneComponent()->RelativeLocation) + (NewRotator - GetSceneComponent()->RelativeRotation );

		GetSceneComponent()->SetRelativeLocation(ChangeRotator.Vector() *  (GetSceneComponent()->RelativeLocation - CellLocation).Size() + CellLocation);
		GetSceneComponent()->SetRelativeRotation(NewRotator);
	}
}
	//Use Cell Location like NewLocation
void UBuildingPart::SetBuildingPartLocation(FVector NewLocation)
{
	if (GetSceneComponent().IsValid())
	{
		FVector ChangeVector = GetSceneComponent()->RelativeLocation - CellLocation;
		GetSceneComponent()->SetRelativeLocation(NewLocation + ChangeVector);
	}
}

void UBuildingPart::SetRelativeRotation(FRotator NewRotator)
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->SetRelativeRotation(NewRotator);
	}
}

void UBuildingPart::SetRelativeLocation(FVector NewLocation)
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->SetRelativeLocation(NewLocation);
	}
}

void UBuildingPart::SetRelativeScale(FVector NewScale)
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->SetRelativeScale3D(NewScale);
	}
}

void UBuildingPart::SetRelativeTransform(FTransform NewTransform)
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->SetRelativeTransform(NewTransform);
	}
}

void UBuildingPart::SetCellsLocationsForInstancedComponents(TArray<FVector> ItCellsLocation)
{
	CellsLocationsForInstancedComponents = ItCellsLocation;
}

void UBuildingPart::AddVectorToLocation(FVector ThisVector)
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->SetRelativeLocation(GetSceneComponent()->RelativeLocation + ThisVector);
	}

}

void UBuildingPart::SetMaterialByIndex(int32 SlotIndex, TWeakObjectPtr<UMaterialInterface> NewMaterial)
{
	if (GetPrimitiveComponent().IsValid()  && NewMaterial.IsValid())
	{
		if (GetPrimitiveComponent()->GetNumMaterials() > SlotIndex)
			GetPrimitiveComponent()->SetMaterial( SlotIndex, NewMaterial.Get());
	}

	if (GetChildActorComponent().IsValid() && NewMaterial.IsValid())
	{
		TArray<UPrimitiveComponent*> ItComponents;
		GetChildActorComponent()->GetChildActor()->GetComponents<UPrimitiveComponent>(ItComponents);

		for(TWeakObjectPtr<UPrimitiveComponent> ItPrimitiveComponent : ItComponents)
		{
			if (ItPrimitiveComponent.IsValid())
			{
				if (ItPrimitiveComponent->GetNumMaterials() > SlotIndex && ItPrimitiveComponent->GetName().Contains(FString("Wall")) )
				{
					ItPrimitiveComponent->SetMaterial(SlotIndex, NewMaterial.Get());
					break;
				}
			}
		}
	}
}
	
void UBuildingPart::SetMaterialBySlotName(FString SlotNane, TWeakObjectPtr<UMaterialInterface> NewMaterial)
{
	if (NewMaterial.IsValid())
	{
		if (GetPrimitiveComponent().IsValid())
		{
			for (int32 it = 0; it < GetSlotNamesOfPrimitiveComponent(GetPrimitiveComponent()).Num(); it++)
			{
				if (GetSlotNamesOfPrimitiveComponent(GetPrimitiveComponent())[it].ToString().Contains(SlotNane))
					GetPrimitiveComponent()->SetMaterial(it, NewMaterial.Get());
			}
		}

		if (GetChildActorComponent().IsValid())
		{
			TArray<UPrimitiveComponent*> ItComponents;
			GetChildActorComponent()->GetChildActor()->GetComponents<UPrimitiveComponent>(ItComponents);

			for (TWeakObjectPtr<UPrimitiveComponent> ItPrimitiveComponent : ItComponents)
			{
				if (ItPrimitiveComponent.IsValid() ? ItPrimitiveComponent->GetName().Contains(FString("Wall")) : false)
				{
					for (int32 it = 0; it < GetSlotNamesOfPrimitiveComponent(ItPrimitiveComponent).Num(); it++)
					{
						if (GetSlotNamesOfPrimitiveComponent(ItPrimitiveComponent)[it].ToString().Contains(SlotNane))
							ItPrimitiveComponent->SetMaterial(it, NewMaterial.Get());
					}
					break;
				}
			}
			/*TArray<UDestructibleComponent*> ItDestructibleComponents;
			GetChildActorComponent()->GetChildActor()->GetComponents<UDestructibleComponent>(ItDestructibleComponents);

			for (TWeakObjectPtr<UDestructibleComponent> ItDestructibleComponent : ItDestructibleComponents)
			{
				if (ItDestructibleComponent.IsValid() ? ItDestructibleComponent->GetName().Contains(FString("Wall")) : false)
				{
					SetDestructibleComponentMaterialBySlotName(ItDestructibleComponent, SlotNane, NewMaterial);
					break;
				}
			}

			TArray<UStaticMeshComponent*> ItStaticMeshComponents;
			GetChildActorComponent()->GetChildActor()->GetComponents<UStaticMeshComponent>(ItStaticMeshComponents);

			for (TWeakObjectPtr<UStaticMeshComponent> ItStaticMeshComponent : ItStaticMeshComponents)
			{
				if (ItStaticMeshComponent.IsValid() ? ItStaticMeshComponent->GetName().Contains(FString("Wall")) : false)
				{
					UE_LOG(LogTemp, Log, TEXT("TRY~~~~~~~~~~~~~~"));
					SetStaticMeshComponentMaterialBySlotName(ItStaticMeshComponent, SlotNane, NewMaterial);
					break;
				}
			}*/

		}
	}
}

void UBuildingPart::SetArrayOfMaterials(TArray<TWeakObjectPtr<UMaterialInterface>> NewMaterials)
{
	for (int32 it = 0; it < NewMaterials.Num(); it++)
	{
		SetMaterialByIndex(it, NewMaterials[it]);
	}
}

//Delete
void UBuildingPart::DestroyComponent()
{
	if (GetSceneComponent().IsValid())
	{
		GetSceneComponent()->UnregisterComponent();
		GetSceneComponent()->DestroyComponent(true);
	}
}

//Get
FRotator UBuildingPart::GetRelativeRotation()
{
	if (GetSceneComponent().IsValid())
		return GetSceneComponent()->RelativeRotation;
	return FRotator::ZeroRotator;
}

FVector UBuildingPart::GetRelativeLocation()
{
	if (GetSceneComponent().IsValid())
		return GetSceneComponent()->RelativeLocation;

	return FVector::ZeroVector;
}

FTransform UBuildingPart::GetRelativeTransform()
{
	if (GetSceneComponent().IsValid())
		return GetSceneComponent()->GetRelativeTransform();

	return FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.f, 1.f, 1.f));
}
	// Main type
TWeakObjectPtr<USceneComponent>  UBuildingPart::GetSceneComponent()
{
	TWeakObjectPtr<USceneComponent> ThisSceneComponent = ThisComponent;

	if (ThisSceneComponent.IsValid())
		return ThisSceneComponent;
	else return NULL;
}

TWeakObjectPtr<UDestructibleComponent> UBuildingPart::GetDestructibleComponent()
{
	TWeakObjectPtr<UDestructibleComponent> ThisDestructibleComponent = Cast<UDestructibleComponent>(ThisComponent);

	if (ThisDestructibleComponent.IsValid())
		return ThisDestructibleComponent;
	else return NULL;
}

TWeakObjectPtr<UPrimitiveComponent> UBuildingPart::GetPrimitiveComponent()
{
	TWeakObjectPtr<UPrimitiveComponent> ThisPrimitiveComponent = Cast<UPrimitiveComponent>(ThisComponent);

	if (ThisPrimitiveComponent.IsValid())
		return ThisPrimitiveComponent;
	else return NULL;
}

TWeakObjectPtr<UStaticMeshComponent> UBuildingPart::GetStaticMeshComponent()
{
	TWeakObjectPtr<UStaticMeshComponent> ThisStaticMeshComponent = Cast<UStaticMeshComponent>( ThisComponent);

	if (ThisStaticMeshComponent.IsValid())
		return ThisStaticMeshComponent;
	else return NULL;
}

TWeakObjectPtr<UInstancedStaticMeshComponent> UBuildingPart::GetInstancedStaticMeshComponent()
{
	//if (ThisComponent.GetInstancedStaticMeshComponent().IsValid())
	//	return ThisComponent.GetInstancedStaticMeshComponent();
	TWeakObjectPtr<UInstancedStaticMeshComponent> ThisInstancedStaticMeshComponent = Cast<UInstancedStaticMeshComponent>(ThisComponent);
	if (ThisInstancedStaticMeshComponent.IsValid())
		return ThisInstancedStaticMeshComponent;
	else return NULL;
}

TWeakObjectPtr<UChildActorComponent> UBuildingPart::GetChildActorComponent()
{
	TWeakObjectPtr<UChildActorComponent> ReturnActor = Cast<UChildActorComponent>(ThisComponent);
	if (ReturnActor.IsValid())
		return ReturnActor;
	else return  NULL;
}

TArray<FVector> UBuildingPart::GetCellsLocationsForInstancedComponents()
{
	//return ThisComponent.CellsLocationsForInstancedComponents ;
	return CellsLocationsForInstancedComponents;
}

TArray<TWeakObjectPtr<UMaterialInterface>> UBuildingPart::GetMaterials()
{
	TArray<TWeakObjectPtr<UMaterialInterface>> ReturnMaterials;

	if (GetPrimitiveComponent().IsValid() )
	{
		for (int32 it = 0; it < GetPrimitiveComponent()->GetNumMaterials(); it++)
		{
			ReturnMaterials.Add(GetPrimitiveComponent()->GetMaterial(it));
		}
		return ReturnMaterials;
	}

	if (GetChildActorComponent().IsValid() )
	{
		TArray<UPrimitiveComponent*> ItComponents;
		GetChildActorComponent()->GetChildActor()->GetComponents<UPrimitiveComponent>(ItComponents);

		for (TWeakObjectPtr<UPrimitiveComponent> ItPrimitiveComponent : ItComponents)
		{
			if (ItPrimitiveComponent.IsValid() ? (ItPrimitiveComponent->GetName().Contains(FString("Wall"))) : false)
			{
				for (int32 it = 0; it < ItPrimitiveComponent->GetNumMaterials(); it++)
				{
					ReturnMaterials.Add(ItPrimitiveComponent->GetMaterial(it));
				}
				return ReturnMaterials;
			}
		}
	}
	return ReturnMaterials;
}

TArray<FName> UBuildingPart::GetSlotNamesOfPrimitiveComponent(TWeakObjectPtr<UPrimitiveComponent> ItPrimitiveComponent)
{
	if (ItPrimitiveComponent.IsValid())
	{
		TWeakObjectPtr<UStaticMeshComponent> ThisStaticMeshComponent = Cast<UStaticMeshComponent>(ItPrimitiveComponent.Get());
		if (ThisStaticMeshComponent.IsValid())
			return ThisStaticMeshComponent->GetMaterialSlotNames();

		TWeakObjectPtr<UDestructibleComponent> ThisDestructibleComponent = Cast<UDestructibleComponent>(ItPrimitiveComponent.Get());
		if (ThisDestructibleComponent.IsValid())
			return ThisDestructibleComponent->GetMaterialSlotNames();
	}
	TArray<FName> ReturnArray;
	return ReturnArray;
}

