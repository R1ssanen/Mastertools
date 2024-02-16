#include "mesh.hpp"

#include "OBJ_Loader.h"
#include "color.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace
{
std::unordered_map<std::string, core::mesh_vector_t> s_LoadedAssets;
}

namespace core
{

mesh_t Mesh::New(const std::vector<Vertex>& t_Vertices, const std::vector<unsigned>& t_Indices, uint32_t t_TextureID)
{
    glm::vec4 VertexSum{0.f};

    for (const Vertex& Vertex : t_Vertices)
    {
        VertexSum += Vertex.m_Pos;
    }

    glm::vec3 MassCenter = VertexSum / static_cast<float>(t_Vertices.size());

    float MaxDistance = 0.f;
    for (const Vertex& Vertex : t_Vertices)
    {
        float Distance = glm::distance(MassCenter, glm::vec3(Vertex.m_Pos));
        if (Distance > MaxDistance)
        {
            MaxDistance = Distance;
        }
    }

    return std::make_shared<Mesh>(
        Mesh(
            _M{
                .Vertices = t_Vertices,
                .Indices = t_Indices,
                .MassCenter = MassCenter,
                .BoundingRadius = MaxDistance,
                .TextureID = t_TextureID
            }
        )
    );
}

mesh_vector_t LoadAsset(const std::filesystem::path& t_Path, bool t_IsMipmapped, bool t_IsDoublesided)
{
    const std::string Directory = std::string(t_Path.parent_path()) + "/",
                      Filename = t_Path.filename();
    const std::string FullPath = Directory + Filename;

    if (s_LoadedAssets.count(FullPath))
    {
        return s_LoadedAssets[FullPath];
    }

    objl::Loader Loader;

    if (!Loader.LoadFile(FullPath)) [[unlikely]]
    {
        throw std::invalid_argument("Asset '" + FullPath + "' could not be loaded.");
    }

    mesh_vector_t Meshes;

    for (const objl::Mesh& ObjlMesh : Loader.LoadedMeshes)
    {
        std::vector<Vertex> Vertices;

        for (const objl::Vertex& Vertex : ObjlMesh.Vertices)
        {
            Vertices.push_back(core::Vertex{
                .m_Pos = glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, 1.f),
                .m_Normal = glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
                .m_UV = glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)
            });
        }

        const bool TextureExists = !ObjlMesh.MeshMaterial.map_Kd.empty();
        const bool IsTransparent = ObjlMesh.MeshMaterial.d < 1.f || !ObjlMesh.MeshMaterial.map_d.empty();

        if (TextureExists)
        {
            texture_t Texture;
            if (t_IsMipmapped)
            {
                Texture = MipmapTexture::New(Directory + ObjlMesh.MeshMaterial.map_Kd, 6, IsTransparent,
                                             IsTransparent ? false : t_IsDoublesided);
            }

            else
            {
                Texture = ImageTexture::New(Directory + ObjlMesh.MeshMaterial.map_Kd, IsTransparent,
                                            IsTransparent ? false : t_IsDoublesided);
            }

            Meshes.push_back(Mesh::New(Vertices, ObjlMesh.Indices, Texture->GetID()));
        }

        else if (ObjlMesh.MeshMaterial.Kd != objl::Vector3(0.f, 0.f, 0.f))
        {
            texture_t Texture = ColorTexture::New(ObjlMesh.MeshMaterial.name,
                                                  ToUint32(ObjlMesh.MeshMaterial.Kd.X, ObjlMesh.MeshMaterial.Kd.Y,
                                                           ObjlMesh.MeshMaterial.Kd.Z, ObjlMesh.MeshMaterial.d),
                                                  IsTransparent, IsTransparent ? false : t_IsDoublesided);

            Meshes.push_back(Mesh::New(Vertices, ObjlMesh.Indices, Texture->GetID()));
        }

        else
        {
            Meshes.push_back(Mesh::New(Vertices, ObjlMesh.Indices, GetDefaultTexture()->GetID()));
        }
    }

    s_LoadedAssets[FullPath] = Meshes;
    return s_LoadedAssets[FullPath];
}

mesh_t LoadAsset(const std::string& t_Path, uint32_t t_TextureID) {
    objl::Loader Loader;

    if (!Loader.LoadFile(t_Path)) [[unlikely]]
    {
        throw std::invalid_argument("Asset '" + t_Path + "' could not be loaded.");
    }

    std::vector<Vertex> Vertices;
    for (const objl::Vertex& Vertex : Loader.LoadedVertices)
    {
        Vertices.push_back(core::Vertex{
                .m_Pos = glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, 1.f),
                .m_Normal = glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
                .m_UV = glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)
            });
    }

    return Mesh::New(Vertices, Loader.LoadedIndices, t_TextureID);
}

} // namespace core
