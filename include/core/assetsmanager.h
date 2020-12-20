#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gamestart
{

    class LoadedMesh
    {
    public:
        GLuint vao;
        GLuint vbo;
        int triangleCount;
        unsigned int materialId;
    };

    class LoadedAsset
    {
    public:
        GLuint shaderId;
        std::vector<LoadedMesh> loadedMeshes;
        glm::vec3 bbMin, bbMax;
    };

    class AssetsManager
    {
    public:
        AssetsManager();

        virtual ~AssetsManager();

        std::shared_ptr<LoadedAsset> LoadAsset(
            const std::string &assetName);

        void UnloadAsset(
            std::shared_ptr<LoadedAsset> asset);

    private:
        std::string _baseDirectory = ".";
        std::map<std::string, std::shared_ptr<LoadedAsset>> _loadedAssets;

        GLuint CompileShader(
            const std::string &vertShaderStr,
            const std::string &fragShaderStr);

        GLuint _meshWithoutAnimationShaderId = 0;
        GLuint GetMeshWithoutAnimationShader();
    };

} // namespace gamestart

#endif // ASSETSMANAGER_H
