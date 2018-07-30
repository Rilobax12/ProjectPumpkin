// Fill out your copyright notice in the Description page of Project Settings.

#include "ModularBuildingPluginPrivatePCH.h"
#include "MBPListOfFurnit.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Runtime/Core/Public/Containers/Array.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "UnrealEd.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstance.h"
//#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
//#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/ChildActorComponent.h"


// Sets default values
AMBPListOfFurnit::AMBPListOfFurnit()
{

	for (int it = 0; it < 20; it++)
	{
		Room.AddArray();
		Room.GetIt[it].AddMeshVal();
		Room.GetIt[it].AddBluepVal();
		Room.GetIt[it].AddAttrib();
	}
	Attrib_Array.Init(FString("some"), 20);
	


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMBPListOfFurnit::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMBPListOfFurnit::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
}