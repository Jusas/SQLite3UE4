using UnrealBuildTool;
using System.IO;

public class SQLite3UE4Plugin : ModuleRules
{
    public SQLite3UE4Plugin(TargetInfo Target)
    {
        string ModulePath = Path.GetDirectoryName( RulesCompiler.GetModuleFilename( this.GetType().Name ) );
        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        string ThirdPartyPath = Path.GetFullPath( Path.Combine( ModulePath, "../../ThirdParty/" ) );
        string LibrariesPath = Path.Combine(ThirdPartyPath, "SQLite3", "Lib");
        string IncludesPath = Path.Combine(ThirdPartyPath, "SQLite3", "Include");
        string LibraryName = Path.Combine(LibrariesPath, "SQLite3." + PlatformString + ".lib");

        PrivateIncludePaths.AddRange(new string[] { "SQLite3UE4Plugin/Private" });
        PublicIncludePaths.AddRange(new string[] { "SQLite3UE4Plugin/Public" });
        PublicAdditionalLibraries.Add(LibraryName);
        PublicIncludePaths.Add(IncludesPath);

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject" });
    }
}