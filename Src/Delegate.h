#pragma once


#include <cassert>
#include <vector>




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
    using FuncType = void(ObjectType::* )(ParamTypes...);


    DelegateEntryImpl() = delete;
    DelegateEntryImpl(ObjectType* object, const FuncType& fn)
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
    FuncType Function = nullptr;
};


template<typename ObjectType, typename... ParamTypes>
class DelegateEntryImplConst : public IDelegateEntry<ParamTypes...>
{
public:
    using ConstFuncType = void(ObjectType::*)(ParamTypes...) const;


    DelegateEntryImplConst() = delete;
    DelegateEntryImplConst(ObjectType* object, const ConstFuncType& fn)
        : Object(object), Function(fn)
    {
    }

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
    ConstFuncType Function = nullptr;
};


template<typename LambdaType, typename... ParamTypes>
class DelegateEntryImplLambda : public IDelegateEntry<ParamTypes...>
{
public:
    DelegateEntryImplLambda() = delete;
    explicit DelegateEntryImplLambda(void* owner, const LambdaType& fn)
        : Owner(owner), Lambda(fn) { }

    virtual void Execute(ParamTypes... params) override
    {
        static_assert(std::is_same_v<decltype(Lambda(params...)), void > , "Lambda needs to return void!");

        Lambda(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Owner;
    }


private:
    void* Owner = nullptr;
    LambdaType Lambda = nullptr;
};




template<typename... ParamTypes>
class Delegate
{
public:
    template<typename ObjectType>
    using FuncType = void(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = void(ObjectType::*)(ParamTypes...) const;


    [[nodiscard]] bool IsBound() const  { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        assert(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImpl<ObjectType, ParamTypes...>(object, fn);
    }

    template<typename ObjectType>
    void BindObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        assert(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImplConst<ObjectType, ParamTypes...>(object, fn);
    }

    template<typename LambdaType>
    void BindLambda(void* owner, const LambdaType& fn)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<LambdaType, ParamTypes...>(owner, fn);
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
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);


    DelegateEntryRetValImpl() = delete;
    DelegateEntryRetValImpl(ObjectType* object, const FuncType& fn)
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
    FuncType Function = nullptr;
};


template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryRetValImplConst : public IDelegateEntryRetVal<RetValType, ParamTypes...>
{
public:
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


    DelegateEntryRetValImplConst() = delete;
    DelegateEntryRetValImplConst(ObjectType* object, const ConstFuncType& fn)
        : Object(object), Function(fn)
    {
    }

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
    ConstFuncType Function = nullptr;
};



template<typename LambdaType, typename RetValType, typename... ParamTypes>
class DelegateEntryRetValImplLambda : public IDelegateEntryRetVal<RetValType, ParamTypes...>
{
public:
    DelegateEntryRetValImplLambda() = delete;
    DelegateEntryRetValImplLambda(void* owner, const LambdaType& fn)
        : Owner(owner), Lambda(fn)  { }

    virtual RetValType Execute(ParamTypes... params) override
    {
        static_assert(std::is_same_v<decltype(Lambda(params...)), RetValType >, "Lambda needs to have same return type!");

        return Lambda(params...);
    }

    virtual void* GetObjectPtr() override
    {
        return Owner;
    }


private:
    void* Owner = nullptr;
    LambdaType Lambda;
};



template<typename RetValType, typename... ParamTypes>
class DelegateRetVal
{
public:
    template<typename ObjectType>
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


    [[nodiscard]] bool IsBound() const { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        assert(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryRetValImpl<ObjectType, RetValType, ParamTypes...>(object, fn);
    }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        assert(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryRetValImplConst<ObjectType, RetValType, ParamTypes...>(object, fn);
    }


    template<typename LambdaType>
    void BindLambda(void* owner, const LambdaType& fn)
    {
        delete Entry;
        Entry = new DelegateEntryRetValImplLambda<LambdaType, RetValType, ParamTypes...>(owner, fn);
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
    template<typename ObjectType>
    using FuncType = void(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = void(ObjectType::*)(ParamTypes...) const;


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
    void AddObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        assert(object != nullptr);
        Entries.push_back(new DelegateEntryImpl<ObjectType, ParamTypes...>(object, fn));
    }

    template<typename ObjectType>
    void AddObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        assert(object != nullptr);
        Entries.push_back(new DelegateEntryImplConst<ObjectType, ParamTypes...>(object, fn));
    }


    template<typename LambdaType>
    void AddLambda(void* owner, const LambdaType& fn)
    {
        Entries.push_back(new DelegateEntryImplLambda<LambdaType, ParamTypes...>(owner, fn));
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
    template<typename ObjectType>
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


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
    void AddObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        assert(object != nullptr);
        Entries.push_back(new DelegateEntryRetValImpl<ObjectType, RetValType, ParamTypes...>(object, fn));
    }

    template<typename ObjectType>
    void AddObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        assert(object != nullptr);
        Entries.push_back(new DelegateEntryRetValImplConst<ObjectType, RetValType, ParamTypes...>(object, fn));
    }


    template<typename LambdaType>
    void AddLambda(void* owner, const LambdaType& fn)
    {
        Entries.push_back(new DelegateEntryRetValImplLambda<LambdaType, RetValType, ParamTypes...>(owner, fn));
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
