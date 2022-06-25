#pragma once

#include "wmeDevice.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace wme
{
    class WmeDescriptorSetLayout 
    {
    public:
        class Builder 
        {
        public:
            Builder(WmeDevice& wmeDevice) : wmeDevice{ wmeDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<WmeDescriptorSetLayout> build() const;

        private:
            WmeDevice& wmeDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        WmeDescriptorSetLayout(WmeDevice& wmeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~WmeDescriptorSetLayout();
        WmeDescriptorSetLayout(const WmeDescriptorSetLayout&) = delete;
        WmeDescriptorSetLayout& operator=(const WmeDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        WmeDevice& wmeDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class WmeDescriptorWriter;
    };

    class WmeDescriptorPool 
    {
    public:
        class Builder 
        {
        public:
            Builder(WmeDevice& wmeDevice) : wmeDevice{ wmeDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<WmeDescriptorPool> build() const;

        private:
            WmeDevice& wmeDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        WmeDescriptorPool(
            WmeDevice& wmeDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~WmeDescriptorPool();
        WmeDescriptorPool(const WmeDescriptorPool&) = delete;
        WmeDescriptorPool& operator=(const WmeDescriptorPool&) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
        void resetPool();

    private:
        WmeDevice& wmeDevice;
        VkDescriptorPool descriptorPool;

        friend class WmeDescriptorWriter;
    };

    class WmeDescriptorWriter 
    {
    public:
        WmeDescriptorWriter(WmeDescriptorSetLayout& setLayout, WmeDescriptorPool& pool);

        WmeDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        WmeDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        WmeDescriptorSetLayout& setLayout;
        WmeDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}