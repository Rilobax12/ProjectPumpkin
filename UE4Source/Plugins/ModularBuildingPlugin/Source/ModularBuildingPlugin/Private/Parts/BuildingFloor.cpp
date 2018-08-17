// Fill out your copyright notice in the Description page of Project Settings.

#include "../Private/ModularBuildingPluginPrivatePCH.h"
#include "BuildingFloor.h"
#include "MBPFloorActor.h"
//#include "MBPLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

//FVector UBuildingFloor::AutoGenerateLocation(float GridSize, int32 FloorMeshHeight)
//{
//	FVector CenterVecrtor = this->GetStaticMesh()->GetBoundingBox().GetCenter() + FVector(0.f, 0.f, this->GetStaticMesh()->GetBoundingBox().GetSize().Z * 0.5f);
//
//	return (( (UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, CenterVecrtor) + this->RelativeRotation).Vector()
//		* CenterVecrtor.Size()) * -1);
//}