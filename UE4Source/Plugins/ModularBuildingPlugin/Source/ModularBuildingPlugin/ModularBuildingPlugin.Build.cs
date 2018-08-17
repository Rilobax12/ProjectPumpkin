// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ModularBuildingPlugin : ModuleRules
{
	public ModularBuildingPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "ApexDestruction" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ApexDestruction" });

        PublicIncludePaths.AddRange(
            new string[] {
                "MyEditor/Public",
         //       "UnrealEd"
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "MyEditor/Private",
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "UnrealEd"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
           //     "Asset",
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UnrealEd"
            }
            );
        PrivateIncludePaths.Add("Editor/DetailCustomizations/Private"); // For PCH includes (because they don't work with relative paths, yet)

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "DesktopWidgets",
                "Engine",
                "Landscape",
                "InputCore",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "UnrealEd",
                "EditorWidgets",
                "KismetWidgets",
                "MovieSceneCapture",
                "SharedSettingsWidgets",
                "ContentBrowser",
                "BlueprintGraph",
                "AnimGraph",
                "PropertyEditor",
                "LevelEditor",
                "DesktopPlatform",
                "ClassViewer",
                "TargetPlatform",
                "ExternalImagePicker",
                "MoviePlayer",
                "SourceControl",
                "InternationalizationSettings",
                "SourceCodeAccess",
                "RHI",
                "HardwareTargeting",
                "SharedSettingsWidgets",
                "LightPropagationVolumeRuntime",
                "AIModule",
                "MeshUtilities",
                "ConfigEditor",
                "Persona",
                "CinematicCamera",
                "ComponentVisualizers"
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Engine",
                "Media",
                "Landscape",
                "LandscapeEditor",
                "PropertyEditor",
                "GameProjectGeneration",
                "ComponentVisualizers"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "Layers",
                "GameProjectGeneration",
            }
        );

    }
}
