#pragma once


#include <vector>
#include <type_traits>
#include <tuple>




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




template<typename FuncSignature>
class IDelegateEntry;

template<typename RetValType, typename... ParamTypes>
class IDelegateEntry<RetValType(ParamTypes...)>
{
public:
    IDelegateEntry() noexcept = default;
    IDelegateEntry(const IDelegateEntry& other) = delete;
    IDelegateEntry& operator=(const IDelegateEntry& other) = delete;

    virtual ~IDelegateEntry() noexcept = default;

    virtual RetValType Execute(ParamTypes... params) noexcept = 0;
};



template<typename ObjectType, typename FuncSignature, typename... PayloadTypes>
class DelegateEntryImpl;

template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryImpl<ObjectType, RetValType(ParamTypes...)> : public IDelegateEntry<RetValType(ParamTypes...)>
{
    using FuncType = RetValType(ObjectType::*)(ParamTypes...);


public:
    DelegateEntryImpl() = delete;
    DelegateEntryImpl(const DelegateEntryImpl& other) = delete;
    DelegateEntryImpl& operator=(const DelegateEntryImpl& other) = delete;

    DelegateEntryImpl(ObjectType* object, const FuncType& fn) noexcept
        : Object(object), Function(fn)  { }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        return (Object->*Function)(params...);
    }


private:
    ObjectType* Object = nullptr;
    FuncType Function = nullptr;
};



template<typename ObjectType, typename FuncSignature, typename... PayloadTypes>
class DelegateEntryImplConst;

template<typename ObjectType, typename RetValType, typename... ParamTypes>
class DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...)> : public IDelegateEntry<RetValType(ParamTypes...)>
{
    using ConstFuncType = RetValType(ObjectType::*)(ParamTypes...) const;


public:
    DelegateEntryImplConst() = delete;
    DelegateEntryImplConst(const DelegateEntryImplConst& other) = delete;
    DelegateEntryImplConst& operator=(const DelegateEntryImplConst& other) = delete;

    DelegateEntryImplConst(ObjectType* object, const ConstFuncType& fn) noexcept
        : Object(object), Function(fn)  { }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        return (Object->*Function)(params...);
    }


private:
    ObjectType* Object = nullptr;
    ConstFuncType Function = nullptr;
};



template<typename LambdaType, typename FuncSignature, typename... PayloadTypes>
class DelegateEntryImplLambda;

template<typename LambdaType, typename RetValType, typename... ParamTypes>
class DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...)> : public IDelegateEntry<RetValType(ParamTypes...)>
{
public:
    DelegateEntryImplLambda() = delete;
    DelegateEntryImplLambda(const DelegateEntryImplLambda& other) = delete;
    DelegateEntryImplLambda& operator=(const DelegateEntryImplLambda& other) = delete;

    explicit DelegateEntryImplLambda(const LambdaType& fn) noexcept
        : Lambda(fn)
    {
        static_assert(std::is_same_v<decltype(Lambda(std::declval<ParamTypes>()...)), RetValType>,
            "Lambda needs to have same return type!");
    }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        return Lambda(params...);
    }


private:
    LambdaType Lambda;
};










template<typename ObjectType, typename RetValType, typename... ParamTypes, typename... PayloadTypes>
class DelegateEntryImpl<ObjectType, RetValType(ParamTypes...), PayloadTypes...> : public IDelegateEntry<RetValType(ParamTypes...)>
{
    using FuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...);


public:
    DelegateEntryImpl() = delete;
    DelegateEntryImpl(const DelegateEntryImpl& other) = delete;
    DelegateEntryImpl& operator=(const DelegateEntryImpl& other) = delete;

    DelegateEntryImpl(ObjectType* object, const FuncTypePayload& fn, PayloadTypes... payloads) noexcept
        : Object(object), Function(fn), Payloads(std::forward<PayloadTypes>(payloads)...)  { }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        auto executeWithPayload = [&] <typename... T>(T&&... payloadArgs)
        {
            return (Object->*Function)(params..., std::forward<T>(payloadArgs)...);
        };

        return std::apply(executeWithPayload, Payloads);
    }


private:
    ObjectType* Object = nullptr;
    FuncTypePayload Function = nullptr;
    std::tuple<PayloadTypes...> Payloads;
};



