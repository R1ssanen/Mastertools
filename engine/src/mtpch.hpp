#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <execution>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

#define GLM_USE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/fast_exponential.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <json.hpp>

#define BUILTINS_DIR                                                                               \
    std::string { "engine/builtins/" }

#include <SDL.h>

#include "core/definitions.hpp"
