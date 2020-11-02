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

	void Draw() override;
	void Shortcuts() override {};

	void DrawHierarchy(AssetNode* node);
	void DrawNode(AssetNode* node);

	//void ImportAsset(const PathNode& node);
	//Resource* GetSelectedResource();

private:
	// --- MAIN HELPERS ---
	AssetNode* HandleSelection(AssetNode* node); //selection states
	bool DrawRightClick();
	void UpdateAssets();

	// --- NODES ---
	AssetNode* CreateNode(std::string name = "", AssetNode* parent = nullptr);
	void DeleteNodes(std::vector<AssetNode*> nodes_list);

	AssetNode* GetNode(std::string name); //get node from name
	std::vector<AssetNode*> GetParents(AssetNode* node); //get all parents until root
	AssetNode::NodeType GetType(AssetNode* node); //get node type
	std::string GetNameWithCount(std::string name); //get name with count
	int FindNode(AssetNode* node, std::vector<AssetNode*> list); //get node pos in list (returns -1 if not found)
	void UpdatePath(AssetNode* node, std::string path); //update path

	void SelectAll();
	void UnSelectAll();
	void Cut(AssetNode* node, AssetNode* parent); //paste cut nodes
	void Copy(AssetNode* node, AssetNode* parent); //paste copied nodes

	// --- FILES ---
	AssetNode* GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr); //filter if you only want specific extensions or ignore if you want to ignore specific extensions
	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool CheckExtension(const char* path, std::vector<std::string> extensions) const; //check if extension matches any of the list
	
	//void UpdateFilters(PathNode& node);
	//void FilterFolders(PathNode& node, PathNode& parent);

public:
	static const uint default_width = 1280;
	static const uint default_height = 323;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 701;

private:
	// --- MAIN VARIABLES ---
	AssetNode* rename_node = nullptr; //used for handling selection
	std::vector<AssetNode*> aux_nodes; //used for cut and copy

	bool is_any_hover = false;
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

	float size = 0.0f;
	uint icon_size = BIG_SIZE;
	ImVec4 bg_color = ImVec4(0.4f, 0.7f, 1.0f, 0.0f);
	ImVec4 border_color = ImVec4(0.0f, 0.4f, 0.8f, 0.0f);

};