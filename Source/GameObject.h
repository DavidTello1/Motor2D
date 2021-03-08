#pragma once
#include "Globals.h"

#include <vector>
#include <string>

//struct ComponentTransform;
//struct ComponentRenderer;
//struct ComponentCamera;

struct GameObjects
{
	enum Components
	{
		COMPONENT_TRANSFORM = 1 << 0,
		COMPONENT_RENDERER = 1 << 1,
		COMPONENT_CAMERA = 1 << 2,
	};

	enum ObjectFlags
	{
		ACTIVE = 1 << 0,
		STATIC = 1 << 1,
		HIDDEN = 1 << 2,
	};

	std::vector<UID> ids;
	std::vector<std::string> names;
	std::vector<int> flags;
	std::vector<int> components;

	//std::vector<ComponentTransform> transforms;
	//std::vector<ComponentRenderer> renderers;
	//std::vector<ComponentCamera> cameras;
};
