using UnrealBuildTool;
using System.IO;

public class SQLite3UE4Plugin : ModuleRules
{
    public SQLite3UE4Plugin(TargetInfo Target)
    {
		//File.WriteAllText("c:/temp/qqq.txt", this.GetType().Name);
		//string ModulePath = Path.GetDirectoryName( RulesAssembly.GetModuleFilename( this.GetType().Name ) );
		
		// RulesAssembly r;
		// FileReference CheckProjectFile;
		// UProjectInfo.TryGetProjectForTarget("MyGame", out CheckProjectFile);
		
		// r = RulesCompiler.CreateProjectRulesAssembly(CheckProjectFile);
		// FileReference f = r.GetModuleFileName( this.GetType().Name );
		// File.WriteAllText("c:/temp/qqq2.txt", f.CanonicalName );
		
        // string ModulePath = Path.GetDirectoryName( f.CanonicalName );
		string ModulePath = ModuleDirectory;
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