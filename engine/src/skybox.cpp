#include "skybox.hpp"

#include "camera.hpp"
#include "clipping.hpp"
#include "culling.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "render.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

Skybox::Skybox(const std::string& t_MeshDirectory,
               const std::string& t_MeshName,
               const std::string& t_TexturePath)
    : m_Mesh{LoadMeshOBJ(t_MeshDirectory, t_MeshName)[0]},
      m_TexName{t_TexturePath} {}

std::vector<Tri> Skybox::Transform(const Camera& t_Camera,
                                   const Context& t_Context) {
  std::vector<Vertex> TransVertices{m_Mesh.GetVertices()};
  std::vector<Tri> TrianglesOut;

  glm::mat4 MatViewProjection{
      t_Camera.GetMatProjection(t_Context.GetWidth(), t_Context.GetHeight()) *
      t_Camera.GetMatView()};

  for (Vertex& Vertex : TransVertices) {
    Vertex.m_Pos = MatViewProjection *
                   glm::translate(glm::mat4(1.f), t_Camera.GetPos()) *
                   Vertex.m_Pos;
  }

  for (size_t ID = 0; ID < m_Mesh.GetIndices().size(); ID += 3) {
    std::vector<Tri> ClipTris{FrustumClipTriangle(
        Tri{TransVertices[ID], TransVertices[ID + 1], TransVertices[ID + 2]},
        t_Camera.GetFrustum())};

    for (Tri& Tri : ClipTris) {
      for (Vertex& Vertex : Tri) {
        Vertex.m_Pos.w = 1.f / Vertex.m_Pos.w;

        Vertex.m_Pos.x *= Vertex.m_Pos.w;
        Vertex.m_Pos.y *= Vertex.m_Pos.w;
        Vertex.m_Pos.z *= Vertex.m_Pos.w;
        Vertex.m_UV *= Vertex.m_Pos.w;

        Vertex.m_Pos.x = (Vertex.m_Pos.x + 1.f) * 0.5f * t_Context.GetWidth();
        Vertex.m_Pos.y = (Vertex.m_Pos.y + 1.f) * 0.5f * t_Context.GetHeight();
      }

      if (BackfaceCull(Tri)) {
        continue;
      }

      TrianglesOut.push_back(Tri);
    }
  }

  return TrianglesOut;
}

void Skybox::Render(const Camera& t_Camera, Context& t_Context) {
  const std::vector<Tri> RenderTris{Transform(t_Camera, t_Context)};

  for (const Tri& Tri : RenderTris) {
    RenderTri(t_Context, Tri, m_TexName);
  }
}

}  // namespace core
