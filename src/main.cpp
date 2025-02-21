#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window();        // On peut la maximiser si on veut

    auto camera = gl::Camera{};
    gl::set_events_callbacks({camera.events_callbacks()});

    auto const shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {
            .on_mouse_pressed = [&](gl::MousePressedEvent const &e)
            {
                std::cout << "Mouse pressed at " << e.position.x << " " << e.position.y << '\n';
            },
        },
    });

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width = gl::framebuffer_width_in_pixels(),
        .height = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter = gl::Filter::NearestNeighbour,  // On va toujours afficher la texture à la taille de l'écran,
                    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
                    .wrap_x = gl::Wrap::ClampToEdge,
                    .wrap_y = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x = gl::Wrap::ClampToEdge,
                .wrap_y = gl::Wrap::ClampToEdge,
            },
        },
    }};

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const &e)
         {
             if (e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
                 render_target.resize(e.width_in_pixels, e.height_in_pixels);
         }},
    });

    auto const texture = gl::Texture{
        gl::TextureSource::File{
            // Peut être un fichier, ou directement un tableau de pixels
            .path = "res/texture.png",
            .flip_y = true,                              // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear,  // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::Repeat,                 // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::Repeat,                 // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
            // .border_color = {0.4f, 0.f, 0.68f, 1.f},    // Couleur du bord si on lit en dehors de la texture et que le wrap mode est ClampToBorder.
        }};

    auto const fullscreen_quad = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}, gl::VertexAttribute::UV{1}},
            .data = {
                // Position2D   |  UV Coordinates
                -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
                +1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
                +1.0f, +1.0f, 1.0f, 1.0f, // Top-right
                -1.0f, +1.0f, 0.0f, 1.0f  // Top-left
            },
        }},
        .index_buffer = {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
        },
    }};

    auto const cube_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}}, // Using Position3D and UV
            .data = {
                // Position3D          |   UV Coordinates
                // Front face
                -0.5f, -0.5f, +0.5f, 0.0f, 0.0f, // Bottom-left-front
                +0.5f, -0.5f, +0.5f, 1.0f, 0.0f, // Bottom-right-front
                +0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-left-front

                // Back face
                -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, 0.0f, 1.0f, // Top-right-back
                -0.5f, +0.5f, -0.5f, 1.0f, 1.0f, // Top-left-back

                // Left face
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left-back
                -0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-left-back
                -0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-left-front
                -0.5f, -0.5f, +0.5f, 0.0f, 1.0f, // Bottom-left-front

                // Right face
                +0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-right-front
                +0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Bottom-right-front

                // Top face
                -0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-left-back
                +0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-left-front

                // Bottom face
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom-right-back
                +0.5f, -0.5f, +0.5f, 0.0f, 0.0f, // Bottom-right-front
                -0.5f, -0.5f, +0.5f, 1.0f, 0.0f, // Bottom-left-front
            },
        }},
        .index_buffer = {// Front face
                         0, 1, 2, 0, 2, 3,

                         // Back face
                         4, 5, 6, 4, 6, 7,

                         // Left face
                         8, 9, 10, 8, 10, 11,

                         // Right face
                         12, 13, 14, 12, 14, 15,

                         // Top face
                         16, 17, 18, 16, 18, 19,

                         // Bottom face
                         20, 21, 22, 20, 22, 23},
    }};

    auto const quad_shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/quad_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/quad_fragment.glsl"},
    }};

    while (gl::window_is_open())
    {
        // ✅ Render Scene into Render Target
        render_target.render([&]()
                             {
        glClearColor(0.4f, 0.f, 0.68f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shader.bind();
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("time", gl::time_in_seconds());
        
        glm::mat4 projection_matrix = glm::infinitePerspective(1.f, gl::framebuffer_aspect_ratio(), 0.001f);
        glm::mat4 view_matrix = camera.view_matrix();
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);

        glm::mat4 rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{0.f, 0.f, 1.f});
        glm::mat4 translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f});
        shader.set_uniform("model_matrix", translation * rotation);

        shader.set_uniform("my_texture", texture);
        cube_mesh.draw(); });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // ✅ Render Fullscreen Quad with Render Target Texture
        quad_shader.bind();
        quad_shader.set_uniform("screen_texture", render_target.color_texture(0));
        fullscreen_quad.draw();
    }
}