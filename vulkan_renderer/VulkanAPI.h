#pragma once

const bool enableValidationLayers =
#ifdef NDEBUG
false;
#else
true;
#endif

struct 
QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct 
SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


static VkVertexInputBindingDescription getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

	return attributeDescriptions;
}

struct
	UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct 
VulkanAPI
{
	GLFWwindow* glfw_window;

	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	VkDevice device;

	VkQueue graphicsQueue;

	VkSurfaceKHR surface;

	VkQueue presentQueue;

	VkSwapchainKHR swapChain;

	std::vector<VkImage> swapChainImages;

	std::vector<VkImageView> swapChainImageViews;

	VkFormat swapChainImageFormat;

	VkExtent2D swapChainExtent;

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;

	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	uint32_t mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
};

//

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData);

void
InitVulkanAPI(VulkanAPI* api, std::vector<const char*> glfwExtensions, GLFWwindow* glfw_window);

void
TermVulkanAPI(VulkanAPI* api);

void
DrawFrame(VulkanAPI* api);

void
WaitIdle(VulkanAPI* api);

void
RecreateSwapChain(VulkanAPI* api);

// private

void
cleanupSwapChain(VulkanAPI* api);

bool
isDeviceSuitable(VulkanAPI* api, VkPhysicalDevice device);

void
pickPhysicalDevice(VulkanAPI* api);

void
checkExtensions(VulkanAPI* api);

bool
checkValidationLayerSupport();

VkResult
createDebugUtilsMessengerEXT(VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugUtilsMessengerEXT* pDebugMessenger);

void
destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

void
setupDebugMessenger(VulkanAPI* api);

QueueFamilyIndices
findQueueFamilies(const VkSurfaceKHR& surface, VkPhysicalDevice device);

void
createLogicalDevice(VulkanAPI* api);

void
createSurface(VulkanAPI* api);

bool
checkDeviceExtensionSupport(VkPhysicalDevice device);

SwapChainSupportDetails
querySwapChainSupport(const VkSurfaceKHR& surface, VkPhysicalDevice device);

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

VkExtent2D
chooseSwapExtent(GLFWwindow* glfw_window, const VkSurfaceCapabilitiesKHR& capabilities);

void
createSwapChain(VulkanAPI* api);

void
createImageViews(VulkanAPI* api);

std::vector<char>
readFile(const std::string& filename);

VkShaderModule
createShaderModule(VkDevice& device, const std::vector<char>& code);

void
createRenderPass(VulkanAPI* api);

void
createGraphicsPipeline(VulkanAPI* api);

void
createFramebuffers(VulkanAPI* api);

void
createCommandPool(VulkanAPI* api);

void
createCommandBuffers(VulkanAPI* api);

void
createSyncObjects(VulkanAPI* api);

void
createBuffer(VulkanAPI* api, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void
copyBuffer(VulkanAPI* api, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

void
createVertexBuffer(VulkanAPI* api);

void
createIndexBuffer(VulkanAPI* api);

uint32_t
findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void
createDescriptorSetLayout(VulkanAPI* api);

void
createUniformBuffers(VulkanAPI* api);

void
updateUniformBuffer(VulkanAPI* api, uint32_t currentImage);

void
createDescriptorPool(VulkanAPI* api);

void
createDescriptorSets(VulkanAPI* api);

void
createTextureImage(VulkanAPI* api);

void
createImage(VulkanAPI* api, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

VkCommandBuffer
beginSingleTimeCommands(VulkanAPI* api);

void
endSingleTimeCommands(VulkanAPI* api, VkCommandBuffer commandBuffer);

void
transitionImageLayout(VulkanAPI* api, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

void
copyBufferToImage(VulkanAPI* api, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

void
createTextureImageView(VulkanAPI* api);

VkImageView
createImageView(VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

void
createTextureSampler(VulkanAPI* api);

void
createDepthResources(VulkanAPI* api);

VkFormat findSupportedFormat(VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat
findDepthFormat(VulkanAPI* api);

bool
hasStencilComponent(VkFormat format);

void
loadModel(VulkanAPI* api);

void
generateMipmaps(VulkanAPI* api, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

VkSampleCountFlagBits
getMaxUsableSampleCount(VkPhysicalDevice& physicalDevice);

void
createColorResources(VulkanAPI* api);

void
createAllocator();


namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
