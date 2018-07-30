// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ModularBuildingPluginPrivatePCH.h"
#include "ModularBuildingPluginEdMode.h"

#define LOCTEXT_NAMESPACE "FModularBuildingPluginModule"

void FModularBuildingPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FModularBuildingPluginEdMode>(
		FModularBuildingPluginEdMode::EM_ModularBuildingPluginEdModeId, 
		LOCTEXT("ModularBuildingPluginEdModeName", "ModularBuildingPluginEdMode"), 
		FSlateIcon(), 
		true);
}

void FModularBuildingPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FModularBuildingPluginEdMode::EM_ModularBuildingPluginEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularBuildingPluginModule, ModularBuildingPlugin)