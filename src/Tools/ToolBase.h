#pragma once

#include <SkeletonCore/Aliases.h>
#include <EASTL/string.h>


class ToolBase
{
public:
	virtual const ea::string& ToolName() const
	{
		static ea::string toolName = "NewTool";
		return toolName;
	}

	virtual const ea::string& ToolTitle() const
	{
		static ea::string toolTitle = "New Tool";
		return toolTitle;
	}

	virtual void OnFocused() {}
	virtual void OnProjectChanged() {}
	virtual void RenderWindowContent() {}
};