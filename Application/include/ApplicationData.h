#pragma once

#include "Mule.h"

class ApplicationData
{
public:
	ApplicationData()
		:
		mUserData(nullptr),
		mIsAppRunning(true)
	{
		// TODO: wrap in macro for dev vs release
		mCWD = fs::current_path();
	}
	~ApplicationData(){}

	void SetUserData(void* data) { mUserData = data; }

	template<typename T>
	T* GetUserData()
	{
		return (T*)mUserData;
	}

	void CloseApplication() { mIsAppRunning = false; }
	bool IsAppRunning() const { return mIsAppRunning; }
	const fs::path GetCWD() const { return mCWD; }

	Mule::Ref<Mule::GraphicsDevice> GraphicsDevice;
private:
	void* mUserData;
	bool mIsAppRunning;
	fs::path mCWD;
};
