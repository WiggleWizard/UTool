#include "Toolbox.h"


ea::vector<Toolbox::ToolFactory> Toolbox::availableToolFactories;

bool Toolbox::RegisterToolFactory(const char* toolName, ToolFactory f)
{
	availableToolFactories.push_back(f);
	return true;
}
