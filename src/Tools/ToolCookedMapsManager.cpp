#include "ToolCookedMapsManager.h"

#include "UToolApplication.h"
#include "ToolHealthCheck.h"

#include <SimpleIni.h>
#include <imgui.h>

#include <SkeletonCore/Logging.h>
LOGGER(logger, ToolCookedMapsManager)

namespace fs = std::filesystem;

static const ea::string packagingSettingsSectionName = "/Script/UnrealEd.ProjectPackagingSettings";
static const ea::string packagingSettingsCookKeyName = "+MapsToCook";


void FindAllUMapsRecursively(const fs::path& inDir, ea::map<fs::path, bool>& paths)
{
	for(const auto& entry : fs::directory_iterator(inDir.c_str()))
	{
		if(entry.is_directory())
		{
			FindAllUMapsRecursively(entry.path(), paths);
			continue;
		}

		ea::string path{ entry.path().string().c_str() };
		if(path.rfind(".umap") != ea::string::npos)
		{
			// Construct a relative path
			paths.insert({ entry.path(), true });
		}
	}
}

void FindAllUMaps(const ea::string& inDir, ea::map<std::filesystem::path, bool>& paths)
{
	paths.clear();
	FindAllUMapsRecursively(inDir.c_str(), paths);
}

ToolCookedMapsManager::ToolCookedMapsManager()
{
	_ini = ea::make_shared<CSimpleIniA>(false, true, false);
	_ini->SetSpaces(false);
}

void ToolCookedMapsManager::Initialize()
{
	UToolApplication* app = UToolApplication::GetInstance();
	if(const ea::shared_ptr<ToolHealthCheck> toolHealthCheck = app->GetTool<ToolHealthCheck>())
	{
		toolHealthCheck->onHealthCheck.Connect<&ToolCookedMapsManager::OnHealthCheck>(this);
	}
}

void ToolCookedMapsManager::OnProjectChanged()
{
	_mapListing.clear();

	UToolApplication* app = UToolApplication::GetInstance();
	const ea::string& projectRootDir = app->GetLoadedProjectRootDir();
	const ea::string defaultGameIniPath = app->GetLoadedProjectIniPath(UToolApplication::INITYPE_DefaultGame);

	// Attempt to find and load the DefaultGame.ini file
	if(_ini->LoadFile(defaultGameIniPath.c_str()) != SI_OK)
	{
		logger->error("Failed to load INI file from {}", defaultGameIniPath);
		return;
	}

	logger->info("Successfully found and parsed INI from {}", defaultGameIniPath);

	// Scan entire project's Content directory for *.umap
	ea::map<fs::path, bool> umapList;
	FindAllUMaps(projectRootDir + "/Content", umapList);

	// Throw these maps into the final map listing
	for(const auto& mapPathPair : umapList)
	{
		BasicMapInfo mapInfo {
			false,
			fs::exists(mapPathPair.first),
			fs::relative(mapPathPair.first, projectRootDir.c_str())
		};
		_mapListing.insert({ mapPathPair.first, mapInfo });
	}

	logger->info("Found {} maps on disk in project", _mapListing.size());

	// Now fill the chosen maps list
	CSimpleIniA::TNamesDepend values;
	_ini->GetAllValues(packagingSettingsSectionName.c_str(), packagingSettingsCookKeyName.c_str(), values);
	for(auto& it : values)
	{
		ea::string inStr = it.pItem;
		fs::path mapPath = ParseMapIniStringToFsPath(inStr);

		// Check first to see if we don't already have this map pushed into the listing.
		// If we do then we need to mark it as selected otherwise make a new entry.
		if(_mapListing.count(mapPath) > 0)
		{
			BasicMapInfo& mapInfo = _mapListing.at(mapPath);
			mapInfo.bSelected = true;
		}
		else
		{
			BasicMapInfo mapInfo {
				true,
				fs::exists(mapPath),
				fs::relative(mapPath, projectRootDir.c_str())
			};
			_mapListing.insert({ mapPath, mapInfo });
		}
	}

	logger->info("{} total maps in listing", _mapListing.size());
}

