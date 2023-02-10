#pragma once
#include "Module.h"

#include "Resource.h" // *** For ResourceType enum

#include <string>
#include <vector>

//struct Resource;

//class FolderManager;
class TextureManager;
//class AudioManager;
//class TextfileManager;
//class ScriptManager;
//class ShaderManager;
//class SceneManager;
//class PrefabManager;
//class MaterialManager;
//class AmimationManager;
//class TilemapManager;

struct ResourceHandle {
    UID id = 0;
    const char* path = "";
    int type = -1;
    int index = -1;
};

class ModuleResources : public Module
{
public:
    ModuleResources(bool start_enabled = true);
    ~ModuleResources();

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;

    // ----------------------
    UID ImportResource(const char* path);
    bool RemoveResource(UID id);

    bool LoadResource(UID id);
    bool UnloadResource(UID id);

    Resource* GetResource(UID id);
    const Resource* GetResource(UID id) const;

    bool SaveResource(UID id, Resource& resource);

    int GetReferenceCount(UID id) const;

    //------------------------------
private:
    void ImportAllResources(const char* directory);

    std::vector<std::string> GetAllFiles(const char* directory);
    std::vector<std::string> GetAllFilesOfType(const char* directory, int type);
    std::vector<std::string> GetAllFilesExcept(const char* directory, int type);
    //std::vector<const char*> GetAllFilesOfType(const char* directory, std::vector<int> type);
    //std::vector<const char*> GetAllFilesExcept(const char* directory, std::vector<int> type);

    int Find(UID id) const;
    ResourceType GetResourceType(std::string extension) const;
    const char* GetLibraryPath(ResourceType type, const char* name) const;

    UID CreateResource(const char* path, const char* name = "", int type = -1, UID id = 0, bool save_meta = true);
    UID CreateResourceFromMeta(const char* path, const char* path_meta);
    bool AddResource(Resource& resource);

    //void CleanLibrary();

    //void CreateMeta();
    //void DeleteMeta();
    //void SaveMeta();
    //void LoadFromMeta(const char* path);

private:
    static constexpr int RESERVED_RESOURCES = 11; // Icons for Panel Assets (10) + Engine Icon (1)

    // --- Resource Handlers ---
    std::vector<ResourceHandle> resources;

    // --- Resource Managers ---
    //FolderManager*      folder_mgr = nullptr;
    TextureManager*     texture_mgr = nullptr;
    //AudioManager*       audio_mgr = nullptr;
    //TextfileManager*    textfile_mgr = nullptr;
    //ScriptManager*      script_mgr = nullptr;
    //ShaderManager*      shader_mgr = nullptr;
    //SceneManager*       scene_mgr = nullptr;
    //PrefabManager*      prefab_mgr = nullptr;
    //MaterialManager*    material_mgr = nullptr;
    //AmimationManager*   animation_mgr = nullptr;
    //TilemapManager*     tilemap_mgr = nullptr;
};
