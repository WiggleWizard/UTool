#pragma once

#include <SkeletonCore/Aliases.h>

#include <EASTL/vector.h>

class ToolBase;


class Toolbox
{
public:
	typedef ea::function<ToolBase* ()> ToolFactory;

	static bool RegisterToolFactory(const char* toolName, ToolFactory f);

	static ea::vector<ToolFactory> availableToolFactories;
};