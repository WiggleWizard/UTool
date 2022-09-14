#pragma once

#include "ToolBase.h"

#include <SkeletonCore/Event/Signal.h>


class ToolHealthCheck : public ToolBase
{
public:
	TOOL("HealthCheck", "Health Check")

	void RenderWindowContent() override;

public:
	SkeletonCore::Signal<void(void)> onHealthCheck;
};
REGISTER_TOOL_FACTORY(HealthCheck)