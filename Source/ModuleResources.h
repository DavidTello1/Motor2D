#pragma once
#include "Module.h"

#include "Resource.h"
#include "ResourceManager.h"

#include <array>

struct ResourceTexture;

class LoaderTexture;

struct ResourceHandle {
    UID id = 0;
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
    UID ImportResource(const char* path, const char* name, const char* extension);
    bool RemoveResource(UID id);

    bool LoadResource(UID id);
    bool UnloadResource(UID id);

    Resource* GetResource(UID id);
    const Resource* GetResource(UID id) const;

    bool SaveResource(UID id, Resource& resource);

    int GetReferenceCount(UID id) const;

    //------------------------------
private:
    int Find(UID id) const;

    ResourceType GetResourceType(std::string name) const;
    const char* GetLibraryPath(ResourceType type, const char* name) const;
    bool AddResource(ResourceType type, Resource& resource);

    //void CleanLibrary();

    //void CreateMeta();
    //void DeleteMeta();
    //void SaveMeta();
    //void LoadFromMeta(const char* path);

private:
    static constexpr int RESERVED_RESOURCES = 11; // Icons for Panel Assets (10) + Engine Icon (1)
    static constexpr int MAX_TEXTURES = 1000;
    static constexpr int TOTAL_MAX_RESOURCES = MAX_TEXTURES; // Max Resources in total

    // --- Resource Handlers ---
    std::array<ResourceHandle, TOTAL_MAX_RESOURCES> resources;
    uint num_resources = 0;

    // --- Resource Managers ---
    //ResourceManager<ResourceFolder>*    folder_mgr = nullptr;
    ResourceManager<ResourceTexture>*   texture_mgr = nullptr;
    //ResourceManager<ResourceAudio>*     audio_mgr = nullptr;
    //ResourceManager<ResourceTextfile>*  textfile_mgr = nullptr;
    //ResourceManager<ResourceScript>*    script_mgr = nullptr;
    //ResourceManager<ResourceShader>*    shader_mgr = nullptr;
    //ResourceManager<ResourceScene>*     scene_mgr = nullptr;
    //ResourceManager<ResourcePrefab>*    prefab_mgr = nullptr;
    //ResourceManager<ResourceMaterial>*  material_mgr = nullptr;
    //ResourceManager<ResourceAnimation>* animation_mgr = nullptr;
    //ResourceManager<ResourceTilemap>*   tilemap_mgr = nullptr;

    // --- Resource Loaders ---
    //LoaderFolder*       folder_loader = nullptr;
    LoaderTexture*      texture_loader = nullptr;
    //LoaderAudio*        audio_loader = nullptr;
    //LoaderTextfile*     textfile_loader = nullptr;
    //LoaderScript*       script_loader = nullptr;
    //LoaderShader*       shader_loader = nullptr;
    //LoaderScene*        scene_loader = nullptr;
    //LoaderPrefab*       prefab_loader = nullptr;
    //LoaderMaterial*     material_loader = nullptr;
    //LoaderAnimation*    animation_loader = nullptr;
    //LoaderTilemap*      tilemap_loader = nullptr;
};
