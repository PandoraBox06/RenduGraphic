#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <iostream>

auto load_mesh(std::filesystem::path const &path) -> gl::Mesh
{
    // On lit le fichier avec tinyobj
    auto reader = tinyobj::ObjReader{};
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty())
        throw std::runtime_error("Failed to read 3D model:\n" + reader.Error());
    if (!reader.Warning().empty())
        std::cout << "Warning while reading 3D model:\n" + reader.Warning();

    // On met tous les attributs dans un tableau
    auto vertices = std::vector<float>{};
    for (auto const &shape : reader.GetShapes())
    {
        for (auto const &idx : shape.mesh.indices)
        {
            // Position
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]);

            // UV
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normale
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
        };
    }

    auto mesh = gl::Mesh{{.vertex_buffers = {{.layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}, gl::VertexAttribute::Normal3D{2}},
                                              .data = {vertices}}}}};

    return mesh;
}

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window();        // On peut la maximiser si on veut

    auto const quad_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}, gl::VertexAttribute::UV(1)},
            .data = {
                -1, -1, 0, 0,
                1, -1, 1, 0,
                -1, 1, 0, 1,
                1, 1, 1, 1},
        }},
        .index_buffer = {0, 1, 2, 1, 2, 3},
    }};

    auto const shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const shaderQuad = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/quad_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/quad_fragment.glsl"},
    }};

    auto camera = gl::Camera{};
    gl::set_events_callbacks({camera.events_callbacks()});

    auto const cube_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}, gl::VertexAttribute::UV(1)},
            .data = {
                // Face en bas
                -0.5f, -0.5f, -0.5f, 0, 1, // haut gauche : 0
                0.5f, -0.5f, -0.5f, 1, 1,  // haut droite : 1
                -0.5f, -0.5f, 0.5f, 0, 0,  // bas gauche : 2
                0.5f, -0.5f, 0.5f, 1, 0,   // bas droite : 3
                // Face en haut
                -0.5f, 0.5f, -0.5f, 0, 1, // haut gauche : 4
                0.5f, 0.5f, -0.5f, 1, 1,  // haut droite : 5
                -0.5f, 0.5f, 0.5f, 0, 0,  // bas gauche : 6
                0.5f, 0.5f, 0.5f, 1, 0    // bas droite : 7
            },
        }},
        .index_buffer = {// Face en bas
                         0, 1, 2, 1, 2, 3,
                         // Face en haut
                         4, 5, 6, 5, 6, 7,
                         // Face dèrrière
                         0, 1, 4, 1, 4, 5,
                         // Face devant
                         2, 3, 6, 3, 6, 7,
                         // Face gauche
                         0, 2, 4, 2, 4, 6,
                         // Face droite
                         1, 3, 5, 3, 5, 7},
    }};

    glEnable(GL_DEPTH_TEST);

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
            .wrap_x = gl::Wrap::MirroredRepeat,         // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::MirroredRepeat,         // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }};

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

    auto const mesh_model = load_mesh("res/fourareen.obj");

    auto const fourareen_texture = gl::Texture{
        gl::TextureSource::File{
            // Peut être un fichier, ou directement un tableau de pixels
            .path = "res/fourareen2K_albedo.jpg",
            .flip_y = true,                              // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear,  // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::MirroredRepeat,         // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::MirroredRepeat,         // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }};

    while (gl::window_is_open())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_target.render([&]()
                             {
            glClearColor(0.5f, 0.f, 0.48f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.bind();

            glm::mat4 const view_matrix = camera.view_matrix();
            glm::mat4 const projection_matrix = glm::infinitePerspective(glm::radians(90.f), gl::framebuffer_aspect_ratio(), 0.001f /*near plane*/);
            glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 0.f, 0.f});

            glm::mat4 const view_projection_matrix = projection_matrix * view_matrix * translation;

            shader.set_uniform("rotation_speed", 1.f);
            shader.set_uniform("time", gl::time_in_seconds());
            shader.set_uniform("view_projection_matrix", view_projection_matrix);
            shader.set_uniform("my_texture", fourareen_texture);
            shader.set_uniform("light_direction", glm::vec3(glm::normalize(glm::vec3(0.2, 0.3, -1))));
            shader.set_uniform("point_light", glm::vec3(0, 0, 0));
            mesh_model.draw(); });

        shaderQuad.bind();
        shaderQuad.set_uniform("my_texture", render_target.color_texture(0));
        quad_mesh.draw();
    }
}