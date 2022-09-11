#pragma once

#include <SkeletonCore/Aliases.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/vector.h>

class ToolBase;


class Toolbox
{
public:
	template<class T>
	static bool AddTool()
	{
		availableTools.push_back(ea::make_shared<T>());
		return true;
	}

	static ea::vector<ea::shared_ptr<ToolBase>> availableTools;
};