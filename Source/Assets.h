#pragma once
#include "Panel.h"
#include "AssetNode.h"

#define VERY_BIG_SIZE 84
#define BIG_SIZE 64
#define MEDIUM_SIZE 50
#define SMALL_SIZE 32

class Assets : public Panel
{
public:
	Assets();
	virtual ~Assets();

	void Draw();
	//void Shortcuts();

	void DrawHierarchy(AssetNode* node);
	void DrawNode(AssetNode* node);
	AssetNode* CreateNode(std::string name = "", AssetNode* parent = nullptr);
	void DeleteNodes(std::vector<AssetNode*> nodes_list);
	void SelectAll();
	void UnSelectAll();

	//void ImportAsset(const PathNode& node);
	//Resource* GetSelectedResource();

private:
	// --- MAIN HELPERS ---
	AssetNode* HandleSelection(AssetNode* node); //selection states
	bool DrawRightClick();
	void UpdateAssets();

	// --- NODES ---
	AssetNode* GetNode(std::string name); //get node from name
	int FindNode(AssetNode* node, std::vector<AssetNode*> list); //get node pos in list (returns -1 if not found)
	std::string GetNameWithCount(std::string name); //get name with count
	AssetNode::NodeType GetType(AssetNode* node); //get node type
	std::vector<AssetNode*> GetParents(AssetNode* node); //get all parents until root
	void UpdatePath(AssetNode* node, std::string path); //update path

	void Cut(AssetNode* node, AssetNode* parent); //paste cut nodes
	void Copy(AssetNode* node, AssetNode* parent); //paste copied nodes


	// --- FILES ---
	AssetNode* GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr); //filter if you only want specific extensions or ignore if you want to ignore specific extensions
	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool CheckExtension(const char* path, std::vector<std::string> extensions) const; //check if extension matches any of the list

	void OrderNodesBy(uint Order) {	order = Order; } //type = 0, name = 1, date = 2
	
	//void UpdateFilters(PathNode& node);
	//void FilterFolders(PathNode& node, PathNode& parent);

public:
	static const uint default_width = 1280;
	static const uint default_height = 323;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 701;

private:
	// --- MAIN VARIABLES ---
	bool is_any_hover = false;
	AssetNode* rename_node = nullptr; //used for handling selection
	std::vector<AssetNode*> aux_nodes; //used for cut and copy

	bool is_cut = false;
	bool is_copy = false;

	// --- HIERARCHY VARIABLES ---
	static ImGuiTextFilter Searcher;
	AssetNode* root = nullptr;
	bool is_search = false;

	// --- NODES VARIABLES ---
	std::vector<AssetNode*> nodes;
	std::vector<AssetNode*> selected_nodes;
	AssetNode* current_folder = nullptr;

	bool is_list_view = false;
	bool is_ascending_order = true;
	uint order = 0; // 0 = type, 1 = name, 2 = date

	float size = 0.0f;
	uint icon_size = BIG_SIZE;
	ImVec4 bg_color = ImVec4(0.4f, 0.7f, 1.0f, 0.0f);
	ImVec4 border_color = ImVec4(0.0f, 0.4f, 0.8f, 0.0f);

};