#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"



class MapPath {
    private:
        std::vector<std::vector<double>>* matrix;
        std::vector<Floor*> floors;
        Floor* entryFloor;
        Floor* lastFloor;
        SDL_Texture** textures;
        Texture fType_texture(FloorType fType);
        FloorType pType_fType(PathType pType);
        PathType fType_pType(FloorType fType);
        void getAttachmentPoint(PathType pType, AttachmentPoint* attachSide, AttachmentPoint* attachPoint);
        

    public:
        // MapPath(SDL_Texture* homebase); // for the center path
        MapPath(SDL_Texture** p_textures, Floor* p_entryFloor) : textures(p_textures), entryFloor(p_entryFloor), lastFloor(p_entryFloor){};
        ~MapPath();
        Floor* make_floor(PathType pType);
        void attach_floor(Floor* f);
        Floor* operator[](int index);
        PathType lastPathType();
        std::vector<Floor*> getFloors();
        void fix_last_floor();
        
};  

class GameMap {
    private:
        std::vector<std::vector<double>> matrix;
        std::vector<MapPath*> paths;
        Floor homeBase; 
        std::vector<Floor*> floors;
        SDL_Texture** textures;
        bool valid_placement(Floor* f);
    public:
        GameMap() {};
        ~GameMap();
        void add_path();
        void init_map(SDL_Texture** p_texxtures);
        std::vector<Floor*> getFloors();

        
};
