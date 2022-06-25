#include "wmeDescriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace wme 
{

    // *************** Descriptor Set Layout Builder *********************

    WmeDescriptorSetLayout::Builder& WmeDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) 
    {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<WmeDescriptorSetLayout> WmeDescriptorSetLayout::Builder::build() const 
    {
        return std::make_unique<WmeDescriptorSetLayout>(wmeDevice, bindings);
    }

    // *************** Descriptor Set Layout *********************

    WmeDescriptorSetLayout::WmeDescriptorSetLayout(
        WmeDevice& wmeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : wmeDevice{ wmeDevice }, bindings{ bindings } 
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) 
            setLayoutBindings.push_back(kv.second);

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            wmeDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) 
            throw std::runtime_error("failed to create descriptor set layout!");
    }

    WmeDescriptorSetLayout::~WmeDescriptorSetLayout() 
    {
        vkDestroyDescriptorSetLayout(wmeDevice.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    WmeDescriptorPool::Builder& WmeDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) 
    {
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    WmeDescriptorPool::Builder& WmeDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) 
    {
        poolFlags = flags;
        return *this;
    }
    WmeDescriptorPool::Builder& WmeDescriptorPool::Builder::setMaxSets(uint32_t count) 
    {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<WmeDescriptorPool> WmeDescriptorPool::Builder::build() const 
    {
        return std::make_unique<WmeDescriptorPool>(wmeDevice, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    WmeDescriptorPool::WmeDescriptorPool(
        WmeDevice& wmeDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : wmeDevice{ wmeDevice } 
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(wmeDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
            throw std::runtime_error("failed to create descriptor pool!");        
    }

    WmeDescriptorPool::~WmeDescriptorPool() 
    {
        vkDestroyDescriptorPool(wmeDevice.device(), descriptorPool, nullptr);
    }

    bool WmeDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(wmeDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) 
            return false;
        return true;
    }

    void WmeDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        vkFreeDescriptorSets(
            wmeDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void WmeDescriptorPool::resetPool() 
    {
        vkResetDescriptorPool(wmeDevice.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    WmeDescriptorWriter::WmeDescriptorWriter(WmeDescriptorSetLayout& setLayout, WmeDescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    WmeDescriptorWriter& WmeDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    WmeDescriptorWriter& WmeDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool WmeDescriptorWriter::build(VkDescriptorSet& set) 
    {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) 
            return false;
        overwrite(set);
        return true;
    }

    void WmeDescriptorWriter::overwrite(VkDescriptorSet& set) 
    {
        for (auto& write : writes) 
            write.dstSet = set;
        vkUpdateDescriptorSets(pool.wmeDevice.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }
}