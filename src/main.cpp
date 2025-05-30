
#include <chrono>
#include <cstring>
#include <format>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/mat4x4.hpp>
#include <glm/packing.hpp>
#include <glm/vec4.hpp>
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
#include "shader.hpp"
#include "texture.hpp"

#define KEY_EVENT(code) (e.type == SDL_EVENT_KEY_DOWN && e.key.key == code)

using namespace mt;

int main(int argc, char* argv[]) {
    srand(time(0));

    u64           width = 1440, height = 900;
    u64           resx = 640, resy = 480; // width / 2, resy = height / 2;

    SDL_Window*   window   = SDL_CreateWindow("Mastertools", width, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture*  frame    = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, resx, resy
    );
    SDL_SetTextureScaleMode(frame, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(frame, SDL_BLENDMODE_NONE);

    std::chrono::nanoseconds time_elapsed(0);
    bool                     running = true;

    // render variables
    Framebuffer   framebuffer(resx, resy);
    DefaultCamera camera;

    MeshGeometry  mesh("resource/teapot.obj", MeshFormat::OBJ);
    glm::vec3     model_pos     = { 0.f, -1.f, -2.f };
    glm::vec3     model_scale   = { 0.5f, 0.5f, 0.5f };
    glm::vec3     rotation_axis = { 0.f, 1.f, 0.f };
    // glm::vec3         rotation_axis = glm::sphericalRand(1.f);

    IndexBuffer       ibo(mesh.GetIndices(), mesh.GetIndexCount());
    VertexBuffer      vbo(mesh.GetVertices(), mesh.GetVertexCount(), 4);
    ElementBuffer     ebo = { vbo, ibo };

    StdForwardVertex1 vs;
    StdForwardFrag1   fs;
    fs.width = resx, fs.height = resy;
    fs.depth_buffer = const_cast<f32*>(framebuffer.GetDepthBuffer().GetData());

    for (f32 frames = 1, dt = 0.f; running; ++frames, dt += 0.001f) {
        auto      time_frame_start = std::chrono::system_clock::now();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if KEY_EVENT (SDLK_Q) running = false;
            if KEY_EVENT (SDLK_RETURN) framebuffer.wireframe = !framebuffer.wireframe;
            if KEY_EVENT (SDLK_S)
                camera.SetPosition(camera.GetPosition() + glm::vec3(0.f, 0.f, 0.1f));
            if KEY_EVENT (SDLK_W)
                camera.SetPosition(camera.GetPosition() - glm::vec3(0.f, 0.f, 0.1f));
        }

        glm::mat4 rotate     = glm::rotate(glm::mat4(1.f), dt, rotation_axis);
        glm::mat4 translate  = glm::translate(glm::mat4(1.f), model_pos);
        glm::mat4 scale      = glm::scale(glm::mat4(1.f), model_scale);
        glm::mat4 model      = translate * rotate * scale; // * glm::inverse(translate);

        glm::mat4 view       = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();
        glm::mat4 transform  = projection * view * model;

        vs.transform         = transform;
        framebuffer.RenderElements(ebo, vs, fs);

        SDL_UpdateTexture(frame, nullptr, framebuffer.GetData(), framebuffer.GetPitch());
        SDL_RenderTexture(renderer, frame, nullptr, nullptr);

        SDL_SetRenderDrawColorFloat(renderer, 1.f, 1.f, 1.f, 1.f);
        SDL_RenderDebugText(
            renderer, 20, 20,
            std::format("render: {}ms", time_elapsed.count() / frames / 1E6).c_str()
        );
        SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
        framebuffer.Clear(BCOLOR);
        std::fill_n(fs.depth_buffer, resx * resy, 0.f);

        time_elapsed += std::chrono::system_clock::now() - time_frame_start;
    }

    return 0;
}
