#pragma once

#include <vector>

struct AssetNode;

class AssetsExplorer
{
	friend class PanelAssets;

public:
	AssetsExplorer(const char* name) : name(name) {};
	~AssetsExplorer() {};

	void Draw(AssetNode* current_node, bool is_forward, bool is_backward);
	void MenuBar(AssetNode* current_node, bool is_forward, bool is_backward);

private:
	void DrawNode(AssetNode* node, int id); //***

	void HandleSelection(AssetNode* node);
	void OpenNode(AssetNode* node);
	void SelectAll(AssetNode* current_node);
	void UnSelectAll(AssetNode* current_node);
	void DrawRightClick();

	void UpdateMenuBarPath(AssetNode* current_node);

private:
	const char* name = "ChildExplorer";
	bool is_hierarchy_hidden = false;

	std::vector<AssetNode*> menubar_path;
};
