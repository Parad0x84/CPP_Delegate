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


#if !defined(DelegateKey)
    #define DelegateKey size_t
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

    explicit DelegateEntryImplLambda(const LambdaType& fn)
        : Lambda(fn)
    {
        static_assert(std::is_same_v<decltype(Lambda(std::declval<ParamTypes>()...)), RetValType>,
            "Lambda needs to have same return type!");
    }

    virtual RetValType Execute(ParamTypes... params) override
    {
        return Lambda(params...);
    }


private:
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

    using GlobalFuncType = RetValType(*)(ParamTypes...);


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
    void BindLambda(const LambdaType& fn)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<LambdaType, RetValType, ParamTypes...>(fn);
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










template<typename RetValType, typename... ParamTypes>
struct EntryWrapper
{
    EntryWrapper() = delete;

    EntryWrapper(const EntryWrapper& other) = default;
    EntryWrapper& operator=(const EntryWrapper& other) = default;

    EntryWrapper(const DelegateKey inID, IDelegateEntry<RetValType, ParamTypes...>* inEntry)
        : ID(inID), Entry(inEntry)  { }


public:
    DelegateKey ID;
    IDelegateEntry<RetValType, ParamTypes...>* Entry;
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

    using GlobalFuncType = RetValType(*)(ParamTypes...);


public:
    MultiDelegate() = default;
    MultiDelegate(const MultiDelegate& other) = delete;
    MultiDelegate& operator=(const MultiDelegate& other) = delete;


    NODISCARD bool HasAnyListeners() const { return Entries.size(); }

    NODISCARD bool IsBound(const DelegateKey inKey) const
    {
        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            if(entry.ID == inKey)
                return true;

        return false;
    }


    template<typename ObjectType>
    DelegateKey AddObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.emplace_back(GetNewID(), new DelegateEntryImpl<ObjectType, RetValType, ParamTypes...>(object, fn));
        return Entries.back().ID;
    }

    template<typename ObjectType>
    DelegateKey AddObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.emplace_back(GetNewID(), new DelegateEntryImplConst<ObjectType, RetValType, ParamTypes...>(object, fn));
        return Entries.back().ID;
    }


    template<typename LambdaType>
    DelegateKey AddLambda(const LambdaType& fn)
    {
        Entries.emplace_back(GetNewID(), new DelegateEntryImplLambda<LambdaType, RetValType, ParamTypes...>(fn));
        return Entries.back().ID;
    }


    void Remove(const DelegateKey inKey)
    {
        for(size_t i = 0; i < Entries.size(); i++)
        {
            if(Entries[i].ID == inKey)
            {
                delete Entries[i].Entry;
                Entries.erase(Entries.begin() + i);
                return;
            }
        }
    }

    void Clear()
    {
        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            delete entry.Entry;

        Entries.clear();
    }


    void Broadcast(ParamTypes... params)
    {
        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            entry.Entry->Execute(params...);
    }

    template<typename T = RetValType, std::enable_if_t<!std::is_void_v<T>>* = nullptr>
    std::vector<RetValType> BroadcastRetVal(ParamTypes... params)
    {
        std::vector<RetValType> temp;
        temp.reserve(Entries.size());

        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            temp.push_back(entry.Entry->Execute(params...));

        return temp;
    }


private:
    NODISCARD DelegateKey GetNewID()  { return CurrentID++; }

    DelegateKey CurrentID = 0;
    std::vector<EntryWrapper<RetValType, ParamTypes...>> Entries;
};
