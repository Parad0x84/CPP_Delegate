#pragma once


#include <vector>
#include <type_traits>




#if !defined(DELEGATE_ASSERT)
    #include <cassert>
    #define DELEGATE_ASSERT(expr) assert(expr)
#endif

#if defined(DELEGATE_NO_NODISCARD)
    #define NODISCARD 
#else
    #define NODISCARD [[nodiscard]]
#endif





template<typename RetValType, typename... ParamTypes>
class IDelegateEntry
{
public:
    IDelegateEntry() = default;
    IDelegateEntry(const IDelegateEntry& other) = delete;
    IDelegateEntry& operator=(const IDelegateEntry& other) = delete;

    virtual ~IDelegateEntry() = default;

    virtual RetValType Execute(ParamTypes... params) = 0;
    NODISCARD virtual void* GetObjectPtr() const = 0;
};



template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryImpl : public IDelegateEntry<RetValType, ParamTypes...>
{
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);


public:
    DelegateEntryImpl() = delete;
    DelegateEntryImpl(const DelegateEntryImpl& other) = delete;
    DelegateEntryImpl& operator=(const DelegateEntryImpl& other) = delete;

    DelegateEntryImpl(ObjectType* object, const FuncType& fn)
        : Object(object), Function(fn)  { }


    virtual RetValType Execute(ParamTypes... params) override
    {
        return (Object->*Function)(params...);
    }

    NODISCARD virtual void* GetObjectPtr() const override
    {
        return Object;
    }


private:
    ObjectType* Object = nullptr;
    FuncType Function = nullptr;
};



template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryImplConst : public IDelegateEntry<RetValType, ParamTypes...>
{
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


public:
    DelegateEntryImplConst() = delete;
    DelegateEntryImplConst(const DelegateEntryImplConst& other) = delete;
    DelegateEntryImplConst& operator=(const DelegateEntryImplConst& other) = delete;

    DelegateEntryImplConst(ObjectType* object, const ConstFuncType& fn)
        : Object(object), Function(fn)  { }


    virtual RetValType Execute(ParamTypes... params) override
    {
        return (Object->*Function)(params...);
    }

    NODISCARD virtual void* GetObjectPtr() const override
    {
        return Object;
    }


private:
    ObjectType* Object = nullptr;
    ConstFuncType Function = nullptr;
};



template<typename LambdaType, typename RetValType, typename... ParamTypes>
class DelegateEntryImplLambda : public IDelegateEntry<RetValType, ParamTypes...>
{
public:
    DelegateEntryImplLambda() = delete;
    DelegateEntryImplLambda(const DelegateEntryImplLambda& other) = delete;
    DelegateEntryImplLambda& operator=(const DelegateEntryImplLambda& other) = delete;

    DelegateEntryImplLambda(void* owner, const LambdaType& fn)
        : Owner(owner), Lambda(fn)  { }


    virtual RetValType Execute(ParamTypes... params) override
    {
        static_assert(std::is_same_v<decltype(Lambda(params...)), RetValType>, "Lambda needs to have same return type!");

        return Lambda(params...);
    }

    NODISCARD virtual void* GetObjectPtr() const override
    {
        return Owner;
    }


private:
    void* Owner = nullptr;
    LambdaType Lambda;
};










template<typename FuncSignature>
class Delegate;



template<typename RetValType, typename... ParamTypes>
class Delegate<RetValType(ParamTypes...)>
{
    template<typename ObjectType>
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


public:
    Delegate() = default;
    Delegate(const Delegate& other) = delete;
    Delegate& operator=(const Delegate& other) = delete;


    NODISCARD bool IsBound() const { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImpl<ObjectType, RetValType, ParamTypes...>(object, fn);
    }

    template<typename ObjectType>
    void BindObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImplConst<ObjectType, RetValType, ParamTypes...>(object, fn);
    }


    template<typename LambdaType>
    void BindLambda(void* owner, const LambdaType& fn)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<LambdaType, RetValType, ParamTypes...>(owner, fn);
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

    bool ExecuteIfBound(ParamTypes... params)
    {
        if(!IsBound())
            return false;

        Execute(params...);
        return true;
    }


private:
    IDelegateEntry<RetValType, ParamTypes...>* Entry = nullptr;
};










template<typename FuncSignature>
class MultiDelegate;

template<typename RetValType, typename... ParamTypes>
class MultiDelegate<RetValType(ParamTypes...)>
{
    template<typename ObjectType>
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);

    template<typename ObjectType>
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


public:
    MultiDelegate() = default;
    MultiDelegate(const MultiDelegate& other) = delete;
    MultiDelegate& operator=(const MultiDelegate& other) = delete;


    NODISCARD bool HasAnyListeners() const { return Entries.size(); }

    NODISCARD bool IsBound(const void* object) const
    {
        for(const IDelegateEntry<RetValType, ParamTypes...>* entry : Entries)
            if(entry->GetObjectPtr() == object)
                return true;

        return false;
    }


    template<typename ObjectType>
    void AddObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.push_back(new DelegateEntryImpl<ObjectType, RetValType, ParamTypes...>(object, fn));
    }

    template<typename ObjectType>
    void AddObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.push_back(new DelegateEntryImplConst<ObjectType, RetValType, ParamTypes...>(object, fn));
    }


    template<typename LambdaType>
    void AddLambda(void* owner, const LambdaType& fn)
    {
        Entries.push_back(new DelegateEntryImplLambda<LambdaType, RetValType, ParamTypes...>(owner, fn));
    }


    void Remove(const void* object)
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
        for(IDelegateEntry<RetValType, ParamTypes...>* entry : Entries)
            delete entry;

        Entries.clear();
    }


    void Broadcast(ParamTypes... params)
    {
        for(IDelegateEntry<RetValType, ParamTypes...>* entry : Entries)
            entry->Execute(params...);
    }

    template<typename T = RetValType, std::enable_if_t<!std::is_void_v<T>>* = nullptr>
    std::vector<RetValType> BroadcastRetVal(ParamTypes... params)
    {
        std::vector<RetValType> temp;
        temp.reserve(Entries.size());

        for(IDelegateEntry<RetValType, ParamTypes...>* entry : Entries)
            temp.push_back(entry->Execute(params...));

        return temp;
    }


private:
    std::vector<IDelegateEntry<RetValType, ParamTypes...>*> Entries;
};