void ToolCookedMapsManager::RenderWindowContent()
{
	static constexpr ImVec4 colError = ImVec4(1.f, 0.05f, 0.05f, 1.f);
	static constexpr ImVec4 colWarning = ImVec4(1.f, 0.75f, 0.55f, 1.f);
	//static bool bEnabled = 

	ImGui::TextUnformatted("Check the maps to be added to the cook");

	if(ImGui::Button("Resolve Errors"))
	{
		for(auto& mapInfoPair : _mapListing)
		{
			BasicMapInfo& mapInfo = mapInfoPair.second;
			if(!mapInfo.bExistsOnDisk)
				mapInfo.bSelected = false;
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Select All"))
	{
		for(auto& mapInfoPair : _mapListing)
		{
			BasicMapInfo& mapInfo = mapInfoPair.second;
			mapInfo.bSelected = true;
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Deselect All"))
	{
		for(auto& mapInfoPair : _mapListing)
		{
			BasicMapInfo& mapInfo = mapInfoPair.second;
			mapInfo.bSelected = false;
		}
	}

	ImGui::BeginChild("MapListing", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 25));
	ImGui::Indent();
	for(auto& mapInfoPair : _mapListing)
	{
		const fs::path& mapPath = mapInfoPair.first;
		BasicMapInfo& mapInfo = mapInfoPair.second;

		bool bPopStyle = false;
		if(!mapInfo.bExistsOnDisk && mapInfo.bSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, colError);
			bPopStyle = true;
		}
		else if(!mapInfo.bExistsOnDisk)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, colWarning);
			bPopStyle = true;
		}

		if(ImGui::Checkbox(mapInfo.display.string().c_str(), &mapInfo.bSelected))
		{
			
		}

		// Tooltips
		if(ImGui::IsItemHovered())
		{
			if(!mapInfo.bExistsOnDisk && mapInfo.bSelected)
				ImGui::SetTooltip("ERROR: This file does not exist on disk but is selected for cooking. Uncheck this entry to resolve this error.");
			else if(!mapInfo.bExistsOnDisk)
				ImGui::SetTooltip("WARNING: This file does not exist on disk and will be removed from cooking upon saving.");
		}

		if(bPopStyle)
			ImGui::PopStyleColor();
	}
	ImGui::Unindent();
	ImGui::EndChild();

	ImGui::BeginChild("Controls");
	{
		if(ImGui::Button("Save"))
			Save();
		ImGui::SameLine();
		if(ImGui::Button("Reload"))
			OnProjectChanged();
	}
	ImGui::EndChild();
}

void ToolCookedMapsManager::OnHealthCheck()
{
	logger->info("Hello WOrld");
}

void ToolCookedMapsManager::Save()
{
	UToolApplication* app = UToolApplication::GetInstance();
	const ea::string projectContentDir  = app->GetLoadedProjectContentPath();
	const ea::string defaultGameIniPath = app->GetLoadedProjectIniPath(UToolApplication::INITYPE_DefaultGame);

	// Destroy all cook map entries so we can just blast over it
	_ini->Delete(packagingSettingsSectionName.c_str(), packagingSettingsCookKeyName.c_str());

	// Now ammend our selected maps
	for(auto& mapInfoPair : _mapListing)
	{
		const fs::path& mapPath     = mapInfoPair.first;
		const BasicMapInfo& mapInfo = mapInfoPair.second;

		if(mapInfo.bSelected)
		{
			// Convert the full map path to something that UE will accept in the config
			fs::path relPath = fs::relative(mapPath, projectContentDir.c_str());
			ea::string finalPath = "/Game/" + ea::string(relPath.string().c_str());
			finalPath.erase(finalPath.rfind(".umap"));

			// \ to /
			ea::replace(finalPath.begin(), finalPath.end(), '\\', '/');

			_ini->SetValue(packagingSettingsSectionName.c_str(), packagingSettingsCookKeyName.c_str(), fmt::format("(FilePath=\"{}\")", finalPath).c_str(), nullptr);
		}
	}

	if(_ini->SaveFile(defaultGameIniPath.c_str()) != SI_OK)
	{
		logger->error("An error occurred when trying to save to {}", defaultGameIniPath);
	}

	OnProjectChanged();
}

fs::path ToolCookedMapsManager::ParseMapIniStringToFsPath(const ea::string& inDir)
{
	ea::string absPath;
	ParseMapIniStringToAbsPath(inDir, absPath);
	return { absPath.c_str() };
}

void ToolCookedMapsManager::ParseMapIniStringToAbsPath(const ea::string& inputPath, ea::string& absPath) const
{
	ea::string relPath = "";
	ParseMapIniStringToRelPath(inputPath, relPath);

	// Now prefix the project path behind the relativePath
	UToolApplication* app = UToolApplication::GetInstance();
	const ea::string& projectRootDir = app->GetLoadedProjectRootDir();
	absPath = projectRootDir + "/Content/" + relPath;
}

void ToolCookedMapsManager::ParseMapIniStringToRelPath(const ea::string& inputPath, ea::string& relPath) const
{
	// Check input first to see if it matches our criteria
	static const ea::string entrySuffix = "(FilePath=\"";
	static const ea::string relPathSuffix = "/Game/";

	if(inputPath.find(entrySuffix) != 0)
	{
		return;
	}

	// Strip the path out of the clutches from the whole FilePath rubbish
	const ea::string mapGamePath = inputPath.substr(entrySuffix.size(), inputPath.size() - entrySuffix.size() - 2);
	// Strip /Game/ from the beginning of the path
	relPath = mapGamePath.substr(relPathSuffix.size(), mapGamePath.size() - relPathSuffix.size()) + ".umap";
}
