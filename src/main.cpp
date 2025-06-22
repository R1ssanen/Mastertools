
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

struct Model {
    MeshGeometry& mesh;
    glm::vec3     pos, scale;
    bool          rotate, is_shadower;
};

int main(int argc, char* argv[]) {
    srand(time(0));

    /*

        uint32_t width = 1440;
        uint32_t height = 900;

        rohan::Frame<uint32_t> pixelbuf(width, height);
        rohan::Frame<float>    zbuf(width, height);

        rohan::RenderTarget<uint32_t, float> target;
        target.bind_color_buffer(pixelbuf);
        target.bind_depth_buffer(zbuf);

        target.cull_backfaces(true);
        target.winding_order(rohan::WindingOrder::CW);
        target.test_depth(true);
        target.depth_reject(rohan::DepthMethod::LESS);
        target.depth_range(1.f, 0.f);

        const float* vertices = {...};
        const uint32_t* indices = {...};

        target.draw_indexed(vertices, indices, rohan::DrawMode::TRIANGLES);

        uint32_t* finished_frame = pixelbuf.data();
    */

    u64           width = 1920, height = 1080;
    u64           resx = width / 2, resy = height / 2;

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
    camera.SetPosition(glm::vec3(0.f, 0.f, 2.f));
    camera.SetFarDistance(200.f);

    DefaultCamera shadow_camera;
    shadow_camera.SetPosition(glm::vec3(0.f, 0.f, 5.f));

    ShadowFrag         shadow_fs;
    Framebuffer        shadowbuffer(resx, resy);

    glm::vec3          rotation_axis = glm::sphericalRand(1.f);
    std::vector<Model> models;

    /*
    auto               pillar_mesh = MeshGeometry("resource/pillar.obj", MeshFormat::OBJ);
    for (int x = -5; x < 5; ++x) {
        for (int z = -5; z < 5; ++z) {
            models.emplace_back(
                pillar_mesh, glm::vec3(x * 40.f, 0.f, z * 40.f), glm::vec3(1.f), false, false
            );
        }
    }*/
    auto mesh = MeshGeometry("resource/billboard.obj", MeshFormat::OBJ);
    models.emplace_back(mesh, glm::vec3(0.f), glm::vec3(1.f), true, false);

    f32  frames         = 1;
    bool rotate         = false;
    bool relative_mouse = true;
    SDL_SetWindowRelativeMouseMode(window, true);

    cubemap_texture_t cubemap = {
        Texture::Load("resource/cubemap_5/px.png"), Texture::Load("resource/cubemap_5/nx.png"),
        Texture::Load("resource/cubemap_5/py.png"), Texture::Load("resource/cubemap_5/ny.png"),
        Texture::Load("resource/cubemap_5/pz.png"), Texture::Load("resource/cubemap_5/nz.png")
    };

    StdForwardVertex1 vs;
    StdForwardFrag1   fs;
    fs.depth_buffer = const_cast<f32*>(framebuffer.GetDepthBuffer().GetData());
    fs.width = resx, fs.height = resy;
    fs.inv_w_2 = 2.f / resx, fs.inv_h_2 = 2.f / resy;
    fs.inv_far = 1.f / 200.f;
    fs.cubemap = &cubemap;

    AABB cubemap_aabb(glm::vec3(0.f, 0.f, -0.5f), glm::vec3(1.03f));
    fs.cubemap_aabb = &cubemap_aabb;

    // auto texture         = Texture::Load("resource/uv_debug.png");
    // fs.texture           = &texture;

    fs.shadowmap    = const_cast<f32*>(shadowbuffer.GetDepthBuffer().GetData());
    fs.light_view   = shadow_camera.GetViewMatrix();
    fs.projection   = shadow_camera.GetProjectionMatrix();

    for (f32 dt = 0.f; running; ++frames, dt += rotate ? 0.001f : 0.f) {
        auto      time_frame_start = std::chrono::system_clock::now();

        glm::mat4 view             = camera.GetViewMatrix();
        glm::mat4 proj             = camera.GetProjectionMatrix();
        glm::mat4 view_proj        = proj * view;
        fs.inv_view_proj           = glm::inverse(view_proj);
        fs.camera_world            = camera.GetPosition();

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

            if KEY_EVENT (SDLK_W)
                camera.SetPosition(camera.GetPosition() + camera.forward() * 0.05f);
            if KEY_EVENT (SDLK_S)
                camera.SetPosition(camera.GetPosition() - camera.forward() * 0.05f);
            if KEY_EVENT (SDLK_D) camera.SetPosition(camera.GetPosition() + camera.right() * 0.05f);
            if KEY_EVENT (SDLK_A) camera.SetPosition(camera.GetPosition() - camera.right() * 0.05f);
            if KEY_EVENT (SDLK_SPACE)
                camera.SetPosition(camera.GetPosition() + camera.up() * 0.05f);
            if KEY_EVENT (SDLK_LSHIFT)
                camera.SetPosition(camera.GetPosition() - camera.up() * 0.05f);

            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                f32 dx, dy;
                SDL_GetRelativeMouseState(&dx, &dy);
                camera.rotate(dx, dy);
            }
        }

