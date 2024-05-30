#include "mesh.hpp"

#include "OBJ_Loader.h"
#include "color.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace
{
std::unordered_map<std::string, core::mesh_vector_t> s_LoadedAssets;
}

namespace core
{

mesh_t Mesh::New(const std::vector<Vertex>& Vertices, const std::vector<u32>& Indices, u32 TextureID)
{
    glm::vec4 VertexSum{0.f};

    for (const Vertex& Vertex : Vertices)
    {
        VertexSum += Vertex.m_Pos;
    }

    glm::vec3 MassCenter = VertexSum / static_cast<f32>(Vertices.size());

    f32 MaxDistance = 0.f;
    for (const Vertex& Vertex : Vertices)
    {
        f32 Distance = glm::distance(MassCenter, glm::vec3(Vertex.m_Pos));
        if (Distance > MaxDistance)
        {
            MaxDistance = Distance;
        }
    }

    return std::make_shared<Mesh>(
        Mesh(
            _M{
                .Vertices = Vertices,
                .Indices = Indices,
                .MassCenter = MassCenter,
                .BoundingRadius = MaxDistance,
                .TextureID = TextureID
            }
        )
    );
}

mesh_vector_t LoadAsset(const std::filesystem::path& Path, b8 IsMipmapped, b8 IsDoublesided)
{
    const std::string Directory = Path.parent_path().string() + "/",
                      Filename = Path.filename().string();
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
        vertex_vector_t Vertices;

        for (const objl::Vertex& Vertex : ObjlMesh.Vertices)
        {
            Vertices.emplace_back(
                glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, 1.f),
                glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
                glm::vec3(0.f),
                glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y),
                0.f
            );
        }

        const b8 TextureExists = !ObjlMesh.MeshMaterial.map_Kd.empty();
        const b8 IsTransparent = ObjlMesh.MeshMaterial.d < 1.f || !ObjlMesh.MeshMaterial.map_d.empty();

        if (TextureExists)
        {
            texture_t Texture;
            if (IsMipmapped)
            {
                Texture = MipmapTexture::New(Directory + ObjlMesh.MeshMaterial.map_Kd, 6, IsTransparent,
                                             IsTransparent ? false : IsDoublesided);
            }

            else
            {
                Texture = ImageTexture::New(Directory + ObjlMesh.MeshMaterial.map_Kd, IsTransparent,
                                            IsTransparent ? false : IsDoublesided);
            }

            Meshes.push_back(Mesh::New(Vertices, ObjlMesh.Indices, Texture->GetID()));
        }

        else if (ObjlMesh.MeshMaterial.Kd != objl::Vector3(0.f, 0.f, 0.f))
        {
            texture_t Texture = ColorTexture::New(ObjlMesh.MeshMaterial.name,
                                                  ToUint32(ObjlMesh.MeshMaterial.Kd.X, ObjlMesh.MeshMaterial.Kd.Y,
                                                           ObjlMesh.MeshMaterial.Kd.Z, ObjlMesh.MeshMaterial.d),
                                                  IsTransparent, IsTransparent ? false : IsDoublesided);

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

mesh_t LoadAsset(const std::string& Path, u32 TextureID) {
    objl::Loader Loader;

    if (!Loader.LoadFile(Path)) [[unlikely]]
    {
        throw std::invalid_argument("Asset '" + Path + "' could not be loaded.");
    }

    std::vector<Vertex> Vertices;
    for (const objl::Vertex& Vertex : Loader.LoadedVertices)
    {
        Vertices.emplace_back(
            glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, 1.f),
            glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
            glm::vec3(0.f),
            glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y),
            0.f
        );
    }

    return Mesh::New(Vertices, Loader.LoadedIndices, TextureID);
}

} // namespace core
