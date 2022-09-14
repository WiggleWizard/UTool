#pragma once

#include "ToolBase.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/map.h>


REGISTER_TOOL(ToolPluginGenerator)
class ToolPluginGenerator : public ToolBase
{
public:
	TOOL("PluginGenerator", "Plugin Generator")
};