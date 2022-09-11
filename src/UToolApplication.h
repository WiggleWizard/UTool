#pragma once

#include <SkeletonImGUi/ApplicationBaseImGui.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>

class ToolBase;


class UToolApplication : public SkImGui::ApplicationBaseImGui
{
public:
	enum IniType
	{
		INITYPE_DefaultGame
	};

public:
	SKELETON_APP_INSTANCE(UToolApplication)

	virtual ea::string& GetWindowTitle() override
	{
		static ea::string title("UTool");
		return title;
	}

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