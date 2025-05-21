
#include <chrono>
#include <cstring>
#include <format>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/packing.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"
#include "buffers/ib.hpp"
#include "buffers/vb.hpp"
#include "camera.hpp"
#include "loader.hpp"
#include "mtdefs.hpp"
#include "render.hpp"
#include "texture.hpp"

//
#include "shader.hpp"

using namespace mt;

int main(int argc, char* argv[]) {

    SDL_Window*   window   = SDL_CreateWindow("Mastertools", 1440, 900, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture*  frame    = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 1440 / 1, 900 / 1
    );
    SDL_SetTextureScaleMode(frame, SDL_SCALEMODE_NEAREST);

    Framebuffer   framebuffer(1440 / 1, 900 / 1);

    MeshGeometry  mesh("C:/Users/aaror/projects/Mastertools/resource/cube.obj", MeshFormat::OBJ);

    IndexBuffer   ibo(mesh.GetIndices(), mesh.GetIndexCount());
    VertexBuffer  vbo(mesh.GetVertices(), mesh.GetVertexCount(), 4);
    ElementBuffer ebo    = { vbo, ibo };

    u32*          colors = new u32[mesh.GetIndexCount() / 3];
    std::srand(std::time(0));
    for (u64 n = 0; n < mesh.GetIndexCount() / 3; ++n)
        colors[n] = glm::linearRand(0xaaaaaaaau, 0xffffffffu);

    auto camera = DefaultCamera(1440.f / 900.f);
    camera.SetPosition(glm::vec3(0.f, 0.f, 5.f));

    f32               dt     = 0.f;
    u64               frames = 0;
    auto              start  = std::chrono::system_clock::now();

    StdForwardVertex1 vs;
    StdForwardFrag1   fs;
    fs.colors = colors;

    for (bool running = true; running; dt += 0.001f, ++frames) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_EVENT_QUIT: running = false; break;

            case SDL_EVENT_KEY_DOWN: {
                switch (event.key.key) {
                case SDLK_Q: running = false; break;

                case SDLK_RETURN: framebuffer.wireframe = !framebuffer.wireframe; break;

                case SDLK_B: framebuffer.cull_backfaces = !framebuffer.cull_backfaces; break;

                case SDLK_W:
                    camera.SetPosition(camera.GetPosition() - glm::vec3(0.f, 0.f, 0.1f));
                    break;
                case SDLK_S:
                    camera.SetPosition(camera.GetPosition() + glm::vec3(0.f, 0.f, 0.1f));
                    break;
                case SDLK_A:
                    camera.SetPosition(camera.GetPosition() - glm::vec3(0.1f, 0.f, 0.f));
                    break;
                case SDLK_D:
                    camera.SetPosition(camera.GetPosition() + glm::vec3(0.1f, 0.f, 0.f));
                    break;
                default: break;
                }
            } break;

            default: break;
            }
        }

        glm::mat4 view       = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();
        glm::mat4 transform =
            projection * view * glm::rotate(glm::mat4(1.f), dt, glm::vec3(0.f, 1.f, 0.f));

        vs.transform = transform;
        framebuffer.RenderElements(ebo, vs, fs);

        SDL_UpdateTexture(
            frame, nullptr, framebuffer.GetColorBuffer().GetData(), int(framebuffer.GetPitch())
        );
        SDL_RenderTexture(renderer, frame, nullptr, nullptr);

        SDL_SetRenderDrawColorFloat(renderer, 1.f, 1.f, 1.f, 1.f);
        SDL_RenderDebugText(
            renderer, 20, 20,
            std::format(
                "Average frametime (ms): {}\n",
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - start
                ) / f32(frames)
            )
                .c_str()
        );
        SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        framebuffer.Clear(BCOLOR | BDEPTH);
    }

    return 0;
}
