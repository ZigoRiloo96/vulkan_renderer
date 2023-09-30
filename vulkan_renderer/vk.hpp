#pragma once

#include "vulkan/vulkan.hpp"

#define CHECK_RESULT(type, success_value, function)\
{\
	type res = (function);\
	assert(res == success_value);\
}

#define VK_CHECK_RESULT(function)\
{\
	CHECK_RESULT(VkResult, VK_SUCCESS, function)\
}

//LOG_ERROR("VkResult is '{0}' in {1}:{2}", res, __FILE__ , __LINE__);