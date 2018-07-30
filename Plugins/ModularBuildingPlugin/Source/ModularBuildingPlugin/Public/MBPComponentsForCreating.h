// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MBPComponentsForCreating.generated.h"

USTRUCT()
struct FSlotOfObjectsForCreating
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY() UObject* ObjectForCreating;

	UPROPERTY() FVector ChangesVector;

	UPROPERTY() FRotator ChangesRotation;

	UPROPERTY() float OffsetOfObject;

	FSlotOfObjectsForCreating()
	{
		ObjectForCreating = NULL;
		ChangesVector = FVector::ZeroVector;
		ChangesRotation = FRotator::ZeroRotator;
		OffsetOfObject = 0.f;
	}
};

USTRUCT()
struct FListOfObjectsForCreating
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()

		UPROPERTY() TArray<FSlotOfObjectsForCreating> SlotsForCreating;

		//UPROPERTY() TArray<UObject*> ObjectsForCreating;
		//UPROPERTY() TArray<FVector> ChangesVectors;
		//UPROPERTY() TArray<FRotator> ChangesRotations;
		//UPROPERTY() TArray<float> OffsetOfObjects;

		UPROPERTY() FText ListName;

		//Get 
		int32 GetNumOfComponents()
		{
			return SlotsForCreating.Num();
		}

		//Add
		void AddSlot(int32 Num)
		{
			for (int32 it = 0; it < Num; it++)
			{
				SlotsForCreating.Add(FSlotOfObjectsForCreating());
			}
		}

		//Special
		bool IsValidIndex(int32 Num)
		{
			return SlotsForCreating.IsValidIndex(Num);
		}

		TArray<TWeakObjectPtr<UObject>> GetWeakObjectsForCreatingPtr()
		{
			TArray<TWeakObjectPtr<UObject>> ReturnArray;
			for (FSlotOfObjectsForCreating ItSlot : SlotsForCreating)
			{
				ReturnArray.Add(ItSlot.ObjectForCreating);
			}
			return ReturnArray;
		}

	FListOfObjectsForCreating()
	{
		ListName = FText::FromString("ListOfObjects");
	}
};

UCLASS()
class MODULARBUILDINGPLUGIN_API AMBPComponentsForCreating : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBPComponentsForCreating();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY() TArray<FListOfObjectsForCreating> ListsOfObjects;

	UPROPERTY() int32 SelectedList = 0;

	//Get
	FListOfObjectsForCreating GetSelectedListOfObjects();

	int32 GetNumOfSlotsInSelectedListOfObjects();

	int32 GetNumOfListsOfObjects();

	//Add
	void AddNewSlot();

	void AddNewList(int32 Num);

	//Delete
	void DeleteListByIndex(int32 Index);

	void DeleteSlotByIndexInSelectedList(int32 Index);

	//Change
	int32 ChangeSlotPosition(bool ToUp, int32 ComponentIndex);

	//Object
	TWeakObjectPtr<UObject> GetObjectByIndex(int32 Index);

	void SetObjectByIndex(TWeakObjectPtr<UObject> NewObject, int32 Index);

	//Vector
	FVector GetChangeVectorByIndex(int32 Index);

	void SetChangeVectorByIndex(FVector NewVector, int32 Index);

	//Rotator
	FRotator GetChangeRotationByIndex(int32 Index);

	void SetChangeRotationByIndex(FRotator NewRotator, int32 Index);

	//Offset
	float GetOffsetByIndex(int32 Index);

	void SetOffsetByIndex(float NewOffset, int32 Index);

	//Name
	FText GetNameOfListByIndex(int32 Index);
	void SetNameOfListByIndex(FText NewName, int32 Index);

	bool IsValidSlotIndex(int32 Index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
};
