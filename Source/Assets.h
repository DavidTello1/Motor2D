#pragma once
#include "Panel.h"
#include "AssetNode.h"
//#include "ModuleResources.h"

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

	void Save(Config* config) const override;
	void Load(Config* config) override;

	void ChildHierarchy();
	void ChildIcons();

	void DrawHierarchy(size_t index);
	void DrawNodeIcon(size_t index);
	void DrawNodeList(size_t index);


	//void ImportAsset(const PathNode& node);
	//Resource* GetSelectedResource();

private:
	// --- MAIN HELPERS ---
	void HandleSelection(size_t index); //selection states
	bool DrawRightClick();
	void UpdateAssets(); //update files (called only on return focus)
	void DrawPath(); //draw path with links

	// --- NODES ---
	size_t CreateNode(std::string name = "", size_t parent_index = 0);
	void DeleteNodes(std::vector<std::string> names_list);

	std::vector<std::string> GetParents(size_t index) const; //get all parents until root
	uint GetNumParents(size_t index) const; //get number of parents
	std::string GetNameWithCount(const std::string name) const; //get name with count
	std::string GetIconList(const ResourceType type) const; //get icon for list view mode
	ImVec4 GetIconColor(const ResourceType type) const; //get icon color for list view mode

	bool IsChildOf(const size_t node, const char* child_name) const; //check if node is child or child of childs of parent
	int FindNode(const char* name, std::vector<std::string> list) const; //get node pos in list (returns -1 if not found)
	void UpdatePath(size_t index, std::string path, std::string parent); //update path

	void SetState(std::string name, State state);
	void SelectAll();
	void UnSelectAll();
	void Cut(size_t node, size_t parent); //paste cut nodes
	void Copy(size_t node, size_t parent); //paste copied nodes
	
	// --- INTERNAL ---
	void DockSpace();
	void Scroll(ImVec2 pos);
	void DeletePopup();
	void Rename(size_t index, ImVec2 pos);

public:
	static const uint default_width = 1280;
	static const uint default_height = 323;
	static const uint default_pos_x = 0;
	static const uint default_pos_y = 701;

private:
	AssetNode nodes;
	size_t current_folder;

	std::vector<std::string> selected_nodes;
	std::vector<std::string> current_list; //used for filters
	std::vector<std::string> aux_nodes; //used for cut and copy

	bool is_delete_popup = true; //used for showing popup delete
	bool is_engine_focus = true; //used for updating assets
	bool is_init = false; //used for dockspace

	bool is_list_view = false;
	bool is_search = false;

	bool is_arrow_hover = false; //used for drag&drop scroll (HierarchyChild)
	bool is_any_hover = false; //used for unselecting all when clicking on empty
	bool is_delete = false;
	bool is_cut = false;
	bool is_copy = false;
	bool is_rename_flag = true;

	static ImGuiTextFilter searcher;
	ResourceType filter = ResourceType::UNKNOWN;

	// Node Visualization Variables
	float node_size = 0.0f;
	uint icon_size = BIG_SIZE;
	ImVec4 bg_color = ImVec4(0.4f, 0.7f, 1.0f, 0.0f);
	ImVec4 border_color = ImVec4(0.0f, 0.4f, 0.8f, 0.0f);};