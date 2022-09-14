#include "ToolHealthCheck.h"

#include <imgui.h>


void ToolHealthCheck::RenderWindowContent()
{
	if(ImGui::Button("Check Project Health"))
	{
		onHealthCheck.Fire();
	}
}
