#pragma once

#include <SkeletonImGUi/ApplicationBaseImGui.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/map.h>

class ToolBase;


class UToolApplication : public SkImGui::ApplicationBaseImGui
{
	typedef ea::map<std::size_t, ea::shared_ptr<ToolBase>> ToolMapByType;
	typedef ea::map<ea::string, ea::shared_ptr<ToolBase>> ToolMapByName;

public:
	SKELETON_APP_INSTANCE(UToolApplication)

public:
	enum IniType
	{
		INITYPE_DefaultGame
	};

	virtual ea::string& GetWindowTitle() override
	{
		static ea::string title("UTool");
		return title;
	}

	const ToolMapByType& GetAvailableTools() const;

	template<class T>
	ea::shared_ptr<T> GetTool() const;
	ea::shared_ptr<ToolBase> GetTool(const ea::string& toolName) const;

	virtual void Initialize() override;
	virtual void ModifyImGuiIo(ImGuiIO& io) override;
	virtual void DrawImGui(double deltaTime) override;

	const ea::string& GetLoadedProjectRootDir();
	const ea::string GetLoadedProjectIniPath(IniType iniType) const;
	const ea::string GetLoadedProjectContentPath() const;

private:
	void RenderMenu();
	void RenderNavigator();
	void RenderTool();
	void RenderStatusBar();

private:
	// 2 maps here for easier and faster referral to maps by either type or name
	// Available tools by type
	ToolMapByType _availableTools = {};
	// Available tools by tool name
	ToolMapByName _availableToolsByName = {};

	// Contains the full file path to the currently loaded .uproject
	ea::string _loadedProjectFilePath = "";
	// Contains just the root directory to the loaded project
	ea::string _loadedProjectRootDir = "";

	ea::shared_ptr<ToolBase> _currentlyFocusedTool = nullptr;
	ea::shared_ptr<ToolBase> _previouslyFocusedTool = nullptr;

	float _menuHeight = 20;
	float _sidebarWidth = 250;
	float _statusBarHeight = 20;
};

template <class T>
ea::shared_ptr<T> UToolApplication::GetTool() const
{
	return ea::dynamic_shared_pointer_cast<T>(_availableTools.at(typeid(T).hash_code()));
}
