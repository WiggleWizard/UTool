#pragma once

#include "Toolbox.h"

#include <SkeletonCore/Aliases.h>
#include <EASTL/string.h>

#define REGISTER_TOOL_FACTORY(T) \
	static bool bSuccessfulRegistry##T = Toolbox::RegisterToolFactory("Tool" #T, []() \
	{ \
		return new Tool##T(); \
	});

#define TOOL(ToolName, ToolTitle) \
	const ea::string& GetToolName() const override \
	{ \
		static ea::string toolName = ##ToolName; \
		return toolName; \
	} \
	const ea::string& GetToolTitle() const override \
	{ \
		static ea::string toolTitle = ##ToolTitle; \
		return toolTitle; \
	}

class ToolBase
{
public:
	virtual const ea::string& GetToolName() const
	{
		static ea::string toolName = "NewTool";
		return toolName;
	}

	virtual const ea::string& GetToolTitle() const
	{
		static ea::string toolTitle = "New Tool";
		return toolTitle;
	}

	virtual void Initialize() {}
	virtual void OnFocused() {}
	virtual void OnProjectChanged() {}
	virtual void RenderWindowContent();

	/**
	 * \brief Called when save project is triggered
	 */
	virtual void OnUToolProjectSave() {}
};