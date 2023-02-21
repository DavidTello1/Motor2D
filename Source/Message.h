#pragma once
//typedef void* SDL_GLContext;

struct AssetNode;

class Message 
{
protected:
    virtual ~Message() {};
};

//------------------------
struct OnCloseEngine : public Message {};
struct OnOpenPanelConfiguration : public Message {};
struct OnResourcesImported : public Message {};
struct OnPanelAssetsHistoryBackward : public Message {};
struct OnPanelAssetsHistoryForward : public Message {};

struct OnChangedPanelAssetsCurrentNode : public Message
{
    OnChangedPanelAssetsCurrentNode(AssetNode* node) : node(node) {};

public:
    AssetNode* node;
};

struct OnHidePanelAssetsHierarchy : public Message
{
    OnHidePanelAssetsHierarchy(bool is_hidden) : is_hidden(is_hidden) {};

public:
    bool is_hidden;
};

struct OnAddLog : public Message 
{
    OnAddLog(char type, const char* message) : type(type), message(message) {};

public:
    char type;
    const char* message;
};