#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"



class MapPath {
	private:
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
		double matrix[PT_END + 1][PT_END + 1];
		std::vector<MapPath*> paths;
		Floor homeBase; 
		std::vector<Floor*> floors;
		SDL_Texture** textures;
		bool valid_placement(Floor* f);
		void reset_invalid_path(bool* invalid_path, PathType lastPath);
		bool no_valid_direction(bool* invalid_paths);
	public:
		GameMap() {};
		~GameMap();
		void generate();
		void generate(int seed);
		void add_path(Floor* entry);
		void init_map(SDL_Texture** p_texxtures);
		std::vector<Floor*> getFloors();
		void init_matrix();
		PathType next_path_type(PathType last_pType, double* randNum, double* sum);

		
};
