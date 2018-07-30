// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "MBPLibrary.h"

#include "BuildingPart.generated.h"



UCLASS()
class MODULARBUILDINGPLUGIN_API UBuildingPart : public UObject
{
	GENERATED_BODY()
		
public:

//Set
	void SetNewComponent(TWeakObjectPtr<USceneComponent> NewComponent);

	void SetNewVision(bool bNewVision);

	void SetBuildingPartRotation(FRotator NewRotator);

	void SetBuildingPartLocation(FVector NewLocation);

	void SetRelativeRotation(FRotator NewRotator);

	void SetRelativeLocation(FVector NewLocation);

	void SetRelativeScale(FVector NewScale);

	void SetRelativeTransform(FTransform NewTransform);

	void SetCellsLocationsForInstancedComponents(TArray<FVector> ItCellsLocation);

	void AddVectorToLocation(FVector ThisVector);

	void SetMaterialByIndex(int32 SlotIndex, TWeakObjectPtr<UMaterialInterface> NewMaterial );
	//Check If Slot Name Contains Selected SlotName. All Material's slots which contains SlotName will change.
	void SetMaterialBySlotName(FString SlotNane, TWeakObjectPtr<UMaterialInterface> NewMaterial);

	void SetArrayOfMaterials(TArray<TWeakObjectPtr<UMaterialInterface>> NewMaterials);

//Delete
	void DestroyComponent();

//Get
	FRotator GetRelativeRotation();

	FVector GetRelativeLocation();

	FTransform GetRelativeTransform();

	TWeakObjectPtr<USceneComponent> GetSceneComponent();

	TWeakObjectPtr<UDestructibleComponent> GetDestructibleComponent();

	TWeakObjectPtr<UPrimitiveComponent> GetPrimitiveComponent();

	TWeakObjectPtr<UStaticMeshComponent> GetStaticMeshComponent();

	TWeakObjectPtr<UInstancedStaticMeshComponent> GetInstancedStaticMeshComponent();

	TWeakObjectPtr<UChildActorComponent> GetChildActorComponent();

	TArray<FVector> GetCellsLocationsForInstancedComponents();

	TArray<TWeakObjectPtr<UMaterialInterface>> GetMaterials();

	TArray<FName> GetSlotNamesOfPrimitiveComponent(TWeakObjectPtr<UPrimitiveComponent> ItPrimitiveComponent);

//***
	UPROPERTY() FVector CellLocation;

	UPROPERTY() UObject* UsedObject;

	UPROPERTY() FVector Center;

	UPROPERTY() FVector Size;

	UPROPERTY(EditAnywhere, Category = Spawning) USceneComponent* ThisComponent;

protected:

	UPROPERTY() TArray<FVector> CellsLocationsForInstancedComponents;

	
};
