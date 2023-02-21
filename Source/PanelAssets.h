#pragma once
#include "Panel.h"

#include "AssetNode.h"
#include <array>

struct OnResourcesImported;
struct OnHidePanelAssetsHierarchy;
struct OnChangedPanelAssetsCurrentNode;
struct OnPanelAssetsHistoryBackward;
struct OnPanelAssetsHistoryForward;

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

	void AddHistory(AssetNode* node);
	void HistoryForward();
	void HistoryBackward();

	// --- MESSAGES ---
	void OnResourcesImported(OnResourcesImported* m) { /*hierarchy_nodes = LoadHierarchyNodes(ASSETS_FOLDER, 0);*/ } //*** only on reload resources
	void OnHideHierarchy(OnHidePanelAssetsHierarchy* m);
	void OnCurrentNodeChanged(OnChangedPanelAssetsCurrentNode* m);
	void OnHistoryBackward(OnPanelAssetsHistoryBackward* m) { HistoryBackward(); }
	void OnHistoryForward(OnPanelAssetsHistoryForward* m) { HistoryForward(); }

private:
	static constexpr int MAX_HISTORY_STEPS = 5;

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

	// --- History
	std::array<AssetNode*, MAX_HISTORY_STEPS> history;
	int history_size = 0;
	int head = 0;
	int tail = -1;
	int current_history = 0;
};
