#pragma once

#include "object.hpp"
#include "srpch.hpp"

#include "glm/gtc/random.hpp"

namespace core {

template <size_t SIZE>
std::array<glm::vec3, SIZE * SIZE> GenerateOffsets(float t_Radius) {
  std::array<glm::vec3, SIZE * SIZE> Offsets;

  size_t i = 0;
  for (size_t x = -size_t(SIZE / 2); x < size_t(SIZE / 2); x++) {
    for (size_t z = -size_t(SIZE / 2); z < size_t(SIZE / 2); z++) {
      Offsets[i] = glm::vec3(x * 30.f, 0.f, z * 30.f);
      i++;
    }
  }

  return Offsets;
}

template <size_t SIZE>
std::vector<Object> GenerateInstances(
    const std::shared_ptr<Mesh>& t_BaseMesh,
    const std::array<glm::vec3, SIZE * SIZE>& t_Offsets) {
  std::vector<Object> Objects;

  for (size_t i = 0; i < SIZE * SIZE; i++) {
    Objects.push_back(Object(mesh_vector_t{t_BaseMesh}, t_Offsets[i]));
  }

  return Objects;
}

}  // namespace core
