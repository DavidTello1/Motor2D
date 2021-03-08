#pragma once

struct GameObjects
{
	//friend class ModuleScene;
	//friend class ResourceScene;
	//ALIGN_CLASS_TO_16 //for float4x4 & aabb

	enum ComponentFlags
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
	std::vector<int> component_flags;

	//std::vector<ComponentTransform> transforms;
	//std::vector<ComponentRenderer> renderers;

};
