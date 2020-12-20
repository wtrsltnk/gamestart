#include <core/assetsmanager.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

using namespace gamestart;

AssetsManager::AssetsManager()
{
    _baseDirectory = (std::filesystem::current_path() / std::filesystem::path("assets")).string();

    spdlog::debug("setting base directory to {}", _baseDirectory);
}

AssetsManager::~AssetsManager() = default;

typedef struct
{
    GLuint va_id;
    GLuint vb_id;
    int numTriangles;
    int material_id;
} DrawObject;

static bool LoadObjAndConvert(
    float bmin[3],
    float bmax[3],
    std::vector<DrawObject> *drawObjects,
    std::vector<tinyobj::material_t> &materials,
    std::map<std::string, GLuint> &textures,
    const char *filename,
    const char *base_dir);

GLuint AssetsManager::CompileShader(
    const std::string &vertShaderStr,
    const std::string &fragShaderStr)
{
    const char *vertShaderSrc = vertShaderStr.c_str();

    GLint result = GL_FALSE;
    GLint logLength;

    // Compile vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<GLchar> vertShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));

        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);

        spdlog::error("error compiling vertex shader {}", vertShaderError.data());

        return 0;
    }

    const char *fragShaderSrc = fragShaderStr.c_str();

    // Compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<GLchar> fragShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));

        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);

        spdlog::error("error compiling fragment shader {}", fragShaderError.data());

        return 0;
    }

    auto shaderId = glCreateProgram();
    glAttachShader(shaderId, vertShader);
    glAttachShader(shaderId, fragShader);
    glLinkProgram(shaderId);

    glGetProgramiv(shaderId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<GLchar> programError(static_cast<size_t>((logLength > 1) ? logLength : 1));

        glGetProgramInfoLog(shaderId, logLength, NULL, &programError[0]);

        spdlog::error("error linking shader {}", programError.data());

        return 0;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    //    _projectionUniformId = glGetUniformLocation(_shaderId, _projectionUniformName.c_str());
    //    _viewUniformId = glGetUniformLocation(_shaderId, _viewUniformName.c_str());
    //    _modelUniformId = glGetUniformLocation(_shaderId, _modelUniformName.c_str());

    //    _textureUniform1Id = glGetUniformLocation(_shaderId, _textureUniform1Name.c_str());
    //    _textureUniform2Id = glGetUniformLocation(_shaderId, _textureUniform2Name.c_str());
    //    _textureUniform3Id = glGetUniformLocation(_shaderId, _textureUniform3Name.c_str());
    //    _textureUniformMaskId = glGetUniformLocation(_shaderId, _textureUniformMaskName.c_str());

    return shaderId;
}

GLuint AssetsManager::GetMeshWithoutAnimationShader()
{
    if (_meshWithoutAnimationShaderId == 0)
    {
        std::string const vshader(
            "#version 150\n"

            "in vec3 vertex;\n"
            "in vec3 normal;\n"
            "in vec3 color;\n"
            "in vec2 texcoords;\n"

            "uniform mat4 u_projection;\n"
            "uniform mat4 u_model;\n"

            "out vec3 f_color;\n"
            "out vec2 f_uvs;\n"

            "void main()\n"
            "{\n"
            "    gl_Position = u_projection * u_model * vec4(vertex.xyz, 1.0);\n"
            "    f_color = color;\n"
            "    f_uvs = texcoords;\n"
            "}\n");

        std::string const fshader(
            "#version 150\n"

            //"uniform sampler2D u_texture;\n"

            "in vec3 f_color;\n"
            "in vec2 f_uvs;\n"
            "out vec4 color;\n"

            "void main()\n"
            "{\n"
            "    color = vec4(f_color.xyz, 1.0);\n"
            //"   color = texture(u_texture, f_uvs.uv);\n"
            "}\n");

        _meshWithoutAnimationShaderId = CompileShader(vshader, fshader);
    }

    return _meshWithoutAnimationShaderId;
}

