#pragma once

#if 0

#include "Util/Singleton.h"
#include "CSTypeMapper.h"
#include "MonoEngineClass.h"
#include "Ref.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace fs = std::filesystem;

namespace Mule
{
	enum class MethodType
	{
		OnStart,
		OnUpdate,
		OnDestroy
	};

	class ScriptableClass;

	class MonoEngine : public Util::Singleton<MonoEngine>
	{
	public:
		void Init();
		void Shutdown() {}

		void LoadAssembly(const fs::path& file);
		void UnloadAssembly();

		const std::vector<std::string>& GetClassNames() const { return mClassNames; }

		template<typename T>
		T InvokeMethod(MonoObject* obj, const std::string& className, MethodType type, void** args)
		{
			mMutex.lock();
			mono_jit_thread_attach(mDomain);

			if (mClasses.find(className) == mClasses.end())
			{
				MULE_LOG_ERROR("Failed to find class {0}", className);
				return T();
			}

			MonoObject* exception = NULL;
			MonoObject* ret = mono_runtime_invoke(mClasses[className].Methods[type], obj, args, &exception);

			if (exception != NULL)
			{
				MonoString* monoString = mono_object_to_string(exception, NULL);
				char* chars = mono_string_to_utf8(monoString);

				std::string exceptionMsg = chars;

				MULE_LOG_ERROR("Script Exception: {0}", exceptionMsg);

				return T();
			}

			T retVal = *(T*)mono_object_unbox(ret);
			mMutex.unlock();
			return retVal;
		}

		std::unordered_map<std::string, CSType> GetClassFieldData(const std::string& className);

		Ref<ScriptableClass> CreateInstance(const std::string& className);
		MonoObject* CreateClassInstance(const std::string& className);

		MonoClassField* GetClassField(const std::string& className, const std::string& fieldName);
		MonoDomain* Domain() const { return mDomain; }
	private:
		friend class Util::Singleton<MonoEngine>;
		std::mutex mMutex;

		MonoEngine(){}
		~MonoEngine(){}

		struct FieldMetaData
		{
			MonoClassField* Field;
			uint32_t Offset;
			std::string Name;
			CSType Type;
			uint32_t Flags;
		};

		struct ClassMetaData
		{
			MonoClass* Class;
			std::map<MethodType, MonoMethod*> Methods;
			std::unordered_map<std::string, FieldMetaData> Fields;
		};

		MonoDomain* mDomain;
		MonoAssembly* mAssembly;
		MonoImage* mImage;
		std::unordered_map<std::string, ClassMetaData> mClasses;

		std::vector<Ref<ScriptableClass>> mUserCreatedClasses;

		std::vector<std::string> mClassNames;
	};

	class ScriptableClass
	{
	public:
		ScriptableClass(const std::string& className)
			:
			mClassName(className)
		{
			if (!className.empty())
			{
				MonoEngine& engine = MonoEngine::Get();
				mThisObject = engine.CreateClassInstance(className);
				Fields = engine.GetClassFieldData(className);
			}
		}

		ScriptableClass(const std::string& className, MonoObject* object)
			:
			mClassName(className),
			mThisObject(object)
		{
			MonoEngine& engine = MonoEngine::Get();
			Fields = engine.GetClassFieldData(className);
		}

		const std::string& ClassName() const { return mClassName; }

		/*
		* SetFieldValue
		* 
		* @param fieldName - The name of the field in the script
		* @param value - the value to set in the script, any literaly type (int, float, bool, etc...)
		* 
		* @return nothing
		*/
		template<typename T>
		void SetFieldValue(const std::string& fieldName, const T& value)
		{
			MonoEngine& engine = MonoEngine::Get();
			MonoClassField* field = engine.GetClassField(mClassName, fieldName);
			mono_field_set_value(mThisObject, field, (void*) & value);
		}

		/*
		* SetFieldValue
		*
		* @param fieldName - The name of the field in the script
		* @param value - The ScriptableClass to set the member to
		*
		* @return nothing
		*/
		template<typename T>
		void SetFieldValue(const std::string& fieldName, const Ref<T>& value)
		{
			MonoEngine& engine = MonoEngine::Get();
			MonoClassField* field = engine.GetClassField(mClassName, fieldName);
			mono_field_set_value(mThisObject, field, value->mThisObject);
		}

		/*
		* SetFieldValue
		*
		* @param fieldName - The name of the field in the script
		* @param value - The ScriptableClass to set the member to
		*
		* @return nothing
		*/
		template<typename T>
		void SetFieldValue(const std::string& fieldName, const WeakRef<T>& value)
		{
			MonoEngine& engine = MonoEngine::Get();
			MonoClassField* field = engine.GetClassField(mClassName, fieldName);
			mono_field_set_value(mThisObject, field, value->mThisObject);
		}

		/*
		* GetFieldValue
		*
		* @param fieldName - The name of the field in the script
		* 
		* @return - The value of the field in the script (can only be literal type: int, float, etc)
		*/
		template<typename T>
		T GetFieldValue(const std::string& fieldName)
		{
			T value = 1;
			MonoEngine& engine = MonoEngine::Get();
			MonoClassField* field = engine.GetClassField(mClassName, fieldName);
			mono_field_get_value(mThisObject, field, &value);
			return value;
		}

		/*
		* GetFieldValue
		*
		* @param fieldName - The name of the field in the script
		*
		* @return - The Scriptable class representing the field
		*/
		Ref<ScriptableClass> GetFieldValue(const std::string& fieldName)
		{
			MonoEngine& engine = MonoEngine::Get();
			MonoClassField* field = engine.GetClassField(mClassName, fieldName);
			MonoObject* obj = mono_field_get_value_object(engine.Domain(), field, mThisObject);
			
			if (obj == NULL)
			{
				return nullptr;
			}

			MonoClass* klass = nullptr;
			klass = mono_object_get_class(obj);
			if (klass == nullptr)
			{
				return nullptr;
			}

			
			const char* className = mono_class_get_name(klass);
			return Ref<ScriptableClass>::Make(className, obj);
		}

		std::unordered_map<std::string, CSType> Fields;

		/*
		* Invoke Method
		* @param MethodType - The method to call: OnStart, OnUpdate, etc...
		* @param Args - The arguments to pass to the method, Must be a literal type
		* 
		* @return - The retun value of the function, must be a literal type
		*/
		template<typename T = void, typename ...Args>
		T InvokeMethod(MethodType type, Args&&... args)
		{
			auto argTuple = std::make_tuple(std::forward<Args>(args)...);

			void* argPtrs[sizeof...(Args)] = { &std::get<Args>(argTuple)... };

			MonoEngine& engine = MonoEngine::Get();
			return engine.InvokeMethod<T>(mThisObject, mClassName, type, argPtrs);
		}

		/*
		* Invoke Method
		* @param MethodType - The method to call: OnStart, OnUpdate, etc...
		*
		* @return - The retun value of the function, must be a literal type
		*/
		template<typename T = void>
		T InvokeMethod(MethodType type)
		{
			MonoEngine& engine = MonoEngine::Get();
			return engine.InvokeMethod<T>(mThisObject, mClassName, type, nullptr);
		}

	protected:
		ScriptableClass(){}
	private:
		MonoObject* mThisObject;
		std::string mClassName;
	};
}
#endif