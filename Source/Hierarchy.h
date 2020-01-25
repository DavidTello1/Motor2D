#pragma once
#include "Panel.h"

#include <vector>

class GameObject;

enum NodeType {
	UNKNOWN = 0,
	OBJECT,
	SCENE,
	FOLDER
};

struct HierarchyNode {
	NodeType type = UNKNOWN;

	bool rename = false;
	bool leaf = true;
	bool selected = false;

	const char* name;
	int pos = 0;

	GameObject* obj = nullptr;
	//ResourceScene* scene = nullptr;
};

class Hierarchy : public Panel
{
public:
	Hierarchy();
	virtual ~Hierarchy();

	void Draw();
	void DrawNode(GameObject* object);

	void Select(GameObject* object);
	void UnSelect(GameObject* object);
	bool ToggleSelection(GameObject* object);

public:
	static const uint default_width = 260;
	static const uint default_height = 540;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 20;

private:
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	std::vector<HierarchyNode*> nodes;
	HierarchyNode* selected_node = nullptr;
};
