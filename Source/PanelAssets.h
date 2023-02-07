#pragma once
#include "Panel.h"

#include "Globals.h"

#include <vector>
#include <string>

struct OnResourcesImported;

struct AssetNode {
	AssetNode(UID id, const char* name, int type, uint preview_index = 0) : id(id), name(name), type(type), preview_index(preview_index) {};
	~AssetNode() {};
	
	bool is_rename = false;
	bool is_selected = false;

	UID id = 0;
	const char* name = "Default Name";
	int type = -1;
	uint preview_index = 0;
};

struct AssetTree {
	AssetTree(std::string name, AssetTree* parent = nullptr, int num_parents = 0, bool is_open = false, bool is_leaf = true)
		: name(name), parent(parent), num_parents(num_parents), is_open(is_open), is_leaf(is_leaf) {};
	~AssetTree() {};

	bool is_open = false;
	bool is_leaf = true;

	std::string name;
	int num_parents = 0;
	AssetTree* parent = nullptr;
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
	void InitDockspace();
	void SetCurrentNode(AssetTree* node) { current_node = node; UpdateNodePath(); }

	// --- HIERARCHY CHILD ---
	void DrawChildHierarchy();
	void DrawHierarchyMenuBar();
	void DrawHierarchyNode(AssetTree* node, int id);

	std::vector<AssetTree*> LoadHierarchyNodes(const char* path, AssetTree* parent, int parents);
	void UpdateHierachy();


	// --- EXPLORER CHILD ---
	void DrawChildExplorer();
	void DrawExplorerMenuBar();
	void DrawRightClick();
	void DrawExplorerNode(AssetNode* node, int id);

	void UpdateNodePath();


	// --- MESSAGES ---
	void OnResourcesImported(OnResourcesImported* m) { /*hierarchy_nodes = LoadHierarchyNodes(ASSETS_FOLDER, 0);*/ } //*** only on reload resources

private:
	bool is_init = false;
	bool is_hierarchy_hidden = false;
	bool is_copy = false;
	bool is_cut = false;

	// --- Hierarchy
	std::vector<AssetTree*> hierarchy_nodes;
	AssetTree* current_node = nullptr;

	static ImGuiTextFilter searcher;
	int filter = -1;


	// --- Explorer
	std::vector<AssetTree*> menubar_path;
	std::vector<AssetNode*> explorer_nodes;
	std::vector<AssetNode*> selected_nodes;
};

