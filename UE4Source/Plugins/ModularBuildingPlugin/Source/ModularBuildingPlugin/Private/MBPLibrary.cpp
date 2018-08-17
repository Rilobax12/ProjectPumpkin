// Fill out your copyright notice in the Description page of Project Settings.

#include "../Private/ModularBuildingPluginPrivatePCH.h"
#include "MBPLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

MBPLibrary::MBPLibrary()
{
}

MBPLibrary::~MBPLibrary()
{
}

FVector MBPLibrary::OptimizeVector(FVector ThisVector)
{
	//double  ItNum = 100.f; 
	
	//float Y = MyRound(ThisVector.Y);
	//float Z = MyRound(ThisVector.Z);
	float X = MyRound(ThisVector.X);
	UE_LOG(LogTemp, Log, TEXT("New Chislo %f"), X);
	//return FVector(X, Y, Z);
	return FVector(ThisVector.X, ThisVector.Y, ThisVector.Z);
}

double MBPLibrary::MyRound(const double x)
{
	const int mainPart = static_cast<int>(x);
	if (x < 0)
	{
		const bool toLowerBound = (0.75 <= mainPart - x);
		const bool toUpperBound = (mainPart - x < 0.25);
		return toUpperBound ? mainPart : toLowerBound ? mainPart - 1 : mainPart - 0.5;
	}
	else
	{
		const bool toLowerBound = (x - mainPart < 0.25);
		const bool toUpperBound = (0.75 <= (x - mainPart));
		return toUpperBound ? mainPart + 1 : toLowerBound ? mainPart : mainPart + 0.5;
	}
}