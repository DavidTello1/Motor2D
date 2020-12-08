#pragma once
#include "Panel.h"
#include "AssetNode.h"

#define VERY_BIG_SIZE 96
#define BIG_SIZE 64
#define MEDIUM_SIZE 48
#define SMALL_SIZE 32

class Assets : public Panel
{
public:
	Assets();
	~Assets();

	void Draw() override;
	void Shortcuts() override;

	void ChildHierarchy();
	void ChildIcons();

	void DrawHierarchy(AssetNode& node);
	void DrawNodeIcon(AssetNode& node);
	void DrawNodeList(AssetNode& node);

	//void ImportAsset(const PathNode& node);
	//Resource* GetSelectedResource();

private:
	// --- MAIN HELPERS ---
	AssetNode* HandleSelection(AssetNode& node); //selection states
	bool DrawRightClick();
	void UpdateAssets(); //update files (called only on return focus)
	void DrawPath(); //draw path with links

	// --- NODES ---
	AssetNode* CreateNode(std::string name = "", AssetNode* parent = nullptr);
	void DeleteNodes(std::vector<AssetNode*> nodes_list);

	AssetNode* GetNode(const std::string path) const; //get node from name
	std::vector<AssetNode*> GetParents(AssetNode& node) const; //get all parents until root
	uint GetNumParents(AssetNode& node) const; //get number of parents
	AssetNode::NodeType GetType(const AssetNode& node) const; //get node type
	std::string GetNameWithCount(const std::string name) const; //get name with count
	std::string GetIconList(const AssetNode::NodeType type) const; //get icon for list view mode
	ImVec4 GetIconColor(const AssetNode::NodeType type) const; //get icon color for list view mode

	bool IsChildOf(const AssetNode& node, AssetNode& child) const; //check if node is child or child of childs of parent
	int FindNode(const AssetNode& node, const std::vector<AssetNode*> list) const; //get node pos in list (returns -1 if not found)
	void UpdatePath(AssetNode& node, const std::string path) const; //update path

	void SelectAll() const;
	void UnSelectAll();
	void Cut(AssetNode& node, AssetNode& parent) const; //paste cut nodes
	void Copy(AssetNode& node, AssetNode& parent); //paste copied nodes

	// --- FILES ---
	AssetNode* GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr); //filter if you only want specific extensions or ignore if you want to ignore specific extensions
	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	bool CheckExtension(const char* path, std::vector<std::string> extensions) const; //check if extension matches any of the list
	
	// --- OTHERS ---
	void DockSpace();

public:
	static const uint default_width = 1280;
	static const uint default_height = 323;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 701;

private:
	std::vector<AssetNode*> nodes;
	std::vector<AssetNode*> selected_nodes;
	std::vector<AssetNode*> aux_nodes; //used for cut and copy
	std::vector<AssetNode*> current_list; //used for selectall and unselectall (can be nodes or current_folder.childs)

	AssetNode* root = nullptr;
	AssetNode* current_folder = nullptr;
	AssetNode* rename_node = nullptr; //used for handling selection

	bool is_delete_popup = true; //used for showing popup delete
	bool is_engine_focus = true; //used for updating assets
	bool is_init = false; //used for dockspace
	bool is_list_view = false;
	bool is_search = false;
	bool is_arrow_hover = false; //used for drag&drop scroll (HierarchyChild)
	bool is_delete = false;
	bool is_cut = false;
	bool is_copy = false;
	bool is_rename_flag = true;

	static ImGuiTextFilter searcher;
	AssetNode::NodeType filter = AssetNode::NodeType::NONE;

	float node_size = 0.0f;
	uint icon_size = BIG_SIZE;
	ImVec4 bg_color = ImVec4(0.4f, 0.7f, 1.0f, 0.0f);
	ImVec4 border_color = ImVec4(0.0f, 0.4f, 0.8f, 0.0f);};