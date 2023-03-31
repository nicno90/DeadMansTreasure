#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "GameMap.hpp"

MapPath::~MapPath() {
	for (Floor* f : floors) {
		delete f;
	}
}

GameMap::~GameMap() {
	for (MapPath* mp : paths) {
		delete mp;
	}
}

void GameMap::init_map(SDL_Texture** p_textures) {
	matrix = {
		{0.0, 0.24, 0.24, 0.24, 0.24, 0.4}, // B
		{0.15, 0.34, 0.0, 0.25, 0.25, 0.01}, // ^
		{0.15, 0.0, 0.34, 0.25, 0.25, 0.01}, // v
		{0.15, 0.25, 0.25, 0.34, 0.0, 0.01}, // <
		{0.15, 0.25, 0.25, 0.0, 0.34, 0.01}, // >
		{0.5, 0.1, 0.1, 0.1, 0.1, 0.1}, // e
	};

	float sum;
	for (int i = 0; i < matrix.size(); i++) {
		sum = std::accumulate(matrix[i].begin(), matrix[i].end(), 0.0);
		printf("Matrix %d = %f\n", i, sum);
	}
	textures = p_textures;
	homeBase = Floor(Vector2f(350, 250), *(textures + T_BIG_OPENING), FT_BIG_AREA, PT_AREA);
	floors = { &homeBase };
}


void GameMap::add_path() {
	paths.push_back(new MapPath(textures, &homeBase));
	MapPath* current_path = paths.back();
	int seed = time(NULL);
	std::cout << "Seed: " << seed << std::endl;
	srand(seed);
	double randNum = (double) rand() / RAND_MAX;
	double sum = 0.0;
	bool invalid_path[PT_END] = { false };
	//floors.push_back(current_path->add_floor((PathType) PT_RIGHT));
	//return;
	Floor* f;
	
	for (int i = 0; i < matrix.size(); ++i) {

		sum += matrix[current_path->lastPathType()][i];
		if (!invalid_path[i] && randNum < sum) {
			
			if (i == PT_END){
				// current_path->add_floor((PathType)i);
				current_path->fix_last_floor();
				return;
			}
			f = current_path->make_floor((PathType)i);
			
			for (int j = 0; j < sizeof(invalid_path); j++) {
				std::cout << invalid_path[j] <<  " ";
			}
			std::cout <<  std::endl;
			if (valid_placement(f)) {
				floors.push_back(f);
				current_path->attach_floor(f);

				for (int j = 0; j < sizeof(invalid_path); j++) {
					matrix[i][j] != 0.0 ? invalid_path[j] = false : invalid_path[j] = true;
				}

			}
			else {
				invalid_path[i] = true;
				delete f;
			}
			i = -1;
			sum = 0;
			randNum = (double) rand() / RAND_MAX;
		}
	}

	std::cout << "found way out of loop\nsum: " << sum << "\n" << std::endl;
}

bool GameMap::valid_placement(Floor* f) {
	SDL_Rect floorRec = { (int)f->getPos()->x, (int)f->getPos()->y - 25, f->getRect()->w, f->getRect()->h };
	for (Floor* f : floors) {

		if (f->floor_collision(&floorRec)) {
			return false;
		}
	}

	return true;
}

bool compareFloors(Floor* f1, Floor* f2) {
	return f1->getPos()->y < f2->getPos()->y;
}

std::vector<Floor*> GameMap::getFloors(){
	std::sort(floors.begin(), floors.end(), compareFloors);
	return floors;
}


Texture MapPath::fType_texture(FloorType fType){
	if (fType == FT_BIG_AREA){
		return T_BIG_OPENING;
	}
	else if (fType == FT_H_HALL || fType == FT_H_HALL_LEFT || fType == FT_H_HALL_RIGHT){
		return T_HORIZONTAL_PATH;
	}
	else if (fType == FT_V_HALL || fType == FT_V_HALL_BOTTOM || fType == FT_V_HALL_TOP){
		return T_VERTICAL_PATH;
	}

	return T_CHEST;
}

FloorType MapPath::pType_fType(PathType pType){
	switch (pType){
		case PT_AREA:
			return FT_BIG_AREA;
		case PT_DOWN:
			return FT_V_HALL;
		case PT_UP:
			return FT_V_HALL;
		case PT_LEFT:
			return FT_H_HALL;
		case PT_RIGHT:
			return FT_H_HALL;
	}
	return FT_BLANK;
}


