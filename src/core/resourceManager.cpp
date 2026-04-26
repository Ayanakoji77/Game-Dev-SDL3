#include "resourceManager.h"

#include <SDL3/SDL_filesystem.h>

#include <string>

ResourceManager::ResourceManager(SDL_Renderer* renderer, const char* basePath)
    : renderer(renderer), basePath(basePath)
{
}

ResourceManager::~ResourceManager() { UnloadAll(); }

void ResourceManager::LoadTexture(const std::string& name, const std::string& filepath)
{
    // Build full path
    std::string fullPath = std::string(basePath) + filepath;

    SDL_Texture* tex = IMG_LoadTexture(renderer, fullPath.c_str());
    if (tex)
    {
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        textures[name] = tex;
    }
    else
    {
        SDL_Log("Failed to load texture: %s", fullPath.c_str());
    }
}

SDL_Texture* ResourceManager::GetTexture(const std::string& name) const
{
    if (textures.find(name) != textures.end())
    {
        return textures.at(name);
    }
    return nullptr;
}

void ResourceManager::UnloadAll()
{
    for (auto& pair : textures)
    {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
