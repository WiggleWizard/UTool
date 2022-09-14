#include "UToolApplication.h"

#include "Tools/ToolCookedMapsManager.h"
#include "Toolbox.h"
#include "Tools/ToolBase.h"

#include <SimpleIni.h>
#include <imgui.h>
#include <nfd.h>

#include <SkeletonCore/Logging.h>
LOGGER(logger, UToolApplication)

constexpr ImGuiWindowFlags sectionFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;


const UToolApplication::ToolMapByType& UToolApplication::GetAvailableTools() const
{
	return _availableTools;
}

ea::shared_ptr<ToolBase> UToolApplication::GetTool(const ea::string& toolName) const
{
	return _availableToolsByName.at(toolName);
}

void UToolApplication::Initialize()
{
	NFD_Init();

	//_bShowDemoWindow = true;

	// Construct the tool instances from the factories
	for(const Toolbox::ToolFactory& toolFactory : Toolbox::availableToolFactories)
	{
		// Construct a new tool using the tool factory
		auto newTool = ea::shared_ptr<ToolBase>(toolFactory());
		if(newTool == nullptr)
		{
			logger->error("Tool factory return nullptr");
			continue;
		}

		const type_info& typeInfo = typeid(*newTool);
		std::size_t t = typeid(*newTool).hash_code();

		// We want to map by type and by name for super easy access to available tools when
		// using type or name
		_availableTools.insert({ t, newTool});
		_availableToolsByName.insert({ newTool->GetToolTitle(), newTool });
	}

	// Initialize each tool, to give them a chance to hook into other processes/tools
	for(const auto& [typeId, tool] : _availableTools)
	{
		tool->Initialize();
	}

	logger->info("UTool starting");
	logger->info("{} available tools", _availableTools.size());
}

void UToolApplication::ModifyImGuiIo(ImGuiIO& io)
{
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
}

void UToolApplication::DrawImGui(double deltaTime)
{
	static bool workspaceOpen = true;
	ImGuiWindowFlags workspaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	workspaceFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	workspaceFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Workspace", &workspaceOpen, workspaceFlags);
	ImGui::PopStyleVar(2);
	{
		RenderMenu();
		RenderNavigator();
		RenderTool();
		RenderStatusBar();
	}
	ImGui::End();
}

const ea::string& UToolApplication::GetLoadedProjectRootDir()
{
	return _loadedProjectRootDir;
}

const ea::string UToolApplication::GetLoadedProjectIniPath(IniType iniType) const
{
	ea::string result = "";

	switch(iniType)
	{
	case(INITYPE_DefaultGame): result = _loadedProjectRootDir + "/Config/DefaultGame.ini"; break;
	}

	return result;
}

const ea::string UToolApplication::GetLoadedProjectContentPath() const
{
	return _loadedProjectRootDir + "/Content";
}

void UToolApplication::RenderMenu()
{
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Open Unreal Project..."))
			{
				nfdchar_t* outPath;
				constexpr nfdfilteritem_t filterItem[1] = { { "Unreal Project", "uproject" } };
				const nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);
				if(result == NFD_OKAY)
				{
					_loadedProjectFilePath = outPath;
					_loadedProjectRootDir = _loadedProjectFilePath.substr(0, _loadedProjectFilePath.rfind("\\"));
					NFD_FreePath(outPath);

					for(const auto& [typeId, tool] : _availableTools)
					{
						tool->OnProjectChanged();
					}
				}
				else if(result == NFD_CANCEL)
				{

				}
				else
				{
					logger->error("Something went wrong");
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void UToolApplication::RenderNavigator()
{
	static bool bOpen = true;
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x, mainViewport->WorkPos.y + _menuHeight));
	ImGui::SetNextWindowSize(ImVec2(_sidebarWidth, mainViewport->WorkSize.y - _menuHeight - _statusBarHeight));

	constexpr ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	
	if(ImGui::Begin("Navigator", &bOpen, sectionFlags))
	{
		if(ImGui::CollapsingHeader("Specific", ImGuiTreeNodeFlags_DefaultOpen))
		{
			int toolIdx = 0;
			for(const auto& [typeId, tool] : _availableTools)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)toolIdx, nodeFlags, "%s", tool->GetToolTitle().c_str());
				if(ImGui::IsItemClicked())
					_currentlyFocusedTool = tool;

				++toolIdx;
			}
		}

		if(ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
		{
		}
	}
	ImGui::End();
}

void UToolApplication::RenderTool()
{
	static bool bOpen = true;
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x + _sidebarWidth, mainViewport->WorkPos.y + _menuHeight));
	ImGui::SetNextWindowSize(ImVec2(mainViewport->WorkSize.x - _sidebarWidth, mainViewport->WorkSize.y - _statusBarHeight - _menuHeight));
	if(ImGui::Begin("Tool", &bOpen, sectionFlags))
	{
		if(_currentlyFocusedTool)
		{
			_currentlyFocusedTool->RenderWindowContent();
		}
	}
	ImGui::End();
}

void UToolApplication::RenderStatusBar()
{
	static bool bOpen = true;
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x, mainViewport->WorkSize.y - _statusBarHeight));
	ImGui::SetNextWindowSize(ImVec2(mainViewport->WorkSize.x, _statusBarHeight));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
	if(ImGui::Begin("StatusBar", &bOpen, sectionFlags))
	{
		ImGui::PopStyleVar();
		ImGui::TextUnformatted(_loadedProjectFilePath.c_str());
	}
	ImGui::End();
}
