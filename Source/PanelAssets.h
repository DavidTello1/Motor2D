#pragma once
#include "Panel.h"

#include "AssetNode.h"

struct OnResourcesImported;
struct OnChangedPanelAssetsCurrentNode;

class AssetsHierarchy;
class AssetsExplorer;

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
	void InitDockspace();
	void LoadAssetsNodes(const char* path, AssetNode* parent, int parents);

	void SetCurrentNode(AssetNode* node);

	// --- MESSAGES ---
	void OnResourcesImported(OnResourcesImported* m) { /*hierarchy_nodes = LoadHierarchyNodes(ASSETS_FOLDER, 0);*/ } //*** only on reload resources
	void OnCurrentNodeChanged(OnChangedPanelAssetsCurrentNode* m);

private:
	bool is_init = false;

	// --- Hierarchy
	AssetsHierarchy* hierarchy = nullptr;
	std::vector<AssetNode*> hierarchy_nodes;
	AssetNode* current_node = nullptr;
	AssetNode* root = nullptr;

	// --- Explorer
	AssetsExplorer* explorer = nullptr;
	std::vector<AssetNode*> nodes_list; // all resources
	std::vector<int> selected_nodes;
	int last_selected = -1;
	int rename_node = -1;
};