template<typename ObjectType, typename RetValType, typename... ParamTypes, typename... PayloadTypes>
class DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...), PayloadTypes...> : public IDelegateEntry<RetValType(ParamTypes...)>
{
    using ConstFuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...) const;


public:
    DelegateEntryImplConst() = delete;
    DelegateEntryImplConst(const DelegateEntryImplConst& other) = delete;
    DelegateEntryImplConst& operator=(const DelegateEntryImplConst& other) = delete;

    DelegateEntryImplConst(ObjectType* object, const ConstFuncTypePayload& fn, PayloadTypes... payloads) noexcept
        : Object(object), Function(fn), Payloads(std::forward<PayloadTypes>(payloads)...)  { }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        auto executeWithPayload = [&] <typename... T>(T&&... payloadArgs)
        {
            return (Object->*Function)(params..., std::forward<T>(payloadArgs)...);
        };

        return std::apply(executeWithPayload, Payloads);
    }


private:
    ObjectType* Object = nullptr;
    ConstFuncTypePayload Function = nullptr;
    std::tuple<PayloadTypes...> Payloads;
};



template<typename LambdaType, typename RetValType, typename... ParamTypes, typename... PayloadTypes>
class DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...), PayloadTypes...> : public IDelegateEntry<RetValType(ParamTypes...)>
{
public:
    DelegateEntryImplLambda() = delete;
    DelegateEntryImplLambda(const DelegateEntryImplLambda& other) = delete;
    DelegateEntryImplLambda& operator=(const DelegateEntryImplLambda& other) = delete;

    explicit DelegateEntryImplLambda(const LambdaType& fn, PayloadTypes... payloads) noexcept
        : Lambda(fn), Payloads(std::forward<PayloadTypes>(payloads)...)
    {
        static_assert(std::is_same_v<decltype(Lambda(std::declval<ParamTypes>()..., std::declval<PayloadTypes>()...)), RetValType>,
            "Lambda needs to have same return type!");
    }

    virtual RetValType Execute(ParamTypes... params) noexcept override
    {
        auto executeWithPayload = [&] <typename... T>(T&&... payloadArgs)
        {
            return Lambda(params..., std::forward<T>(payloadArgs)...);
        };

        return std::apply(executeWithPayload, Payloads);
    }


private:
    LambdaType Lambda;
    std::tuple<PayloadTypes...> Payloads;
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


    template<typename ObjectType, typename... PayloadTypes>
    using FuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...);

    template<typename ObjectType, typename... PayloadTypes>
    using ConstFuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...) const;


public:
    Delegate() noexcept = default;
    Delegate(const Delegate& other) = delete;
    Delegate& operator=(const Delegate& other) = delete;


    NODISCARD bool IsBound() const noexcept { return Entry; }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const FuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImpl<ObjectType, RetValType(ParamTypes...)>(object, fn);
    }

    template<typename ObjectType, typename... PayloadTypes>
    void BindObject(ObjectType* object, const FuncTypePayload<ObjectType, PayloadTypes...>& fn, PayloadTypes... payloads)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImpl<ObjectType, RetValType(ParamTypes...), PayloadTypes...>(object, fn, payloads...);
    }


    template<typename ObjectType>
    void BindObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...)>(object, fn);
    }

    template<typename ObjectType, typename... PayloadTypes>
    void BindObject(ObjectType* object, const ConstFuncTypePayload<ObjectType, PayloadTypes...>& fn, PayloadTypes... payloads)
    {
        DELEGATE_ASSERT(object != nullptr);

        delete Entry;
        Entry = new DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...), PayloadTypes...>(object, fn, payloads...);
    }


    template<typename LambdaType>
    void BindLambda(const LambdaType& fn)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...)>(fn);
    }

    template<typename LambdaType, typename... PayloadTypes>
    void BindLambda(const LambdaType& fn, PayloadTypes... payloads)
    {
        delete Entry;
        Entry = new DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...), PayloadTypes...>(fn, payloads...);
    }


    void Unbind() noexcept
    {
        delete Entry;
        Entry = nullptr;
    }


    RetValType Execute(ParamTypes... params) noexcept
    {
        return Entry->Execute(params...);
    }

    bool ExecuteIfBound(ParamTypes... params) noexcept
    {
        if(!IsBound())
            return false;

        Execute(params...);
        return true;
    }


