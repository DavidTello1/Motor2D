#pragma once
#include "Panel.h"

#include "Globals.h"

#include <vector>

struct Resource;

struct AssetNode {
	AssetNode(Resource* resource, bool is_open = false, bool is_selected = false) : resource(resource), is_open(is_open), is_selected(is_selected) {};
	~AssetNode() {};
	
	bool is_open = false;
	bool is_selected = false;
	Resource* resource = nullptr;
};

class PanelAssets : public Panel
{
public:
	PanelAssets(bool active = true);
	~PanelAssets();

	void Draw() override;
	void Shortcuts() override;

	void Save(Config* config) const override;
	void Load(Config* config) override;

private:
	// ------------------------------
	// --- HIERARCHY CHILD ---
	void DrawChildHierarchy();
	void LoadHierarchyNodes();
	void DrawHierarchyNode(AssetNode* node);

	int FindAsset(UID id);

	void CreateAsset(int type, AssetNode* parent);
	void DeleteAsset(int index);

	void UpdateHierachy();

	// ------------------------------
	// --- EXPLORER CHILD ---
	void DrawChildExplorer();
	void DrawExplorerNode();


private:
	std::vector<AssetNode*> hierarchy_nodes;
	int current_node = 0;
};

