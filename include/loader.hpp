#ifndef MT_LOADER_HPP_
#define MT_LOADER_HPP_

#include <string>
#include <type_traits>
#include <vector>

#include "mtdefs.hpp"

namespace mt {

    std::vector<std::string> Split(const std::string&, const std::string&);

    enum class MeshFormat {
        INVALID,
        OBJ,
    };

    class MeshGeometry {
      public:

        MeshGeometry(const std::string& path, MeshFormat format);

        MeshGeometry()          = delete;

        virtual ~MeshGeometry() = default;

        const f32* GetVertices(void) const;

        const u32* GetIndices(void) const;

        u64        GetVertexCount(void) const;

        u64        GetIndexCount(void) const;

      protected:

        std::vector<f32> m_vertices;
        std::vector<f32> m_uvs;
        std::vector<f32> m_normals;
        std::vector<u32> m_indices;
        MeshFormat       m_format;
    };

} // namespace mt

#endif
