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

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}, gl::VertexAttribute::UV{1}},
            .data = {
                // Position2D   |  UV Coordinates
                -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
                +0.5f, -0.5f, 1.0f, 0.0f, // Bottom-right
                +0.5f, +0.5f, 1.0f, 1.0f, // Top-right
                -0.5f, +0.5f, 0.0f, 1.0f  // Top-left
            },
        }},
        .index_buffer = {
            0, 1, 2, // First triangle (Bottom-left, Bottom-right, Top-right)
            0, 2, 3  // Second triangle (Bottom-left, Top-right, Top-left)
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

    while (gl::window_is_open())
    {
        // Rendu à chaque frame
        glClearColor(0.4f, 0.f, 0.68f, 1.f);                // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        shader.bind();
        shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("time", gl::time_in_seconds());
        glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
        glm::mat4 const view_matrix = camera.view_matrix();
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
        glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */;
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f} /* déplacement */);
        // shader.set_uniform("model_matrix", rotation * translation);
        shader.set_uniform("model_matrix", translation * rotation);
        shader.set_uniform("my_texture", texture);
        cube_mesh.draw();
    }
}