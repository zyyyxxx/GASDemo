// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASDemo : ModuleRules
{
	public GASDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" , "MotionWarping" , 
			"NetCore", "Niagara" , "OnlineSubsystem" , "Chaos" , "FieldSystemEngine" });

		PrivateIncludePaths.Add("GASDemo/");
		PrivateDependencyModuleNames.AddRange(new string[] {
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
		});
	}
}
