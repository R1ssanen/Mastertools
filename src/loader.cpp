#include "loader.hpp"

#include <cassert>
#include <cstring>
#include <format>
#include <fstream>
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
        const std::string&& path, std::vector<f32>& vertices, std::vector<u32>& indices,
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
                vertices.emplace_back(std::stof(parts[0]));
                vertices.emplace_back(std::stof(parts[1]));
                vertices.emplace_back(std::stof(parts[2]));

                if (parts.size() >= 4) // w
                    vertices.emplace_back(std::stof(parts[3]));
                else
                    vertices.emplace_back(1.f);
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
                    indices.emplace_back(std::stoi(index_data[0]) - 1);
#if 0
                    // uv only
                    if (index_data.size() == 2) {
                        indices.emplace_back(std::stoi(index_data[1]) - 1);
                        indices.emplace_back(0);
                        uvs_indexed     = true;
                        normals_indexed = false;
                    }

                    else if (index_data.size() == 3) {

                        // no uv, with normal
                        if (index_data[1].empty()) {
                            indices.emplace_back(0);
                            indices.emplace_back(std::stoi(index_data[2]) - 1);
                            uvs_indexed     = false;
                            normals_indexed = true;
                        }

                        // uv & normal
                        else {
                            indices.emplace_back(std::stoi(parts[1]) - 1);
                            indices.emplace_back(std::stoi(parts[2]) - 1);
                            uvs_indexed     = true;
                            normals_indexed = true;
                        }
                    }

                    else {
                        indices.emplace_back(0);
                        indices.emplace_back(0);
                        uvs_indexed     = false;
                        normals_indexed = false;
                    }
#endif
                }
                break;

            case FNV_1A("vt"): {
                assert(
                    parts.size() >= 2 && "obj: at least 2 value required for a texture coordinate."
                );
                uvs.emplace_back(std::stof(parts[0]));
                uvs.emplace_back(std::stof(parts[1]));
            } break;

            case FNV_1A("vn"): {
                assert(parts.size() >= 3 && "obj: at least 3 values required for a vertex normal.");
                normals.emplace_back(std::stof(parts[0]));
                normals.emplace_back(std::stof(parts[1]));
                normals.emplace_back(std::stof(parts[2]));
            } break;

            default: continue;
            }
            }
        }

        return true;
    }

} // namespace mt

namespace mt {

    MeshGeometry::MeshGeometry(const std::string& path, MeshFormat format) : m_format(format) {
        bool             uvs_indexed     = false;
        bool             normals_indexed = false;

        std::vector<f32> positions;

        switch (format) {

        case MeshFormat::OBJ:
            if (!LoadOBJ(
                    std::forward<const std::string>(path), m_vertices, m_indices, m_uvs,
                    uvs_indexed, m_normals, normals_indexed
                )) {
                return;
            }
            break;

        case MeshFormat::INVALID:
        default: std::cerr << "error: unknown mesh geometry format.\n";
        }

        return;
        for (u64 i = 0; i < m_indices.size(); i += 3) {
            f32* offset_positions = positions.data() + m_indices[i] * 4;
            f32* offset_normals   = m_normals.data() + m_indices[i + 1] * 3;
            f32* offset_uvs       = m_uvs.data() + m_indices[i + 2] * 2;

            m_vertices.emplace_back(offset_positions[0]);
            m_vertices.emplace_back(offset_positions[1]);
            m_vertices.emplace_back(offset_positions[2]);
            m_vertices.emplace_back(offset_positions[3]);

            m_vertices.emplace_back(offset_normals[0]);
            m_vertices.emplace_back(offset_normals[1]);
            m_vertices.emplace_back(offset_normals[2]);

            m_vertices.emplace_back(offset_uvs[0]);
            m_vertices.emplace_back(offset_uvs[1]);
        }
    }

    const f32* MeshGeometry::GetVertices(void) const { return m_vertices.data(); }

    const u32* MeshGeometry::GetIndices(void) const { return m_indices.data(); }

    u64        MeshGeometry::GetVertexCount(void) const { return m_vertices.size(); }

    u64        MeshGeometry::GetIndexCount(void) const { return m_indices.size(); }

} // namespace mt
