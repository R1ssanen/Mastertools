
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

    u64           width = 1920, height = 1080;
    u64           resx = width / 3, resy = height / 3;

    SDL_Window*   window   = SDL_CreateWindow("Mastertools", width, height, SDL_WINDOW_RESIZABLE);
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
    camera.SetFarDistance(10.f);

    MeshGeometry mesh("resource/cornell_box.obj", MeshFormat::OBJ);
    glm::vec3    model_pos     = { 0.f, 0.f, 0.f };
    glm::vec3    model_scale   = { 1.f, 1.f, 1.f };
    glm::vec3    rotation_axis = { 0.f, 1.f, 0.f };
    rotation_axis              = glm::sphericalRand(1.f);

    VertexBuffer      vbo(mesh.GetVertices(), mesh.GetVertexCount(), 4);
    IndexBuffer       ibo(mesh.GetIndices(), mesh.GetIndexCount());
    ElementBuffer     ebo = { vbo, ibo };

    StdForwardVertex1 vs;
    StdForwardFrag1   fs;
    fs.depth_buffer = const_cast<f32*>(framebuffer.GetDepthBuffer().GetData());
    fs.width = resx, fs.height = resy;
    fs.inv_w_2 = 2.f / resx, fs.inv_h_2 = 2.f / resy;

    f32  frames         = 1;
    bool rotate         = false;
    bool relative_mouse = true;
    SDL_SetWindowRelativeMouseMode(window, true);

    cubemap_texture_t cubemap = {
        Texture::Load("resource/cubemap_1/px.png"), Texture::Load("resource/cubemap_1/nx.png"),
        Texture::Load("resource/cubemap_1/py.png"), Texture::Load("resource/cubemap_1/ny.png"),
        Texture::Load("resource/cubemap_1/pz.png"), Texture::Load("resource/cubemap_1/nz.png")
    };

    fs.cubemap = &cubemap;

    for (f32 dt = 0.f; running; ++frames, dt += rotate ? 0.001f : 0.f) {
        auto      time_frame_start = std::chrono::system_clock::now();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;

            if KEY_EVENT (SDLK_Q) running = false;
            if KEY_EVENT (SDLK_RETURN) framebuffer.wireframe = !framebuffer.wireframe;
            if KEY_EVENT (SDLK_B) framebuffer.cull_backfaces = !framebuffer.cull_backfaces;
            if KEY_EVENT (SDLK_R) rotate = !rotate;
            if KEY_EVENT (SDLK_ESCAPE) {
                relative_mouse = !relative_mouse;
                SDL_SetWindowRelativeMouseMode(window, relative_mouse);
            }

            if KEY_EVENT (SDLK_S)
                camera.SetPosition(camera.GetPosition() + glm::vec3(0.f, 0.f, 0.1f));
            if KEY_EVENT (SDLK_W)
                camera.SetPosition(camera.GetPosition() - glm::vec3(0.f, 0.f, 0.1f));
            if KEY_EVENT (SDLK_D)
                camera.SetPosition(camera.GetPosition() + glm::vec3(0.1f, 0.f, 0.f));
            if KEY_EVENT (SDLK_A)
                camera.SetPosition(camera.GetPosition() - glm::vec3(0.1f, 0.f, 0.f));
            if KEY_EVENT (SDLK_SPACE)
                camera.SetPosition(camera.GetPosition() + glm::vec3(0.f, 0.1f, 0.f));
            if KEY_EVENT (SDLK_LSHIFT)
                camera.SetPosition(camera.GetPosition() - glm::vec3(0.f, 0.1f, 0.f));

            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                f32 dx, dy;
                SDL_GetRelativeMouseState(&dx, &dy);
                camera.rotate(dx, dy);
            }
        }

        glm::mat4 rotate    = glm::rotate(glm::mat4(1.f), dt, rotation_axis);
        glm::mat4 translate = glm::translate(glm::mat4(1.f), model_pos);
        glm::mat4 scale     = glm::scale(glm::mat4(1.f), model_scale);
        glm::mat4 model     = translate * rotate * scale;

        glm::mat4 view      = camera.GetViewMatrix();
        glm::mat4 proj      = camera.GetProjectionMatrix();
        glm::mat4 transform = proj * view * model;

        fs.inv_view_proj    = glm::inverse(proj * view);

        vs.transform        = transform;
        framebuffer.render_elements(ebo, vs, fs);

        // framebuffer.render_cubemap_fullscreen(proj, view, cubemap);

        { // SDL side
            SDL_UpdateTexture(frame, nullptr, framebuffer.GetData(), framebuffer.GetPitch());
            SDL_RenderTexture(renderer, frame, nullptr, nullptr);

            SDL_SetRenderDrawColorFloat(renderer, 1.f, 1.f, 1.f, 1.f);
            SDL_RenderDebugText(
                renderer, 20, 20,
                std::format("render: {}ms", time_elapsed.count() / frames / 1E6f).c_str()
            );
            SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);

            SDL_RenderPresent(renderer);
        }

        framebuffer.Clear(BCOLOR | BDEPTH);
        time_elapsed += std::chrono::system_clock::now() - time_frame_start;
    }

    std::clog << "Average perf. " << time_elapsed.count() / frames / 1E6f;

    return 0;
}
