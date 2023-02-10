#pragma once

#include <vector>

struct AssetNode;
struct ImGuiTextFilter;

class AssetsHierarchy
{
	friend class PanelAssets;
public:
	AssetsHierarchy(const char* name) : name(name) {};
	~AssetsHierarchy() {};

	bool IsActive() const { return is_active; }

	void Draw(std::vector<AssetNode*> hierarchy_nodes);
	void MenuBar();

	//void ClearNodes();

private:
	void DrawNode(AssetNode* node, int id); //***

	void HandleSelection(AssetNode* node);

	void UpdateNodesVisibility(AssetNode* node, bool hidden);

private:
	const char* name = "ChildHierarchy";
	bool is_active = true;

	static ImGuiTextFilter searcher;
	int filter = -1;
};
