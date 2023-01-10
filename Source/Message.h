#pragma once
typedef void* SDL_GLContext;

class Message 
{
protected:
    virtual ~Message() {};
};

//------------------------
struct OnCloseEngine : public Message {};
struct OnOpenPanelConfiguration : public Message {};

struct OnAddLog : public Message 
{
    OnAddLog(char type, const char* message) : type(type), message(message) {};

public:
    char type;
    const char* message;
};