#pragma once

// Define GLM configuration flags before including GLM headers
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

// Standard library includes
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Project-specific headers
#include "lve_device.hpp"
#include "lve_buffer.hpp"
// Third-party library includes
// TINYOBJLOADER_IMPLEMENTATION should be defined in a single .cpp file, not here
struct Vertex
{
  glm::vec3 position{};
  glm::vec3 color{};
  glm::vec3 normal{};
  glm::vec2 uv{};
  static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

  bool operator==(const Vertex &other) const
  {
    return position == other.position && color == other.color && normal == other.normal &&
           uv == other.uv;
  }
};
namespace std
{
  template <>
  struct hash<Vertex>
  {
    size_t operator()(Vertex const &vertex) const
    {
      size_t posHash = hash<glm::vec3>()(vertex.position);
      size_t colorHash = hash<glm::vec3>()(vertex.color) << 1;
      size_t normalHash = hash<glm::vec3>()(vertex.normal) >> 1; // Adding normal
      size_t uvHash = hash<glm::vec2>()(vertex.uv) << 1;

      return (((posHash ^ colorHash) >> 1) ^ uvHash) ^ normalHash; // Including normal in the final hash
    }
  };
}

namespace lve
{

  class LveModel
  {
  public:
    struct Builder
    {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
    };

    LveModel(LveDevice &device, const LveModel::Builder &builder);
    ~LveModel();

    LveModel(const LveModel &) = delete;
    LveModel &operator=(const LveModel &) = delete;

    static std::unique_ptr<LveModel> createModelFromFile(
        LveDevice &device, const std::string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    LveDevice &lveDevice;

    std::unique_ptr<LveBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<LveBuffer> indexBuffer;
    uint32_t indexCount;
  };
} // namespace lve const std::string &filepath);
