// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ModularBuildingPluginPrivatePCH.h"
#include "ModularBuildingPluginEdMode.h"
#include "ModularBuildingPluginEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"

const FEditorModeID FModularBuildingPluginEdMode::EM_ModularBuildingPluginEdModeId = TEXT("EM_ModularBuildingPluginEdMode");

FModularBuildingPluginEdMode::FModularBuildingPluginEdMode()
{

}

FModularBuildingPluginEdMode::~FModularBuildingPluginEdMode()
{

}

void FModularBuildingPluginEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FModularBuildingPluginEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FModularBuildingPluginEdMode::Exit()
{
	if (Toolkit.IsValid())
	{

	//	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	//	Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	//FEdMode::Exit();
}

bool FModularBuildingPluginEdMode::UsesToolkits() const
{
	return true;
}