private:
    IDelegateEntry<RetValType(ParamTypes...)>* Entry = nullptr;
};










template<typename RetValType, typename... ParamTypes>
struct EntryWrapper
{
    EntryWrapper() = delete;

    EntryWrapper(const EntryWrapper& other) = default;
    EntryWrapper& operator=(const EntryWrapper& other) = default;

    EntryWrapper(const DelegateKey inID, IDelegateEntry<RetValType(ParamTypes...)>* inEntry) noexcept
        : ID(inID), Entry(inEntry)  { }


public:
    DelegateKey ID;
    IDelegateEntry<RetValType(ParamTypes...)>* Entry;
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


    template<typename ObjectType, typename... PayloadTypes>
    using FuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...);

    template<typename ObjectType, typename... PayloadTypes>
    using ConstFuncTypePayload = RetValType(ObjectType::*)(ParamTypes..., PayloadTypes...) const;


public:
    MultiDelegate() noexcept = default;
    MultiDelegate(const MultiDelegate& other) = delete;
    MultiDelegate& operator=(const MultiDelegate& other) = delete;


    NODISCARD bool HasAnyListeners() const noexcept { return Entries.size(); }

    NODISCARD bool IsBound(const DelegateKey inKey) const noexcept
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
        Entries.emplace_back(GetNewID(), new DelegateEntryImpl<ObjectType, RetValType(ParamTypes...)>(object, fn));
        return Entries.back().ID;
    }

    template<typename ObjectType, typename... PayloadTypes>
    DelegateKey AddObject(ObjectType* object, const FuncTypePayload<ObjectType, PayloadTypes...>& fn, PayloadTypes... payloads)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.emplace_back(GetNewID(), new DelegateEntryImpl<ObjectType, RetValType(ParamTypes...), PayloadTypes...>(object, fn, payloads...));
        return Entries.back().ID;
    }


    template<typename ObjectType>
    DelegateKey AddObject(ObjectType* object, const ConstFuncType<ObjectType>& fn)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.emplace_back(GetNewID(), new DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...)>(object, fn));
        return Entries.back().ID;
    }

    template<typename ObjectType, typename... PayloadTypes>
    DelegateKey AddObject(ObjectType* object, const ConstFuncTypePayload<ObjectType, PayloadTypes...>& fn, PayloadTypes... payloads)
    {
        DELEGATE_ASSERT(object != nullptr);
        Entries.emplace_back(GetNewID(), new DelegateEntryImplConst<ObjectType, RetValType(ParamTypes...), PayloadTypes...>(object, fn, payloads...));
        return Entries.back().ID;
    }


    template<typename LambdaType>
    DelegateKey AddLambda(const LambdaType& fn)
    {
        Entries.emplace_back(GetNewID(), new DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...)>(fn));
        return Entries.back().ID;
    }

    template<typename LambdaType, typename... PayloadTypes>
    DelegateKey AddLambda(const LambdaType& fn, PayloadTypes... payloads)
    {
        Entries.emplace_back(GetNewID(), new DelegateEntryImplLambda<LambdaType, RetValType(ParamTypes...), PayloadTypes...>(fn, payloads...));
        return Entries.back().ID;
    }


    void Remove(const DelegateKey inKey) noexcept
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

    void Clear() noexcept
    {
        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            delete entry.Entry;

        Entries.clear();
    }


    void Broadcast(ParamTypes... params) noexcept
    {
        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            entry.Entry->Execute(params...);
    }

    template<typename T = RetValType, std::enable_if_t<!std::is_void_v<T>>* = nullptr>
    std::vector<RetValType> BroadcastRetVal(ParamTypes... params) noexcept
    {
        std::vector<RetValType> temp;
        temp.reserve(Entries.size());

        for(const EntryWrapper<RetValType, ParamTypes...>& entry : Entries)
            temp.push_back(entry.Entry->Execute(params...));

        return temp;
    }


private:
    NODISCARD DelegateKey GetNewID() noexcept { return CurrentID++; }

    DelegateKey CurrentID = 0;
    std::vector<EntryWrapper<RetValType, ParamTypes...>> Entries;
};
