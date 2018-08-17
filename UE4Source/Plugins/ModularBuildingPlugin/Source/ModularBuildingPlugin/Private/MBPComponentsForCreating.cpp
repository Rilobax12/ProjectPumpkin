// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularBuildingPluginPrivatePCH.h"
#include "MBPComponentsForCreating.h"

#define NumOfDefaultListsOfObjects 3


// Sets default values
AMBPComponentsForCreating::AMBPComponentsForCreating()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AddNewList(3);
}

// Called when the game starts or when spawned
void AMBPComponentsForCreating::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMBPComponentsForCreating::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Get
FListOfObjectsForCreating AMBPComponentsForCreating::GetSelectedListOfObjects()
{
	return ListsOfObjects[SelectedList];
}

int32 AMBPComponentsForCreating::GetNumOfSlotsInSelectedListOfObjects()
{
	return GetSelectedListOfObjects().GetNumOfComponents();
}

int32 AMBPComponentsForCreating::GetNumOfListsOfObjects()
{
	return ListsOfObjects.Num();
}

//Add
void AMBPComponentsForCreating::AddNewSlot()
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		ListsOfObjects[SelectedList].AddSlot(1);


	}
}

void AMBPComponentsForCreating::AddNewList(int32 Num)
{
	for (int32 it = 0; it < Num; it++)
	{
		ListsOfObjects.Add(FListOfObjectsForCreating());
	}
}

//Delete
void AMBPComponentsForCreating::DeleteListByIndex(int32 Index)
{
	if(ListsOfObjects.IsValidIndex(Index))
		ListsOfObjects.RemoveAt(Index);
}

void AMBPComponentsForCreating::DeleteSlotByIndexInSelectedList(int32 Index)
{
	if (IsValidSlotIndex(Index))
	{
		ListsOfObjects[SelectedList].SlotsForCreating.RemoveAt(Index);
	}
}

//Change
int32 AMBPComponentsForCreating::ChangeSlotPosition(bool ToUp, int32 ComponentIndex)
{
	if (IsValidSlotIndex(ComponentIndex))
	{
		int32 NewSlotIndex = 0;

		if (ToUp)
		{

			NewSlotIndex = ComponentIndex - 1;

			if (IsValidSlotIndex(NewSlotIndex) == false)
				NewSlotIndex = GetNumOfSlotsInSelectedListOfObjects() - 1;
		}
		else
		{
			NewSlotIndex = ComponentIndex + 1;

			if (IsValidSlotIndex(NewSlotIndex) == false)
				NewSlotIndex = 0;
		}

		FSlotOfObjectsForCreating LocalSlot = FSlotOfObjectsForCreating();
		LocalSlot = ListsOfObjects[SelectedList].SlotsForCreating[ComponentIndex];
		
		ListsOfObjects[SelectedList].SlotsForCreating[ComponentIndex] = GetSelectedListOfObjects().SlotsForCreating[NewSlotIndex];
		ListsOfObjects[SelectedList].SlotsForCreating[NewSlotIndex] = LocalSlot;

		return NewSlotIndex;
	}
	return 0;
}

//Object
TWeakObjectPtr<UObject> AMBPComponentsForCreating::GetObjectByIndex(int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
		{
			return ListsOfObjects[SelectedList].SlotsForCreating[Index].ObjectForCreating;
		}
		//	return ListsOfObjects[SelectedList].SlotsForCreating[Index].ObjectForCreating;
	}
	return NULL;
}

void AMBPComponentsForCreating::SetObjectByIndex(TWeakObjectPtr<UObject> NewObject, int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			//ListsOfObjects[SelectedList].GetSlot(Index).ObjectForCreating = NewObject.Get();
			ListsOfObjects[SelectedList].SlotsForCreating[Index].ObjectForCreating = NewObject.Get();
	}
}

//Vector
FVector AMBPComponentsForCreating::GetChangeVectorByIndex(int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			return ListsOfObjects[SelectedList].SlotsForCreating[Index].ChangesVector;
	}
	return FVector::ZeroVector;
}

void AMBPComponentsForCreating::SetChangeVectorByIndex(FVector NewVector, int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			ListsOfObjects[SelectedList].SlotsForCreating[Index].ChangesVector = NewVector;
	}
}

//Rotator
FRotator AMBPComponentsForCreating::GetChangeRotationByIndex(int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			return ListsOfObjects[SelectedList].SlotsForCreating[Index].ChangesRotation;
	}
		return FRotator::ZeroRotator;
}

void AMBPComponentsForCreating::SetChangeRotationByIndex(FRotator NewRotator, int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			ListsOfObjects[SelectedList].SlotsForCreating[Index].ChangesRotation = NewRotator;
	}
}

//Offset
float AMBPComponentsForCreating::GetOffsetByIndex(int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			return ListsOfObjects[SelectedList].SlotsForCreating[Index].OffsetOfObject;
	}
	return 0.f;
}

void AMBPComponentsForCreating::SetOffsetByIndex(float NewOffset, int32 Index)
{
	if (ListsOfObjects.IsValidIndex(SelectedList))
	{
		if (ListsOfObjects[SelectedList].SlotsForCreating.IsValidIndex(Index))
			ListsOfObjects[SelectedList].SlotsForCreating[Index].OffsetOfObject = NewOffset;
	}
}

//Name
FText AMBPComponentsForCreating::GetNameOfListByIndex(int32 Index)
{
	if (ListsOfObjects.IsValidIndex(Index))
	{
		return ListsOfObjects[Index].ListName;
	}
	else return FText::FromString("ListOfObjects");
}

void AMBPComponentsForCreating::SetNameOfListByIndex(FText NewName, int32 Index)
{
	if (ListsOfObjects.IsValidIndex(Index))
	{
		ListsOfObjects[Index].ListName = NewName;
	}
}

//Special
bool AMBPComponentsForCreating::IsValidSlotIndex(int32 Index)
{
	if(ListsOfObjects.IsValidIndex(SelectedList))
	return GetSelectedListOfObjects().IsValidIndex(Index);
	else return false;
}

