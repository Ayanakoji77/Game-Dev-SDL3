#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <string>
#include <unordered_map>

class ResourceManager
{
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textures;
    const char* basePath;

   public:
    ResourceManager(SDL_Renderer* renderer, const char* basePath);
    ~ResourceManager();

    void LoadTexture(const std::string& name, const std::string& filepath);

    SDL_Texture* GetTexture(const std::string& name) const;

    void UnloadAll();
};
