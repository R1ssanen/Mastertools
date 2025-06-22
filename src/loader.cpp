#include "loader.hpp"

#include <cassert>
#include <cstring>
#include <format>
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "mtdefs.hpp"

namespace mt {

    static constexpr u64 FNV_1A(const std::string& str) {
        u64 hash = 0xcbf29ce484222325;

        for (u8 i = 0; i < str.size(); ++i) {
            hash ^= str[i];
            hash *= 0x00000100000001b3;
        }

        return hash;
    }

    static std::string Trim(std::string& str, const std::string& whitespace) {
        const auto begin = str.find_first_not_of(whitespace);
        if (begin == std::string::npos) return str;

        const auto end = str.find_last_not_of(whitespace);
        return str.substr(begin, end - begin + 1);
    }

    std::vector<std::string> Split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> tokens;
        u64                      last = 0;
        u64                      next = 0;

        while ((next = str.find(delimiter, last)) != std::string::npos) {
            tokens.emplace_back(str.substr(last, next - last));
            last = next + 1;
        }

        tokens.emplace_back(str.substr(last));
        return tokens;
    }

} // namespace mt

namespace mt {

    static bool LoadOBJ(
        const std::string& path, std::vector<f32>& vertices, std::vector<u32>& indices,
        std::vector<f32>& uvs, bool& uvs_indexed, std::vector<f32>& normals, bool& normals_indexed
    ) {

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "obj: could not open OBJ file " << path << ".\n";
            return false;
        }

        for (std::string line; std::getline(file, line); line = Trim(line, " \t\r\b\n")) {
            std::vector<std::string> parts = Split(line, " ");
            assert(parts.size() != 0);

            const u64 line_tag = FNV_1A(parts[0]);
            parts.erase(parts.begin());

            switch (line_tag) {
            case FNV_1A("v"): {
                assert(
                    parts.size() >= 3 && "obj: at least 3 position values required for a vertex."
                );
                vertices.push_back(std::stof(parts[0]));
                vertices.push_back(std::stof(parts[1]));
                vertices.push_back(std::stof(parts[2]));
            } break;

            case FNV_1A("f"): {
                assert(parts.size() >= 3 && "obj: at least 3 vertex indices required for a face.");

                for (const std::string& index : parts) {
                    std::vector<std::string> index_data = Split(index, "/");
                    assert(
                        index_data.size() >= 1 &&
                        "obj: face index must have at least one value per vertex."
                    );

                    // vertex
                    indices.push_back(std::stoi(index_data[0]) - 1);
                    // indices.push_back(std::stoi(index_data[1]) - 1);

#if 1
                    // uv only
                    if (index_data.size() == 2) {
                        indices.push_back(std::stoi(index_data[1]) - 1);
                        indices.push_back(0);
                        uvs_indexed     = true;
                        normals_indexed = false;
                    }

                    else if (index_data.size() == 3) {

                        // no uv, with normal
                        if (index_data[1].empty()) {
                            indices.push_back(0);
                            indices.push_back(std::stoi(index_data[2]) - 1);
                            uvs_indexed     = false;
                            normals_indexed = true;
                        }

                        // uv & normal
                        else {
                            indices.push_back(std::stoi(index_data[1]) - 1);
                            indices.push_back(std::stoi(index_data[2]) - 1);
                            uvs_indexed     = true;
                            normals_indexed = true;
                        }
                    }

                    else {
                        indices.push_back(0);
                        indices.push_back(0);
                        uvs_indexed     = false;
                        normals_indexed = false;
                    }
#endif
                }
            } break;

            case FNV_1A("vt"): {
                assert(
                    parts.size() >= 2 && "obj: at least 2 value required for a texture coordinate."
                );
                uvs.push_back(std::stof(parts[0]));
                uvs.push_back(std::stof(parts[1]));
            } break;

            case FNV_1A("vn"): {
                assert(parts.size() >= 3 && "obj: at least 3 values required for a vertex normal.");
                normals.push_back(std::stof(parts[0]));
                normals.push_back(std::stof(parts[1]));
                normals.push_back(std::stof(parts[2]));
            } break;

            default: continue;
            }
        }

        return true;
    }

} // namespace mt

namespace mt {

    MeshGeometry::MeshGeometry(const std::string& path, MeshFormat format) : m_format(format) {
        bool             uvs_indexed     = false;
        bool             normals_indexed = false;

        std::vector<u32> indices;
        std::vector<f32> positions;

        switch (format) {
        case MeshFormat::OBJ:
            if (!LoadOBJ(path, positions, indices, m_uvs, uvs_indexed, m_normals, normals_indexed))
                return;
            break;

        case MeshFormat::INVALID:
        default: std::cerr << "error: unknown mesh geometry format.\n";
        }

        assert(uvs_indexed && normals_indexed);

        u64 vertex_count = positions.size() / 3;
        m_vertices       = std::vector<f32>(vertex_count * (3 + 2 + 3));

        for (u64 i = 0; i < indices.size(); i += 3) {
            u32 pos_id               = indices[i];
            u32 uv_id                = indices[i + 1];
            u32 normal_id            = indices[i + 2];

            u64 pos_begin            = pos_id * 3;
            u64 pos_dest             = pos_id * (3 + 2 + 3);
            m_vertices[pos_dest + 0] = positions[pos_begin + 0];
            m_vertices[pos_dest + 1] = positions[pos_begin + 1];
            m_vertices[pos_dest + 2] = positions[pos_begin + 2];

            u64 uv_begin             = uv_id * 2;
            m_vertices[pos_dest + 3] = m_uvs[uv_begin + 0];
            m_vertices[pos_dest + 4] = m_uvs[uv_begin + 1];

            u64 normal_begin         = normal_id * 3;
            m_vertices[pos_dest + 5] = m_normals[normal_begin + 0];
            m_vertices[pos_dest + 6] = m_normals[normal_begin + 1];
            m_vertices[pos_dest + 7] = m_normals[normal_begin + 2];

            m_indices.push_back(pos_id);
        }
    }

    const f32* MeshGeometry::GetVertices(void) const { return m_vertices.data(); }

    const u32* MeshGeometry::GetIndices(void) const { return m_indices.data(); }

    u64        MeshGeometry::GetVertexCount(void) const { return m_vertices.size(); }

    u64        MeshGeometry::GetIndexCount(void) const { return m_indices.size(); }

} // namespace mt
