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

    auto const cube_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0}}, // Using Position3D for 3D coordinates
            .data = {
                // Front face
                -0.5f, -0.5f, +0.5f, // Bottom-left-front
                +0.5f, -0.5f, +0.5f, // Bottom-right-front
                +0.5f, +0.5f, +0.5f, // Top-right-front
                -0.5f, +0.5f, +0.5f, // Top-left-front

                // Back face
                -0.5f, -0.5f, -0.5f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, // Top-right-back
                -0.5f, +0.5f, -0.5f, // Top-left-back
            },
        }},
        .index_buffer = {// Front face
                         0, 1, 2, 0, 2, 3,

                         // Back face
                         4, 5, 6, 4, 6, 7,

                         // Left face
                         0, 3, 7, 0, 7, 4,

                         // Right face
                         1, 5, 6, 1, 6, 2,

                         // Top face
                         3, 2, 6, 3, 6, 7,

                         // Bottom face
                         0, 1, 5, 0, 5, 4},
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
        cube_mesh.draw();
    }
}