// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MBPListOfFurnit.generated.h"

#define LOCTEXT_NAMESPACE "Your Namespace" 
//USTRUCT()
//struct FStringArray
//{
//	GENERATED_USTRUCT_BODY()
//
//		//Whatever your core data of the array is, make sure you use UPROPERTY()
//		UPROPERTY()
//		TArray<FString> StringArray;
//};
//USTRUCT()
//struct FStaticMeshArray
//{
//	GENERATED_USTRUCT_BODY()
//
//		//Whatever your core data of the array is, make sure you use UPROPERTY()
//		UPROPERTY()
//		TArray<UStaticMesh*> MeshArrayIt;
//};
//USTRUCT()
//struct FStaticMeshAssetArray
//{
//	GENERATED_USTRUCT_BODY()
//
//		UPROPERTY()
//		TArray<TWeakObjectPtr<UStaticMesh>> MeshAssetArray;
//};
//USTRUCT()
//struct FBlueprintArray
//{
//	GENERATED_USTRUCT_BODY()
//
//		UPROPERTY()
//		TArray<UBlueprint*> BluepArray;
//};
//USTRUCT()
//struct FBlueprintAssetArray
//{
//	GENERATED_USTRUCT_BODY()
//
//		UPROPERTY()
//		TArray<TWeakObjectPtr<UBlueprint>> BluepAssetArray;
//};

USTRUCT()
struct FVal
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()

		UPROPERTY()
			TArray<float> Mesh_Margin;
		UPROPERTY()
			TArray<float> Bluep_Margin;

		UPROPERTY()
			TArray<FString> Mesh_Posit;
		UPROPERTY()
			TArray<FString> Bluep_Posit;

		UPROPERTY()
			TArray<FString> Mesh_Posit_Array;
		UPROPERTY()
			TArray<FString> Bluep_Posit_Array;

		UPROPERTY()
			TArray<FText> Mesh_AttribNum_Array;
		UPROPERTY()
			TArray<FText> Bluep_AttribNum_Array;


		UPROPERTY()
			TArray<FText> AttribText;
		UPROPERTY()
			TArray<int32> AttribNum;


		UPROPERTY()
			TArray<UStaticMesh*> Mesh_Array;
		UPROPERTY()
			TArray<TWeakObjectPtr<UStaticMesh>> Mesh_Asset;
		UPROPERTY()
			TArray<UBlueprint*>  Bluep_Array;
		UPROPERTY()
			TArray<TWeakObjectPtr<UBlueprint>> Bluep_Asset;

		/* Top of File */
	
		
		FText Null = LOCTEXT("ANULL", "NULL");
		
	

		void AddMeshVal()
		{
			Mesh_Array.Add(NULL);
			Mesh_Asset.Add(NULL);
			Mesh_AttribNum_Array.Add(Null);
			Mesh_Posit.Add(FString("down"));
			Mesh_Margin.Add(1.f);
		}
		void AddBluepVal()
		{
			Bluep_Array.Add(NULL);
			Bluep_Asset.Add(NULL);
			Bluep_AttribNum_Array.Add(Null);
			Bluep_Posit.Add(FString("down"));
			Bluep_Margin.Add(1.f);
		}
		void AddAttrib()
		{
			AttribText.Add(Null);
			AttribNum.Add(-1);
		}
		//default properties
		FVal()
		{

		}

	
};

#undef LOCTEXT_NAMESPACE 
/* Bottom of File */

USTRUCT()
struct FFuArray
{
	GENERATED_USTRUCT_BODY()
		//Whatever your core data of the array is, make sure you use UPROPERTY()
	UPROPERTY()
		TArray<FVal> GetIt;
	
	UPROPERTY(EditAnywhere, Category = Spawning)
		TArray <int32> Size;
	void AddArray()
	{
		Size.Add(1);
		GetIt.Add(FVal());
	}
};

UCLASS()
class AMBPListOfFurnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMBPListOfFurnit();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	


	virtual void OnConstruction(const FTransform & Transform) override;

	UPROPERTY()
		FFuArray Room;

	UPROPERTY()
		TArray<FString> Attrib_Array;

	
};



