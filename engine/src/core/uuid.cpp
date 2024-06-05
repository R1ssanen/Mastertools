#include "uuid.hpp"

#include "../mtpch.hpp"

namespace {

    std::random_device s_Device;
    std::mt19937_64    s_MersenneTwister(s_Device());
    std::uniform_int_distribution<mt::u64>
        s_IntDistribution(0, std::numeric_limits<mt::u64>::max());

} // namespace

namespace mt {

    UUID::UUID() : m_ID(s_IntDistribution(s_MersenneTwister)) { }

    UUID UUID::Register() { return UUID(); }

} // namespace mt
