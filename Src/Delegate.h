#pragma once


#include <functional>




template<typename... ParamTypes>
class IDelegateEntry
{
public:
    virtual ~IDelegateEntry() = default;
    virtual void Execute(ParamTypes... params) = 0;

    virtual void* GetObjectPtr() = 0;
};


template<typename ObjectType, typename... ParamTypes>
class DelegateEntryImpl : public IDelegateEntry<ParamTypes...>
{
public:
    DelegateEntryImpl() = delete;
    DelegateEntryImpl(ObjectType* object, void(ObjectType::* fn)(ParamTypes...))
        : Object(object), Function(fn)  { }

    virtual void Execute(ParamTypes... params) override
    {
        (Object->*Function)(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Object;
    }


private:
    ObjectType* Object = nullptr;
    void(ObjectType::* Function)(ParamTypes...) = nullptr;
};


template<typename... ParamTypes>
class DelegateEntryImplLambda : public IDelegateEntry<ParamTypes...>
{
public:
    DelegateEntryImplLambda() = delete;
    explicit DelegateEntryImplLambda(void* owner, const std::function<void(ParamTypes...)>& fn)
        : Owner(owner), Lambda(fn) { }

    virtual void Execute(ParamTypes... params) override
    {
        Lambda(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Owner;
    }


private:
    void* Owner = nullptr;
    std::function<void(ParamTypes...)> Lambda;
};




template<typename... ParamTypes>
class Delegate
{
public:
    [[nodiscard]] bool IsBound() const  { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, void(ObjectType::* fn)(ParamTypes...))
    {
        delete Entry;
        Entry = new DelegateEntryImpl<ObjectType, ParamTypes...>(object, fn);
    }

    void BindLambda(void* owner, const std::function<void(ParamTypes...)>& fn)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<ParamTypes...>(owner, fn);
    }

    void Unbind()
    {
        delete Entry;
        Entry = nullptr;
    }

    void Execute(ParamTypes... params)
    {
        Entry->Execute(params...);
    }

    void ExecuteIfBound(ParamTypes... params)
    {
        if(IsBound())
            Execute(params...);
    }


private:
    IDelegateEntry<ParamTypes...>* Entry = nullptr;
};







template<typename RetValType, typename... ParamTypes>
class IDelegateEntryRetVal
{
public:
    virtual ~IDelegateEntryRetVal() = default;
    virtual RetValType Execute(ParamTypes... params) = 0;