std::shared_ptr<LoadedAsset> AssetsManager::LoadAsset(
    const std::string &assetName)
{
    const auto &loadedAsset = _loadedAssets.find(assetName);
    if (loadedAsset != _loadedAssets.end())
    {
        return loadedAsset->second;
    }

    auto asset = std::make_shared<LoadedAsset>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::material_t> materials;
    std::map<std::string, GLuint> textures;
    float bmin[3], bmax[3];

    std::vector<DrawObject> drawObjects;

    auto result = LoadObjAndConvert(
        bmin,
        bmax,
        &drawObjects,
        materials,
        textures,
        assetName.c_str(),
        _baseDirectory.c_str());

    if (result)
    {
        asset.get()->bbMax = glm::vec3(bmax[0], bmax[1], bmax[2]);
        asset.get()->bbMin = glm::vec3(bmin[0], bmin[1], bmin[2]);

        asset.get()->shaderId = GetMeshWithoutAnimationShader();

        if (asset.get()->shaderId > 0)
        {
            for (auto obj : drawObjects)
            {
                LoadedMesh mesh;
                mesh.materialId = obj.material_id;
                mesh.triangleCount = obj.numTriangles;
                mesh.vbo = obj.vb_id;

                asset.get()->loadedMeshes.push_back(mesh);
            }
        }
        else
        {
            spdlog::error("failed to 'mesh-without-animation' shader for {}", assetName);
        }
    }
    else
    {
        spdlog::error("failed to load {}", assetName);
    }

    _loadedAssets.insert(std::make_pair(assetName, asset));

    return _loadedAssets
        .find(assetName)
        ->second;
}

void AssetsManager::UnloadAsset(
    std::shared_ptr<LoadedAsset> asset)
{
    (void)asset;
}

namespace // Local utility functions
{
    static bool FileExists(
        const std::string &abs_filename)
    {
        if (!std::filesystem::exists(std::filesystem::path(abs_filename)))
        {
            spdlog::error("{} does not exist", abs_filename);

            return false;
        }

        return true;
    }

    struct vec3
    {
        float v[3];
        vec3()
        {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
        }
    };

    void normalizeVector(vec3 &v)
    {
        float len2 = v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            v.v[0] /= len;
            v.v[1] /= len;
            v.v[2] /= len;
        }
    }

    // Check if `mesh_t` contains smoothing group id.
    bool hasSmoothingGroup(const tinyobj::shape_t &shape)
    {
        for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++)
        {
            if (shape.mesh.smoothing_group_ids[i] > 0)
            {
                return true;
            }
        }
        return false;
    }

    static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3])
    {
        float v10[3];
        v10[0] = v1[0] - v0[0];
        v10[1] = v1[1] - v0[1];
        v10[2] = v1[2] - v0[2];

        float v20[3];
        v20[0] = v2[0] - v0[0];
        v20[1] = v2[1] - v0[1];
        v20[2] = v2[2] - v0[2];

        N[0] = v10[1] * v20[2] - v10[2] * v20[1];
        N[1] = v10[2] * v20[0] - v10[0] * v20[2];
        N[2] = v10[0] * v20[1] - v10[1] * v20[0];

        float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            N[0] /= len;
            N[1] /= len;
            N[2] /= len;
        }
    }

    void computeSmoothingNormals(
        const tinyobj::attrib_t &attrib,
        const tinyobj::shape_t &shape,
        std::map<int, vec3> &smoothVertexNormals)
    {
        smoothVertexNormals.clear();
        std::map<int, vec3>::iterator iter;

        for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++)
        {
            // Get the three indexes of the face (all faces are triangular)
            tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

            // Get the three vertex indexes and coordinates
            int vi[3];     // indexes
            float v[3][3]; // coordinates

            for (int k = 0; k < 3; k++)
            {
                vi[0] = idx0.vertex_index;
                vi[1] = idx1.vertex_index;
                vi[2] = idx2.vertex_index;
                assert(vi[0] >= 0);
                assert(vi[1] >= 0);
                assert(vi[2] >= 0);

                v[0][k] = attrib.vertices[3 * vi[0] + k];
                v[1][k] = attrib.vertices[3 * vi[1] + k];
                v[2][k] = attrib.vertices[3 * vi[2] + k];
            }

            // Compute the normal of the face
            float normal[3];
            CalcNormal(normal, v[0], v[1], v[2]);

            // Add the normal to the three vertexes
            for (size_t i = 0; i < 3; ++i)
            {
                iter = smoothVertexNormals.find(vi[i]);
                if (iter != smoothVertexNormals.end())
                {
                    // add
                    iter->second.v[0] += normal[0];
                    iter->second.v[1] += normal[1];
                    iter->second.v[2] += normal[2];
                }
                else
                {
                    smoothVertexNormals[vi[i]].v[0] = normal[0];
                    smoothVertexNormals[vi[i]].v[1] = normal[1];
                    smoothVertexNormals[vi[i]].v[2] = normal[2];
                }
            }

        } // f

        // Normalize the normals, that is, make them unit vectors
        for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end(); iter++)
        {
            normalizeVector(iter->second);
        }
    } // computeSmoothingNormals

} // namespace

