#pragma once

#include <memory>

namespace Mule
{
    template<typename T>
    class Ref 
    {    
    public:
        explicit Ref(T* ptr)
        {
            mPtr = ptr;
            mRefCount = new size_t(1);
        }

        explicit Ref()
        {
            mPtr = nullptr;
            mRefCount = nullptr;
        }

        Ref(std::nullptr_t) 
            :
            mPtr(nullptr),
            mRefCount(nullptr)
        {}
    
        Ref(const Ref& other) 
            : 
            mPtr(other.mPtr), 
            mRefCount(other.mRefCount) 
        {
            if (mRefCount)
            {
                ++(*mRefCount);
            }
        }

        template<typename Derived>
        Ref(const Ref<Derived>& other)
        {
            // Use dynamic_cast to cast the pointer to the derived type
            mPtr = (T*)other.Get();
            if (mPtr) {
                mRefCount = other.RefCountPtr();
                ++(*mRefCount);
            }
            else {
                // Handle dynamic cast failure
                // For example, set mRefCount to nullptr to indicate an invalid reference
                mRefCount = nullptr;
            }
        }

        template<typename ...Args>
        static Ref Make(Args&&... args)
        {
            T* ptr = new T(std::forward<Args>(args)...);
            size_t* refCount = new size_t(1);
            return Ref(ptr, refCount);
        }

        template<typename Derived>
        Ref& operator=(const Ref<Derived>& other) {
            if (mPtr != other.Get()) {
                Release();
                mPtr = (T*)other.Get();
                mRefCount = other.RefCountPtr();
                ++(*mRefCount);
            }
            return *this;
        }
    
        ~Ref() {
            Release();
        }
    
        Ref& operator=(const Ref& other) {
            if (this != &other && other.mPtr != nullptr) {
                Release();
                mPtr = other.mPtr;
                mRefCount = other.mRefCount;
                ++(*mRefCount);
            }
            return *this;
        }

        Ref& operator=(T* ptr) {
            if (mPtr != ptr) {
                Release();
                mPtr = ptr;
                mRefCount = new size_t(ptr ? 1 : 0);
            }
            return *this;
        }

        bool operator==(std::nullptr_t) const {
            return mPtr == nullptr;
        }

        bool operator==(const Ref<T>& other) const {
            return mPtr == other.mPtr;
        }

        bool operator!=(std::nullptr_t) const {
            return mPtr != nullptr;
        }

        operator bool() const {
            return mPtr != nullptr;
        }
    
        T& operator*() const {
            return *mPtr;
        }
    
        // Member access operator
        T* operator->() const {
            return mPtr;
        }
    
        void Release() {
            if (mPtr != nullptr) {
                if (--(*mRefCount) == 0) {
                    delete mPtr;
                    delete mRefCount;
                }
                mPtr = nullptr;
                mRefCount = nullptr;
            }
        }
    
        T* Get() const {
            return mPtr;
        }

        T& GetRef() {
            return *mPtr;
        }
    
        size_t UseCount() const {
            return (mRefCount != nullptr) ? *mRefCount : 0;
        }

        size_t* RefCountPtr() const { return mRefCount; }

    private:
        Ref(T* ptr, size_t* refCount)
            :
            mPtr(ptr),
            mRefCount(refCount)
        {}

        T* mPtr;
        size_t* mRefCount;
    };

    template<typename T>
    class WeakRef
    {
    public:
        WeakRef(T* ptr = nullptr)
            :
            mPtr(ptr)
        {}

        WeakRef(const WeakRef& other) 
            : 
            mPtr(other.mPtr)
        {}

        WeakRef(const Ref<T>& other)
            :
            mPtr(other.Get())
        {}

        WeakRef(std::nullptr_t) :
            mPtr(nullptr)
        {}

        template<typename Derived>
        WeakRef(const Ref<Derived>& other)
        {
            mPtr = (T*)other.Get();
        }

        template<typename Derived>
        WeakRef(const WeakRef<Derived>& other)
        {
            mPtr = (T*)other.Get();
        }

        ~WeakRef() {}


        WeakRef& operator=(T* ptr) {
            if (mPtr != ptr) {
                mPtr = ptr;
            }
            return *this;
        }

        bool operator==(std::nullptr_t) const {
            return mPtr == nullptr;
        }

        bool operator==(const WeakRef<T>& other) const {
            return mPtr == other.mPtr;
        }

        bool operator!=(std::nullptr_t) const {
            return mPtr != nullptr;
        }

        operator bool() const {
            return mPtr != nullptr;
        }

        WeakRef& operator=(const WeakRef& other) {
            if (this != &other) {
                mPtr = other.mPtr;
            }
            return *this;
        }

        WeakRef& operator=(const Ref<T>& other)
        {
            mPtr = other.Get();
            return *this;
        }

        template<typename Derived>
        WeakRef& operator=(const Ref<Derived>& other)
        {
            mPtr = (T*)other.Get();
            return *this;
        }

        template<typename Derived>
        WeakRef& operator=(const WeakRef<Derived>& other)
        {
            mPtr = (T*)other.Get();
            return *this;
        }

        T& operator*() const {
            return *mPtr;
        }

        // Member access operator
        T* operator->() const {
            return mPtr;
        }

        T* Get() const {
            return mPtr;
        }

    private:
        T* mPtr;
    };
}