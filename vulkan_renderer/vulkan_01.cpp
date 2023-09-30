
#include <iostream>
#include <cstdlib>

#include <vulkan/vulkan.h>

const char* ENGINE_SHORT_NAME = "outcore";
const char* APP_SHORT_NAME = "vulkane_renderer_instance";

VkResult init_global_layer_properties(struct sample_info& info)
{

}

int main()
{
	struct sample_info info = {};

	init_global_layer_properties(info);

	// init the VkApplicationInfo struct

	VkApplicationInfo app_info = 
  {
    VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
    nullptr, // pNext
    APP_SHORT_NAME, // pApplicationName
    1, // applicationVersion
    ENGINE_SHORT_NAME, // pEngineName
    1, // engineVersion
    VK_API_VERSION_1_2 // apiVersion
  };


  // init VkInstanceCretateInfo struct

  VkInstanceCreateInfo inst_info =
  {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    nullptr,
    0, // ?
    &app_info,
    0, // ?
    nullptr,
    0, // ?
    nullptr
  };

  VkInstance inst;
  VkResult res;

  res = vkCreateInstance(&inst_info, nullptr, &inst);

  if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
  {
    std::cout << "BAD DRIVER\n";
    return -1;
  }
  else if (res)
  {
    std::cout << "unknown error\n";
    return -1;
  }

  vkDestroyInstance(inst, nullptr);

  return 0;
}
