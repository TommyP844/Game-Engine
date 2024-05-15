#pragma once

#include <assert.h>

#ifdef _DEBUG
#define MULE_ASSERT(x) assert(x);
#else
#define MULE_ASSERT(x)
#endif