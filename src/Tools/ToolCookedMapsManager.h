#pragma once

#include "ToolBase.h"
#include "SimpleIniFwd.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/map.h>

#include <filesystem>

REGISTER_TOOL(ToolCookedMapsManager)
class ToolCookedMapsManager : public ToolBase
{
	struct BasicMapInfo
	{
		bool bSelected = false;
		bool bExistsOnDisk = false;
		std::filesystem::path display = "";
	};

public:
	ToolCookedMapsManager();

	const ea::string& ToolName() const override
	{
		static ea::string toolName = "MapCookManager";
		return toolName;
	}

	const ea::string& ToolTitle() const override
	{
		static ea::string toolTitle = "Map Cook Manager";
		return toolTitle;
	}

	void OnProjectChanged() override;
	void RenderWindowContent() override;

private:
	void Save();

	std::filesystem::path ParseMapIniStringToFsPath(const ea::string& inDir);

	/**
	 * \brief Takes in an INI formatted map path and parses it into full map path with extension
	 *
	 * Example: input  - (FilePath="/Game/Maps/Entry")
	 *          output - C:/Projects/Games/MyGame/Content/Maps/Entry.umap
	 */
	void ParseMapIniStringToAbsPath(const ea::string& inputPath, ea::string& absPath) const;

	/**
	 * \brief Takes in an INI formatted map path and parses it into a path that's relative to the
	 *        game's dir + "/Content/"
	 */
	void ParseMapIniStringToRelPath(const ea::string& inputPath, ea::string& relPath) const;

private:
	ea::shared_ptr<CSimpleIniA> _ini = nullptr;

	/**
	 * \brief Holds all maps that exist on disk and exist in the ini (with no duplicates)
	 */
	ea::map<std::filesystem::path, BasicMapInfo> _mapListing;
};