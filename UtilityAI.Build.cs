// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UtilityAI : ModuleRules
{
	public UtilityAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "UnrealEd",       
    "AIModule",        
    "GameplayTasks",
    "EnhancedInput"
});

    }
}
