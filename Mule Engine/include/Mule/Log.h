#pragma once

#include <ostream>
#include <iostream>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <filesystem>

#include "Util/Singleton.h"


#ifdef _DEBUG
#define MULE_LOG_INIT(x) Mule::Log::Initialize(x);
#define MULE_LOG_ERROR(x, ...) Mule::Log::Get().LogMsg("ERROR", __FILE__, __LINE__, x, __VA_ARGS__);
#define MULE_LOG_WARNING(x, ...) Mule::Log::Get().LogMsg("WARNING", __FILE__, __LINE__, x, __VA_ARGS__);
#define MULE_LOG_SUCCESS(x, ...) Mule::Log::Get().LogMsg("SUCCESS", __FILE__, __LINE__, x, __VA_ARGS__);
#define MULE_LOG_VERBOSE(x, ...) Mule::Log::Get().LogMsg("VERBOSE", __FILE__, __LINE__, x, __VA_ARGS__);
#else
#define MULE_LOG_INIT(x, ...)
#define MULE_LOG_ERROR(x, ...)
#define MULE_LOG_WARNING(x, ...)
#define MULE_LOG_VERBOSE(x, ...)
#define MULE_LOG_SUCCESS(x, ...)
#endif

namespace Mule
{
	class Log : public Util::Singleton<Log>
	{
	public:
		// TODO: Check if the stream is cout and set console color
		template<typename... Args>
		void LogMsg(const std::string& prefix, const std::string& filename, size_t lineNumber, const std::string& format, Args&&... args) {
			std::stringstream oss;
			oss << ("[" + prefix + " " + filename.substr(filename.find_last_of("\\")+1) + ":" + std::to_string(lineNumber) + "] " + format);
			formatLog(oss, 0, std::forward<Args>(args)...);
			mStream << oss.str() << std::endl;
		}
		

	private:
		friend class Singleton<Log>;
		std::ostream& mStream;

		Log(std::ostream& stream)
			:
			mStream(stream)
		{
		}

		template<typename T, typename... Args>
		void formatLog(std::stringstream& oss, int index, const T& value, Args&&... args) {
			while (true) {
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					break;
				oss.str(oss.str().replace(pos, exp.size(), std::to_string(value)));
			}
			formatLog(oss, ++index, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void formatLog(std::stringstream& oss, int index, const std::filesystem::path& value, Args&&... args) {
			while (true) {
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					break;
				oss.str(oss.str().replace(pos, exp.size(), value.string()));
			}
			formatLog(oss, ++index, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void formatLog(std::stringstream& oss, int index, const std::string& value, Args&&... args) {
			while (true) {
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					break;
				oss.str(oss.str().replace(pos, exp.size(), value));
			}
			formatLog(oss, ++index, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void formatLog(std::stringstream& oss, int index, const char* value, Args&&... args) {
			{
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					return;
				oss.str(oss.str().replace(pos, exp.size(), value));
			}
			formatLog(oss, ++index, std::forward<Args>(args)...);
		}

		template<typename T>
		void formatLog(std::stringstream& oss, int index, const T& value) {
			{
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					return;
				oss.str(oss.str().replace(pos, exp.size(), std::to_string(value)));
			}
		}

		template<typename T>
		void formatLog(std::stringstream& oss, int index, const std::filesystem::path& value) {
			{
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					return;
				oss.str(oss.str().replace(pos, exp.size(), value.string()));
			}
		}

		void formatLog(std::stringstream& oss, int index, const std::string& value) {
			{
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					return;
				oss.str(oss.str().replace(pos, exp.size(), value));
			}
		}

		void formatLog(std::stringstream& oss, int index, const char* value) {
			while (true) {
				std::string exp = "{" + std::to_string(index) + "}";
				auto pos = oss.str().find(exp);
				if (pos == std::string::npos)
					return;
				oss.str(oss.str().replace(pos, exp.size(), value));
			}
		}

		void formatLog(std::stringstream& oss, int index) {
		}
	};
}