static bool LoadObjAndConvert(
    float bmin[3],
    float bmax[3],
    std::vector<DrawObject> *drawObjects,
    std::vector<tinyobj::material_t> &materials,
    std::map<std::string, GLuint> &textures,
    const char *filename,
    const char *base_dir)
{
    auto fullPath = std::filesystem::path(base_dir) / std::filesystem::path(filename);

    if (!FileExists(fullPath.string()))
    {
        return false;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        fullPath.string().c_str(),
        base_dir);

    if (!warn.empty())
    {
        spdlog::warn(warn);
    }

    if (!err.empty())
    {
        spdlog::error(err);
    }

    if (!ret)
    {
        return false;
    }

    spdlog::info("# of vertices  = {}", (int)(attrib.vertices.size()) / 3);
    spdlog::info("# of normals   = {}", (int)(attrib.normals.size()) / 3);
    spdlog::info("# of texcoords = {}", (int)(attrib.texcoords.size()) / 2);
    spdlog::info("# of materials = {}", (int)materials.size());
    spdlog::info("# of shapes    = {}", (int)shapes.size());

    // Append `default` material
    materials.push_back(tinyobj::material_t());

    for (size_t i = 0; i < materials.size(); i++)
    {
        spdlog::info("material[{}].diffuse_texname = {}", int(i), materials[i].diffuse_texname.c_str());
    }

    // Load diffuse textures
    {
        for (size_t m = 0; m < materials.size(); m++)
        {
            tinyobj::material_t *mp = &materials[m];

            if (mp->diffuse_texname.length() > 0)
            {
                // Only load the texture if it is not already loaded
                if (textures.find(mp->diffuse_texname) == textures.end())
                {
                    GLuint texture_id;
                    int w, h;
                    int comp;

                    std::string texture_filename = mp->diffuse_texname;
                    if (!FileExists(texture_filename))
                    {
                        // Append base dir.
                        texture_filename = base_dir + mp->diffuse_texname;
                        if (!FileExists(texture_filename))
                        {
                            spdlog::error("Unable to find file: {}", mp->diffuse_texname);

                            exit(1);
                        }
                    }

                    unsigned char *image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
                    if (!image)
                    {
                        spdlog::error("Unable to load texture: {}", texture_filename);

                        exit(1);
                    }

                    spdlog::info("Loaded texture: {}, w = {}, h = {}, comp = {}", texture_filename, w, h, comp);

                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    if (comp == 3)
                    {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                    }
                    else if (comp == 4)
                    {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                    }
                    else
                    {
                        assert(0); // TODO
                    }
                    glBindTexture(GL_TEXTURE_2D, 0);
                    stbi_image_free(image);
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                }
            }
        }
    }

    bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
    bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

    {
        for (size_t s = 0; s < shapes.size(); s++)
        {
            DrawObject o;
            std::vector<float> buffer; // pos(3float), normal(3float), color(3float)

            // Check for smoothing group and compute smoothing normals
            std::map<int, vec3> smoothVertexNormals;
            if (hasSmoothingGroup(shapes[s]))
            {
                spdlog::info("Compute smoothingNormal for shape [{}]", s);

                computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
            }

            for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
            {
                tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

                int current_material_id = shapes[s].mesh.material_ids[f];

                if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size())))
                {
                    // Invaid material ID. Use default material.
                    // Default material is added to the last item in `materials`.
                    current_material_id = static_cast<int>(materials.size()) - 1;
                }

                // if (current_material_id >= materials.size()) {
                //    std::cerr << "Invalid material index: " << current_material_id <<
                //    std::endl;
                //}
                //
                float diffuse[3];
                for (size_t i = 0; i < 3; i++)
                {
                    diffuse[i] = materials[current_material_id].diffuse[i];
                }

                float tc[3][2];

                if (attrib.texcoords.size() > 0)
                {
                    if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) || (idx2.texcoord_index < 0))
                    {
                        // face does not contain valid uv index.
                        tc[0][0] = 0.0f;
                        tc[0][1] = 0.0f;
                        tc[1][0] = 0.0f;
                        tc[1][1] = 0.0f;
                        tc[2][0] = 0.0f;
                        tc[2][1] = 0.0f;
                    }
                    else
                    {
                        assert(attrib.texcoords.size() > size_t(2 * idx0.texcoord_index + 1));
                        assert(attrib.texcoords.size() > size_t(2 * idx1.texcoord_index + 1));
                        assert(attrib.texcoords.size() > size_t(2 * idx2.texcoord_index + 1));

                        // Flip Y coord.
                        tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                        tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
                        tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                        tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
                        tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                        tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
                    }
                }
                else
                {
                    tc[0][0] = 0.0f;
                    tc[0][1] = 0.0f;
                    tc[1][0] = 0.0f;
                    tc[1][1] = 0.0f;
                    tc[2][0] = 0.0f;
                    tc[2][1] = 0.0f;
                }

                float v[3][3];
                for (int k = 0; k < 3; k++)
                {
                    int f0 = idx0.vertex_index;
                    int f1 = idx1.vertex_index;
                    int f2 = idx2.vertex_index;
                    assert(f0 >= 0);
                    assert(f1 >= 0);
                    assert(f2 >= 0);

                    v[0][k] = attrib.vertices[3 * f0 + k];
                    v[1][k] = attrib.vertices[3 * f1 + k];
                    v[2][k] = attrib.vertices[3 * f2 + k];
                    bmin[k] = std::min(v[0][k], bmin[k]);
                    bmin[k] = std::min(v[1][k], bmin[k]);
                    bmin[k] = std::min(v[2][k], bmin[k]);
                    bmax[k] = std::max(v[0][k], bmax[k]);
                    bmax[k] = std::max(v[1][k], bmax[k]);
                    bmax[k] = std::max(v[2][k], bmax[k]);
                }

                float n[3][3];
                {
                    bool invalid_normal_index = false;
                    if (attrib.normals.size() > 0)
                    {
                        int nf0 = idx0.normal_index;
                        int nf1 = idx1.normal_index;
                        int nf2 = idx2.normal_index;

                        if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
                        {
                            // normal index is missing from this face.
                            invalid_normal_index = true;
                        }
                        else
                        {
                            for (int k = 0; k < 3; k++)
                            {
                                assert(size_t(3 * nf0 + k) < attrib.normals.size());
                                assert(size_t(3 * nf1 + k) < attrib.normals.size());
                                assert(size_t(3 * nf2 + k) < attrib.normals.size());
                                n[0][k] = attrib.normals[3 * nf0 + k];
                                n[1][k] = attrib.normals[3 * nf1 + k];
                                n[2][k] = attrib.normals[3 * nf2 + k];
                            }
                        }
                    }
                    else
                    {
                        invalid_normal_index = true;
                    }

                    if (invalid_normal_index && !smoothVertexNormals.empty())
                    {
                        // Use smoothing normals
                        int f0 = idx0.vertex_index;
                        int f1 = idx1.vertex_index;
                        int f2 = idx2.vertex_index;

                        if (f0 >= 0 && f1 >= 0 && f2 >= 0)
                        {
                            n[0][0] = smoothVertexNormals[f0].v[0];
                            n[0][1] = smoothVertexNormals[f0].v[1];
                            n[0][2] = smoothVertexNormals[f0].v[2];

                            n[1][0] = smoothVertexNormals[f1].v[0];
                            n[1][1] = smoothVertexNormals[f1].v[1];
                            n[1][2] = smoothVertexNormals[f1].v[2];

                            n[2][0] = smoothVertexNormals[f2].v[0];
                            n[2][1] = smoothVertexNormals[f2].v[1];
                            n[2][2] = smoothVertexNormals[f2].v[2];

                            invalid_normal_index = false;
                        }
                    }

                    if (invalid_normal_index)
                    {
                        // compute geometric normal
                        CalcNormal(n[0], v[0], v[1], v[2]);
                        n[1][0] = n[0][0];
                        n[1][1] = n[0][1];
                        n[1][2] = n[0][2];
                        n[2][0] = n[0][0];
                        n[2][1] = n[0][1];
                        n[2][2] = n[0][2];
                    }
                }

                for (int k = 0; k < 3; k++)
                {
                    buffer.push_back(v[k][0]);
                    buffer.push_back(v[k][1]);
                    buffer.push_back(v[k][2]);
                    buffer.push_back(n[k][0]);
                    buffer.push_back(n[k][1]);
                    buffer.push_back(n[k][2]);
                    // Combine normal and diffuse to get color.
                    float normal_factor = 0.2f;
                    float diffuse_factor = 1 - normal_factor;
                    float c[3] = {
                        n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                        n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                        n[k][2] * normal_factor + diffuse[2] * diffuse_factor,
                    };
                    float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                    if (len2 > 0.0f)
                    {
                        float len = sqrtf(len2);

                        c[0] /= len;
                        c[1] /= len;
                        c[2] /= len;
                    }
                    buffer.push_back(c[0] * 0.5f + 0.5f);
                    buffer.push_back(c[1] * 0.5f + 0.5f);
                    buffer.push_back(c[2] * 0.5f + 0.5f);

                    buffer.push_back(tc[k][0]);
                    buffer.push_back(tc[k][1]);
                }
            }

            o.vb_id = 0;
            o.numTriangles = 0;

            // OpenGL viewer does not support texturing with per-face material.
            if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s)
            {
                o.material_id = shapes[s].mesh.material_ids[0]; // use the material ID
                                                                // of the first face.
            }
            else
            {
                o.material_id = static_cast<int>(materials.size()) - 1; // = ID for default material.
            }

            spdlog::info("shape[{}] material_id {}", int(s), int(o.material_id));

            auto vertexAttribSize = 3 / sizeof(float);
            auto normalAttribSize = 3 / sizeof(float);
            auto colorAttribSize = 3 / sizeof(float);
            auto texcoordsAttribSize = 2 / sizeof(float);

            auto stride = vertexAttribSize + normalAttribSize + colorAttribSize + texcoordsAttribSize;

            if (buffer.size() > 0)
            {
                glGenVertexArrays(1, &o.va_id);
                glGenBuffers(1, &o.vb_id);

                glBindVertexArray(o.va_id);
                glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);

                glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
                o.numTriangles = static_cast<int>(buffer.size()) / (3 + 3 + 3 + 2) / 3; // 3:vtx, 3:normal, 3:col, 2:texcoord

                // vertex positions
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
                // vertex normals
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)vertexAttribSize);
                // vertex colors
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(vertexAttribSize + normalAttribSize));
                // vertex texture coords
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(vertexAttribSize + normalAttribSize + colorAttribSize));

                glBindVertexArray(0);

                spdlog::info("shape[{}] # of triangles = {}", static_cast<int>(s), o.numTriangles);
            }

            drawObjects->push_back(o);
        }
    }

    spdlog::info("bmin = {}, {}, {}", bmin[0], bmin[1], bmin[2]);
    spdlog::info("bmax = {}, {}, {}", bmax[0], bmax[1], bmax[2]);

    return true;
}
