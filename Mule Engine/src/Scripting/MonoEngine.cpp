
#include "Scripting/MonoEngine.h"

#include "Log.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/threads.h>

#define LOAD_MONO_METHOD(methodName) classMetaData.Methods[MethodType::methodName] = mono_class_get_method_from_name(klass, #methodName, -1);

namespace Mule
{
    void MonoEngine::Init()
    {
        CSTypeMapper::Initialize();
    }

    void MonoEngine::LoadAssembly(const fs::path& file)
	{
        mono_set_dirs("C:/Program Files/Mono/lib", "C:/Program Files/Mono");
        mDomain = mono_jit_init("MyDomain");

        mAssembly = mono_domain_assembly_open(mDomain, file.string().c_str());
        if (!mAssembly) {
            MULE_LOG_ERROR("Failed to load assembly: {0}", file);
        }

        mImage = mono_assembly_get_image(mAssembly);

        const MonoTableInfo* table_info = mono_image_get_table_info(mImage, MONO_TABLE_TYPEDEF);

        int rows = mono_table_info_get_rows(table_info);

        MULE_LOG_VERBOSE("Loading classes from assembly: {0}", file.filename().string());
        for (int i = 0; i < rows; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
            const char* name = mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAME]);
            const char* name_space = mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAMESPACE]);
            if (std::string(name_space) == "Mule")
            {
                MULE_LOG_VERBOSE("\tLoaded Class: {0}", name);
                mClassNames.push_back(name);
            }
        }
	}

    void MonoEngine::UnloadAssembly()
    {
        mono_jit_cleanup(mDomain);
    }

    std::unordered_map<std::string, CSType> MonoEngine::GetClassFieldData(const std::string& className)
    {
        mMutex.lock();
        mono_thread_attach(mDomain);

        std::unordered_map<std::string, CSType> fields;

        if (mClasses.find(className) == mClasses.end())
        {
            MULE_LOG_ERROR("Failed to find class {0}", className);
            return {};
        }

        for (auto& field : mClasses[className].Fields)
        {
            fields[field.first] = field.second.Type;
        }
        mMutex.unlock();
        return fields;
    }

    Ref<ScriptableClass> MonoEngine::CreateInstance(const std::string& className)
    {
        mono_thread_attach(mDomain);
        return Ref<ScriptableClass>::Make(className);
    }

    MonoObject* MonoEngine::CreateClassInstance(const std::string& className)
    {
        mMutex.lock();
        mono_thread_attach(mDomain);

        ClassMetaData classMetaData{};

        if (mClasses.find(className) != mClasses.end())
        {
            classMetaData = mClasses[className];
        }
        else
        {
            MonoClass* klass = mono_class_from_name(mImage, "Mule", className.c_str());
            if (klass == NULL)
            {
                MULE_LOG_ERROR("Failed to load class: {0}", className);
                return nullptr;
            }

            classMetaData.Class = klass;

            // Load all methods
            LOAD_MONO_METHOD(OnStart);
            LOAD_MONO_METHOD(OnUpdate);
            LOAD_MONO_METHOD(OnDestroy);

            // Load all fields
            MonoClassField* field;
            void* iter = NULL;
            while ((field = mono_class_get_fields(klass, &iter)) != NULL) {
                const char* fieldName = mono_field_get_name(field);
                MonoType* fieldType = mono_field_get_type(field);
                const char* fieldTypeName = mono_type_get_name(fieldType);

                FieldMetaData fieldMetaData{};
                fieldMetaData.Field = field;
                fieldMetaData.Type = CSTypeMapper::Get().GetType(fieldTypeName);
                fieldMetaData.Offset = mono_field_get_offset(field);
                fieldMetaData.Name = fieldName;
                fieldMetaData.Flags = mono_field_get_flags(field);

                classMetaData.Fields[fieldName] = fieldMetaData;

                MULE_LOG_VERBOSE("Field Name: {0}, Field Type: {1}", fieldName, fieldTypeName);
            }

            mClasses[className] = classMetaData;
        }

        MonoObject* object = mono_object_new(mDomain, classMetaData.Class);
        if (object == NULL)
        {
            MULE_LOG_ERROR("Failed to create instance of class: {0}", className);
            return nullptr;
        }

        mMutex.unlock();
        return object;
    }

    MonoClassField* MonoEngine::GetClassField(const std::string& className, const std::string& fieldName)
    {
        mMutex.lock();

        if (mClasses.find(className) == mClasses.end())
        {
            MULE_LOG_ERROR("Failed to find class: {0}", className);
            return nullptr;
        }
        auto& data = mClasses[className];
        if (data.Fields.find(fieldName) == data.Fields.end())
        {
            MULE_LOG_ERROR("Failed to find field {0} in class {1}", fieldName, className);
            return nullptr;
        }
        mMutex.unlock();
        return data.Fields[fieldName].Field;
    }
}