    virtual void* GetObjectPtr() = 0;
};


template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryRetValImpl : public IDelegateEntryRetVal<RetValType, ParamTypes...>
{
public:
    DelegateEntryRetValImpl() = delete;
    DelegateEntryRetValImpl(ObjectType* object, RetValType(ObjectType::* fn)(ParamTypes...))
        : Object(object), Function(fn)  { }

    virtual RetValType Execute(ParamTypes... params) override
    {
        return (Object->*Function)(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Object;
    }


private:
    ObjectType* Object = nullptr;
    RetValType(ObjectType::* Function)(ParamTypes...) = nullptr;
};



template<typename RetValType, typename... ParamTypes>
class DelegateEntryRetValImplLambda : public IDelegateEntryRetVal<RetValType, ParamTypes...>
{
public:
    DelegateEntryRetValImplLambda() = delete;
    DelegateEntryRetValImplLambda(void* owner, const std::function<RetValType(ParamTypes...)>& fn)
        : Owner(owner), Lambda(fn)  { }

    virtual RetValType Execute(ParamTypes... params) override
    {
        return Lambda(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Owner;
    }


private:
    void* Owner = nullptr;
    std::function<RetValType(ParamTypes...)> Lambda;
};



template<typename RetValType, typename... ParamTypes>
class DelegateRetVal
{
public:
    [[nodiscard]] bool IsBound() const { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, RetValType(ObjectType::* fn)(ParamTypes...))
    {
        delete Entry;
        Entry = new DelegateEntryRetValImpl<ObjectType, RetValType, ParamTypes...>(object, fn);
    }

    void BindLambda(void* owner, const std::function<RetValType(ParamTypes...)>& fn)
    {
        delete Entry;
        Entry = new DelegateEntryRetValImplLambda<RetValType, ParamTypes...>(owner, fn);
    }

    void Unbind()
    {
        delete Entry;
        Entry = nullptr;
    }

    RetValType Execute(ParamTypes... params)
    {
        return Entry->Execute(params...);
    }

    void ExecuteIfBound(ParamTypes... params)
    {
        if(IsBound())
            Execute(params...);
    }

    RetValType ExecuteIfBoundWithDefaultReturn(ParamTypes... params)
    {
        if(IsBound())
            return Execute(params);

        return {};
    }


private:
    IDelegateEntryRetVal<RetValType, ParamTypes...>* Entry = nullptr;
};






template<typename... ParamTypes>
class MultiDelegate
{
public:
    [[nodiscard]] bool HasAnyListeners() const { return Entries.size(); }

    template<typename ObjectType>
    [[nodiscard]] bool IsBound(ObjectType* object) const
    {
        for(IDelegateEntry<ParamTypes...>* entry : Entries)
            if(entry->GetObjectPtr() == object)
                return true;

        return false;
    }


    template<typename ObjectType>
    void AddObject(ObjectType* object, void(ObjectType::* fn)(ParamTypes...))
    {
        Entries.push_back(new DelegateEntryImpl<ObjectType, ParamTypes...>(object, fn));
    }

    void AddLambda(void* owner, const std::function<void(ParamTypes...)>& fn)
    {
        Entries.push_back(new DelegateEntryImplLambda<ParamTypes...>(owner, fn));
    }

    void Remove(void* object)
    {
        for(size_t i = 0; i < Entries.size(); i++)
        {
            if(Entries[i]->GetObjectPtr() == object)
            {
                delete Entries[i];
                Entries.erase(Entries.begin() + i);
                return;
            }
        }
    }

    void Clear()
    {
        for(IDelegateEntry<ParamTypes...>* entry : Entries)
            delete entry;

        Entries.clear();
    }

    void Broadcast(ParamTypes... params)
    {
        for(IDelegateEntry<ParamTypes...>* entry : Entries)
            entry->Execute(params...);
    }


private:
    std::vector<IDelegateEntry<ParamTypes...>*> Entries;
};



template<typename RetValType, typename... ParamTypes>
class MultiDelegateRetVal
{
public:
    [[nodiscard]] bool HasAnyListeners() const { return Entries.size(); }

    template<typename ObjectType>
    [[nodiscard]] bool IsBound(ObjectType* object) const
    {
        for(IDelegateEntryRetVal<RetValType, ParamTypes...>* entry : Entries)
            if(entry->GetObjectPtr() == object)
                return true;

        return false;
    }


    template<typename ObjectType>
    void AddObject(ObjectType* object, RetValType(ObjectType::* fn)(ParamTypes...))
    {
        Entries.push_back(new DelegateEntryRetValImpl<ObjectType, RetValType, ParamTypes...>(object, fn));
    }

    void AddLambda(void* owner, const std::function<RetValType(ParamTypes...)>& fn)
    {
        Entries.push_back(new DelegateEntryRetValImplLambda<RetValType, ParamTypes...>(owner, fn));
    }

    template<typename ObjectType>
    void Remove(ObjectType* object)
    {
        for(size_t i = 0; i < Entries.size(); i++)
        {
            if(Entries[i]->GetObjectPtr() == object)
            {
                delete Entries[i];
                Entries.erase(Entries.begin() + i);
                return;
            }
        }
    }

    void Clear()
    {
        for(IDelegateEntryRetVal<RetValType, ParamTypes...>* entry : Entries)
            delete entry;

        Entries.clear();
    }

    const std::vector<RetValType>& Broadcast(ParamTypes... params)
    {
        LastReturnValues.clear();

        for(IDelegateEntryRetVal<RetValType, ParamTypes...>* entry : Entries)
            LastReturnValues.push_back(entry->Execute(params...));

        return LastReturnValues;
    }


private:
    std::vector<IDelegateEntryRetVal<RetValType, ParamTypes...>*> Entries;
    std::vector<RetValType> LastReturnValues;
};
