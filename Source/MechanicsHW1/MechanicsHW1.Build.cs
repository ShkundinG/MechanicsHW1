// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MechanicsHW1 : ModuleRules
{
	public MechanicsHW1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MechanicsHW1",
			"MechanicsHW1/Variant_Platforming",
			"MechanicsHW1/Variant_Platforming/Animation",
			"MechanicsHW1/Variant_Combat",
			"MechanicsHW1/Variant_Combat/AI",
			"MechanicsHW1/Variant_Combat/Animation",
			"MechanicsHW1/Variant_Combat/Gameplay",
			"MechanicsHW1/Variant_Combat/Interfaces",
			"MechanicsHW1/Variant_Combat/UI",
			"MechanicsHW1/Variant_SideScrolling",
			"MechanicsHW1/Variant_SideScrolling/AI",
			"MechanicsHW1/Variant_SideScrolling/Gameplay",
			"MechanicsHW1/Variant_SideScrolling/Interfaces",
			"MechanicsHW1/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
