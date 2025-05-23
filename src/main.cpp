
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

//
#include <barrier>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace mt;

int main(int argc, char* argv[]) {
    srand(time(0));

    u64           width = 1440, height = 900;
    u64           resx = width / 3, resy = height / 3;

    SDL_Window*   window   = SDL_CreateWindow("Mastertools", width, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture*  frame    = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, resx, resy
    );
    SDL_SetTextureScaleMode(frame, SDL_SCALEMODE_NEAREST);

    std::chrono::nanoseconds time_elapsed(0);
    bool                     running = true;

    // render variables
    Framebuffer       framebuffer(resx, resy);
    DefaultCamera     camera(resx / f32(resy));

    MeshGeometry      mesh("resource/teapot.obj", MeshFormat::OBJ);
    glm::vec3         model_pos     = { 0.f, -1.f, -2.5f };
    glm::vec3         model_scale   = { 0.5f, 0.5f, 0.5f };
    glm::vec3         rotation_axis = glm::sphericalRand(1.f);

    IndexBuffer       ibo(mesh.GetIndices(), mesh.GetIndexCount());
    VertexBuffer      vbo(mesh.GetVertices(), mesh.GetVertexCount(), 4);
    ElementBuffer     ebo = { vbo, ibo };

    StdForwardVertex1 vs;
    StdForwardFrag1   fs;

    for (f32 frames = 1, dt = 0.f; running; ++frames, dt += 0.001f) {
        auto      time_frame_start = std::chrono::system_clock::now();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q) running = false;
        }

        glm::mat4 rotate     = glm::rotate(glm::mat4(1.f), dt, rotation_axis);
        glm::mat4 translate  = glm::translate(glm::mat4(1.f), model_pos);
        glm::mat4 scale      = glm::scale(glm::mat4(1.f), model_scale);
        glm::mat4 model      = translate * rotate * scale; // * glm::inverse(translate);

        glm::mat4 view       = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();
        glm::mat4 transform  = projection * view * model;

        vs.transform         = transform;
        fs.depth_buffer      = const_cast<f32*>(framebuffer.GetDepthBuffer().GetData());
        framebuffer.RenderElements(ebo, vs, fs);

        SDL_UpdateTexture(frame, nullptr, framebuffer.GetData(), framebuffer.GetPitch());
        SDL_RenderTexture(renderer, frame, nullptr, nullptr);

        SDL_SetRenderDrawColorFloat(renderer, 1.f, 1.f, 1.f, 1.f);
        SDL_RenderDebugText(
            renderer, 20, 20,
            std::format("render: {}ms", time_elapsed.count() / frames / 1E6).c_str()
        );
        SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 0.f);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
        framebuffer.Clear(BCOLOR | BDEPTH);

        time_elapsed += std::chrono::system_clock::now() - time_frame_start;
    }

    return 0;
}
