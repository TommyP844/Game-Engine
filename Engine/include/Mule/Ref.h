#pragma once

#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
std::shared_ptr<T> MakeRef(Args&&... args)
{
	return std::shared_ptr<T>(std::forward<Args>(args)...);
}
