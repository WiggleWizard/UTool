#pragma once

#include "ToolBase.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/map.h>


class ToolPAKInspector : public ToolBase
{
public:
	TOOL("PAKInspector", "PAK Inspector")

	void RenderWindowContent() override;

private:
	ea::string _loadedPakPath = "";
};
REGISTER_TOOL_FACTORY(PAKInspector)