#if 1

        // render shadowmap
        for (auto& model : models) {
            if (!model.is_shadower) continue;

            glm::mat4 mat_model =
                glm::rotate(glm::mat4(1.f), model.rotate ? dt : 0.f, rotation_axis) *
                glm::scale(glm::mat4(1.f), model.scale);

            VertexBuffer vbo(model.mesh.GetVertices(), model.mesh.GetVertexCount(), 4);
            IndexBuffer  ibo(model.mesh.GetIndices(), model.mesh.GetIndexCount());

            vs.transform =
                shadow_camera.GetProjectionMatrix() * shadow_camera.GetViewMatrix() * mat_model;
            shadowbuffer.render_elements(vbo, ibo, vs, shadow_fs);
        }

        // render frame
        for (auto& model : models) {

            glm::mat4 mat_model =
                glm::translate(glm::mat4(1.f), model.pos) *
                glm::rotate(glm::mat4(1.f), model.rotate ? dt : 0.f, rotation_axis) *
                glm::scale(glm::mat4(1.f), model.scale);

            fs.rotation = glm::rotate(glm::mat4(1.f), model.rotate ? dt : 0.f, rotation_axis);

            VertexBuffer vbo(model.mesh.GetVertices(), model.mesh.GetVertexCount(), 8);
            IndexBuffer  ibo(model.mesh.GetIndices(), model.mesh.GetIndexCount());

            vs.transform =
                view_proj *
                /*glm::inverse(
                    glm::lookAtRH(
                        model.pos, camera.GetPosition() - model.pos, glm::vec3(0.f, 1.f, 0.f)
                    )
                ) * */
                mat_model;
            framebuffer.render_elements(vbo, ibo, vs, fs);
        }

#endif

        // draw world-space axes
        {
            u32       cx = resx - 80, cy = 80;

            glm::vec3 x = glm::normalize(view * glm::vec4(1.f, 0.f, 0.f, 0.f));
            glm::vec3 y = glm::normalize(view * glm::vec4(0.f, 1.f, 0.f, 0.f));
            glm::vec3 z = glm::normalize(view * glm::vec4(0.f, 0.f, -1.f, 0.f));

            framebuffer.render_line(
                cx + x.x * 5.f, cy + x.y * 5.f, cx + x.x * 50.f, cy + x.y * 50.f, 0xff0000ff
            );
            framebuffer.render_line(
                cx - y.x * 5.f, cy - y.y * 5.f, cx - y.x * 50.f, cy - y.y * 50.f, 0x00ff00ff
            );
            framebuffer.render_line(
                cx + z.x * 5.f, cy + z.y * 5.f, cx + z.x * 50.f, cy + z.y * 50.f, 0x0000ffff
            );

            framebuffer.render_line(cx, cy, cx - x.x * 50.f, cy - x.y * 50.f, 0x646464ff);
            framebuffer.render_line(cx, cy, cx + y.x * 50.f, cy + y.y * 50.f, 0x646464ff);
            framebuffer.render_line(cx, cy, cx - z.x * 50.f, cy - z.y * 50.f, 0x646464ff);
        }

        // glm::mat4 skybox_rotation = glm::rotate(glm::mat4(1.f), dt * 0.2f,
        // rotation_axis);
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
        // shadowbuffer.Clear(BDEPTH);
        time_elapsed += std::chrono::system_clock::now() - time_frame_start;
    }

    std::clog << "Average perf. " << time_elapsed.count() / frames / 1E6f;

    return 0;
}
