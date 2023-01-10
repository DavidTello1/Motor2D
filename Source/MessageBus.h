#pragma once
#include "Globals.h"
#include "Message.h"

#include <map>
#include <list>
#include <typeindex>

class HandlerFunctionBase
{
public:
    void Exec(Message* e) { Call(e); } // Call the member function

private:
    virtual void Call(Message* e) = 0; // Implemented by MemberFunctionHandler
};

template<class T, class EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
    typedef void (T::* MemberFunction)(EventType*);
    MemberFunctionHandler(T* inst, MemberFunction function) : instance(inst), memberFunction(function) {};

    void Call(Message* e) { 
        (instance->*memberFunction)(static_cast<EventType*>(e)); // Cast event to the correct type and call member function
    } 

private:
    T* instance; // Pointer to class instance
    MemberFunction memberFunction; // Pointer to member function
};

//---------------------------
typedef std::list<HandlerFunctionBase*> HandlerList;
class MessageBus
{
public:
    MessageBus() {};
    ~MessageBus() {
        for (auto it = subscribers.rbegin(); it != subscribers.rend(); ++it)
        {
            HandlerList* list = (*it).second;
            if (list != nullptr)
            {
                for (auto it2 = list->rbegin(); it2 != list->rend(); ++it2)
                {
                    RELEASE(*it2);
                }
                list->clear();
            }
            RELEASE((*it).second);
        }
        subscribers.clear();
    }


    template<typename EventType>
    void Publish(EventType* evnt) 
    {
        HandlerList* handlers = subscribers[typeid(EventType)];

        if (handlers != nullptr)
        {
            for (auto& handler : *handlers)
            {
                if (handler != nullptr)
                    handler->Exec(evnt);
            }
        }

        delete evnt;
    }

    template<class T, class EventType>
    void Subscribe(T* instance, void (T::* memberFunction)(EventType*)) 
    {
        HandlerList* handlers = subscribers[typeid(EventType)];

        if (handlers == nullptr)  //First time initialization
        {
            handlers = new HandlerList();
            subscribers[typeid(EventType)] = handlers;
        }

        handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
    }

private:
    std::map<std::type_index, HandlerList*> subscribers;
};