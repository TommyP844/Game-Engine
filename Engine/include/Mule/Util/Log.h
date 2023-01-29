#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>

#include <iostream>

#include <string>
#include <any>
#include <vector>

#include <Windows.h>

#include <ctime>


#ifndef DEBUG

#define LOG_MSG(X, ...)
#define LOG_ERR(X, ...)
#define LOG_WARN(X, ...)
#define LOG_SUCCESS(X, ...)


#else

static std::wstring FormatFileMacro(const std::string& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	temp = temp.substr(temp.find_last_of(L"\\") + 1);	
	return temp;
}

#define LOG_MSG(X, ...) Mule::Internal::Logger::LogMsg(FormatFileMacro(__FILE__), __LINE__, X, __VA_ARGS__);
#define LOG_ERR(X, ...) Mule::Internal::Logger::LogErr(FormatFileMacro(__FILE__), __LINE__, X, __VA_ARGS__);
#define LOG_WARN(X, ...) Mule::Internal::Logger::LogWarn(FormatFileMacro(__FILE__), __LINE__, X, __VA_ARGS__);
#define LOG_SUCCESS(X, ...) Mule::Internal::Logger::LogSuccess(FormatFileMacro(__FILE__), __LINE__, X, __VA_ARGS__);


#endif

namespace Mule::Internal
{
	class Logger
	{
	public:
		
		template<typename ... Args>
		static void LogMsg(const std::wstring& file, int line, const std::wstring& msg, Args&&... args)
		{
			std::wstring output = L"[ \"" + file + L"\"  Line: " + std::to_wstring(line) + L"  Time: " + GetTime() + L" ] MESSAGE: " + msg;
			int index = 0;
			BuildOutput(output, index, std::forward<Args>(args)...);
			std::wcout << output << std::endl;
		}

		template<typename ... Args>
		static void LogErr(const std::wstring& file, int line, const std::wstring& msg, Args... args)
		{
#ifdef WIN32
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
			std::wstring output = L"[ \"" + file + L"\"  Line: " + std::to_wstring(line) + L"  Time: " + GetTime() + L" ]   ERROR: " + msg;
			int index = 0;
			BuildOutput(output, index, args...);
			std::wcout << output << std::endl;
#ifdef WIN32
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
		}

		template<typename ... Args>
		static void LogWarn(const std::wstring& file, int line, const std::wstring& msg, Args... args)
		{
#ifdef WIN32
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
			std::wstring output = L"[ \"" + file + L"\"  Line: " + std::to_wstring(line) + L"  Time: " + GetTime() + L" ] WARNING: " + msg;
			int index = 0;
			BuildOutput(output, index, args...);
			std::wcout << output << std::endl;
#ifdef WIN32
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
		}

		template<typename ... Args>
		static void LogSuccess(const std::wstring& file, int line, const std::wstring& msg, Args... args)
		{
#ifdef WIN32
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif
			std::wstring output = L"[ \"" + file + L"\"  Line: " + std::to_wstring(line) + L"  Time: " + GetTime() + L" ] SUCCESS: " + msg;
			int index = 0;
			BuildOutput(output, index, args...);
			std::wcout << output << std::endl;
#ifdef WIN32
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
		}

	private:

		static std::wstring GetTime()
		{
			std::time_t t = std::time(0);   // get time now
			std::tm* now = std::localtime(&t);

			std::wstring str =
				(now->tm_hour < 10 ? L"0" : L"") + std::to_wstring(now->tm_hour)
				+ L":" + (now->tm_min < 10 ? L"0" : L"") + std::to_wstring(now->tm_min)
				+ L":" + (now->tm_sec < 10 ? L"0" : L"") + std::to_wstring(now->tm_sec);

			return str;
		}

		constexpr static void BuildOutput(std::wstring& output, int& argIndex)
		{
			return;
		}

		template<typename T>
		constexpr static void BuildOutput(std::wstring& output, int& argIndex, T value)
		{
			auto iter = output.find(L"{" + std::to_wstring(argIndex++) + L"}");
			if (iter == std::wstring::npos)
				return;
			std::wstring arg = ToString(value);
			output.replace(iter, 3, arg, 0, arg.size());
			return;
		}

		template<typename T, typename ... Args>
		constexpr static void BuildOutput(std::wstring& output, int& argIndex, T value, Args... args)
		{
			auto iter = output.find(L"{" + std::to_wstring(argIndex++) + L"}");
			if (iter == std::wstring::npos)
				return;
			std::wstring arg = ToString(value);
			output.replace(iter, 3, arg, 0, arg.size());

			BuildOutput(output, argIndex, args...);
		}

		static std::wstring ToString(const std::wstring& wstr)
		{
			return wstr;
		}

		static std::wstring ToString(const std::string& str)
		{
			std::wstring temp(str.length(), L' ');
			std::copy(str.begin(), str.end(), temp.begin());
			return temp;
		}

		static std::wstring ToString(const char* cs)
		{
			std::string str(cs);
			std::wstring temp(str.length(), L' ');
			std::copy(str.begin(), str.end(), temp.begin());
			return temp;
		}

		template<typename T>
		static std::wstring ToString(T value)
		{
			return std::to_wstring(value);
		}

	};
}