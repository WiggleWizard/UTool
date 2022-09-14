#pragma once

#include "ToolBase.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/map.h>


class ToolPluginGenerator : public ToolBase
{
public:
	TOOL("PluginGenerator", "Plugin Generator")
};
REGISTER_TOOL_FACTORY(PluginGenerator)