void MapPath::getAttachmentPoint(PathType pType, AttachmentPoint* attachSide, AttachmentPoint* attachPoint) {
	*attachSide = AP_NULL;
	*attachPoint = AP_NULL;
	switch (lastFloor->getPtype()) {
	case PT_UP:
		switch (pType) {
		case PT_UP:
			std::cout << " up ";
			*attachSide = AP_TOP;
			break;
		case PT_LEFT:
			std::cout << " left ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_RIGHT:
			std::cout << " right ";
			*attachSide = AP_RIGHT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_AREA:
			std::cout << " area ";
			*attachSide = AP_TOP;
			break;
		case PT_END:
			//lastFloor->set_fType(FT_V_HALL_TOP);
			std::cout << " end ";
			break;
		}
		break;
	case PT_DOWN:
		switch (pType) {
		case PT_DOWN:
			std::cout << " down ";
			*attachSide = AP_BOTTOM;
			break;
		case PT_LEFT:
			std::cout << " left ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_BOTTOM;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_RIGHT:
			std::cout << " right ";
			*attachSide = AP_RIGHT;
			*attachPoint = AP_BOTTOM;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_AREA:
			std::cout << " area ";
			*attachSide = AP_BOTTOM;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_END:
			std::cout << " end ";
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);/
			break;
		};
		break;
	case PT_LEFT:
		switch (pType) {
		case PT_UP:
			std::cout << " up ";
			*attachSide = AP_TOP;
			*attachPoint = AP_LEFT;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_DOWN:
			std::cout << " down ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_LEFT;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_LEFT:
			std::cout << " left ";
			*attachSide = AP_LEFT;
			break;
		case PT_AREA:
			std::cout << " area ";
			*attachSide = AP_LEFT;
			break;
		case PT_END:
			std::cout << " end ";
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		};
		break;
	case PT_RIGHT:
		switch (pType) {
		case PT_UP:
			std::cout << " up ";
			*attachSide = AP_TOP;
			*attachPoint = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_DOWN:
			std::cout << " down ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_RIGHT:
			std::cout << " right ";
			*attachSide = AP_RIGHT;
			break;
		case PT_AREA:
			std::cout << " area ";
			*attachSide = AP_RIGHT;
			break;
		case PT_END:
			std::cout << " end ";
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		};
		break;
	case PT_AREA:
		switch (pType) {
		case PT_UP:
			std::cout << " up ";
			*attachSide = AP_TOP;
			break;
		case PT_DOWN:
			std::cout << " down ";
			*attachSide = AP_BOTTOM;
			break;
		case PT_LEFT:
			std::cout << " left ";
			*attachSide = AP_LEFT;
			break;
		case PT_RIGHT:
			std::cout << " right ";
			*attachSide = AP_RIGHT;
			break;
		}
		break;
	};
}

Floor* MapPath::make_floor(PathType pType) {
	AttachmentPoint attachSide, attachPoint;
	// std::cout << "Last floor type: " << lastFloor->getType() << std::endl;
	getAttachmentPoint(pType, &attachSide, &attachPoint);
	std::cout << pType << ": " << lastFloor->getPtype() << std::endl;
	
	return new Floor(*(textures + fType_texture(pType_fType(pType))), pType_fType(pType), pType, attachSide, attachPoint, lastFloor);

}

void MapPath::fix_last_floor() {
	PathType pType = floors.back()->getPtype();
	switch (lastFloor->getPtype()) {
	case PT_UP:
		switch (pType) {
		case PT_LEFT:
			lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_RIGHT:
			lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_END:
			lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		}
		break;
	case PT_DOWN:
		switch (pType) {
		case PT_LEFT:
			lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_RIGHT:
			lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_END:
			lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		};
		break;
	case PT_LEFT:
		switch (pType) {
		case PT_UP:
			lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_DOWN:
			lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_END:
			lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		};
		break;
	case PT_RIGHT:
		switch (pType) {
		case PT_UP:
			lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_DOWN:
			lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_END:
			lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		};
		break;
	};
}

void MapPath::attach_floor(Floor* f) {
	floors.push_back(f);
	fix_last_floor();
	lastFloor = f;
}

Floor* MapPath::operator[](int index){
	if (index == -1){
		return entryFloor;
	}
	return floors[index];
}

PathType MapPath::lastPathType(){
	if (floors.size() < 1){
		return entryFloor->getPtype();
	}
	return lastFloor->getPtype();
}

std::vector<Floor*> MapPath::getFloors(){
	return floors;
}