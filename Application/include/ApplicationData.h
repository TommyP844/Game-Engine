#pragma once

#include "Mule.h"

class ApplicationData
{
public:
	ApplicationData()
		:
		mUserData(nullptr)
	{}
	~ApplicationData(){}

	void SetUserData(void* data) { mUserData = data; }

	template<typename T>
	T* GetUserData()
	{
		return (T*)mUserData;
	}

private:
	void* mUserData;
};
