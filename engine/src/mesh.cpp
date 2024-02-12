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

Mesh Mesh::New(const std::vector<Vertex>& t_Vertices, const std::vector<unsigned>& t_Indices, uint32_t t_TextureID)
{
    glm::vec3 VertexSum{0.f};

    for (const Vertex& Vertex : t_Vertices)
    {
        VertexSum += glm::vec3(Vertex.m_Pos);
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

    return Mesh(_M{.Vertices = t_Vertices,
                   .Indices = t_Indices,
                   .MassCenter = MassCenter,
                   .BoundingRadius = MaxDistance,
                   .TextureID = t_TextureID});
}

mesh_vector_t LoadAsset(const std::string& t_Directory, const std::string& t_Name, bool t_IsMipmapped,
                        bool t_IsDoublesided)
{
    if (s_LoadedAssets.count(t_Directory + t_Name))
    {
        return s_LoadedAssets[t_Directory + t_Name];
    }

    objl::Loader Loader;

    if (!Loader.LoadFile(t_Directory + t_Name)) [[unlikely]]
    {
        throw std::invalid_argument("Asset '" + t_Directory + t_Name + "' could not be loaded.");
    }

    mesh_vector_t Meshes;

    for (const objl::Mesh& ObjlMesh : Loader.LoadedMeshes)
    {
        std::vector<Vertex> Vertices;

        for (const objl::Vertex& Vertex : ObjlMesh.Vertices)
        {
            Vertices.push_back(core::Vertex(glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z, 1.f),
                                            glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
                                            glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)));
        }

        const bool TextureExists{!ObjlMesh.MeshMaterial.map_Kd.empty()};
        const bool Transparent{ObjlMesh.MeshMaterial.d < 1.f};

        if (TextureExists)
        {
            texture_t Texture;
            if (t_IsMipmapped)
            {
                Texture = MipmapTexture::New(t_Directory + ObjlMesh.MeshMaterial.map_Kd, 6, Transparent,
                                             Transparent ? false : t_IsDoublesided);
            }

            else
            {
                Texture = ImageTexture::New(t_Directory + ObjlMesh.MeshMaterial.map_Kd, Transparent,
                                            Transparent ? false : t_IsDoublesided);
            }

            Meshes.push_back(std::make_shared<Mesh>(Mesh::New(Vertices, ObjlMesh.Indices, Texture->GetID())));
        }

        else if (!TextureExists && ObjlMesh.MeshMaterial.Kd != objl::Vector3(0.f, 0.f, 0.f))
        {
            texture_t Texture = ColorTexture::New(ObjlMesh.MeshMaterial.name,
                                                  ToUint32(ObjlMesh.MeshMaterial.Kd.X, ObjlMesh.MeshMaterial.Kd.Y,
                                                           ObjlMesh.MeshMaterial.Kd.Z, ObjlMesh.MeshMaterial.d),
                                                  Transparent, Transparent ? false : t_IsDoublesided);

            Meshes.push_back(std::make_shared<Mesh>(Mesh::New(Vertices, ObjlMesh.Indices, Texture->GetID())));
        }

        else
        {
            Meshes.push_back(std::make_shared<Mesh>(Mesh::New(Vertices, ObjlMesh.Indices, GetDefaultTexture()->GetID())));
        }
    }

    s_LoadedAssets[t_Directory + t_Name] = Meshes;
    return s_LoadedAssets[t_Directory + t_Name];
}

} // namespace core
