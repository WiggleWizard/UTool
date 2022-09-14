#include "ToolPAKInspector.h"

#include <imgui.h>
#include <nfd.hpp>


void ToolPAKInspector::RenderWindowContent()
{
	if(ImGui::Button("Open PAK..."))
	{
		nfdchar_t* outPath;
		constexpr nfdfilteritem_t filterItem[1] = { { "Unreal PAK File", "pak" } };
		const nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);
		if(result == NFD_OKAY)
		{
			_loadedPakPath = outPath;
			NFD_FreePath(outPath);
		}
		else if(result == NFD_CANCEL)
		{

		}
		else
		{
		}
	}
}
