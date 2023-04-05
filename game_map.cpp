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
		{0.0, 0.24, 0.24, 0.24, 0.24, 0.04}, // B
		{0.24, 0.25, 0.0, 0.25, 0.25, 0.01}, // ^
		{0.24, 0.0, 0.25, 0.25, 0.25, 0.01}, // v
		{0.24, 0.25, 0.25, 0.25, 0.0, 0.01}, // <
		{0.24, 0.25, 0.25, 0.0, 0.25, 0.01}, // >
		{0.0, 0.25, 0.25, 0.25, 0.25, 0.0}, // e
	};

	float sum;
	for (int i = 0; i < matrix.size(); i++) {
		sum = std::accumulate(matrix[i].begin(), matrix[i].end(), 0.0);
		printf("Matrix %d = %f\n", i, sum);
	}
	textures = p_textures;
	homeBase = Floor(Vector2f(350, 250), *(textures + T_BIG_OPENING), FT_BIG_AREA, PT_AREA);
	std::cout << "\n&homebase: " << &homeBase << std::endl;
	floors = { &homeBase };
}


void GameMap::add_path() {
	paths.push_back(new MapPath(textures, &homeBase));
	MapPath* current_path = paths.back();
	int seed = time(NULL);
	printf("\n\nNew Path:\n");
	std::cout << "Seed: " << seed << std::endl;
	srand(seed);
	double randNum = (double) rand() / RAND_MAX;
	double sum = 0.0;
	bool invalid_path[PT_END+1] = { false };
	int minLen = 50, maxLen = 200, size = 0;
	
	Floor* f;
	
	for (int i = 0; i < matrix.size(); ++i) {
		sum += matrix[current_path->lastPathType()][i];
		if (!invalid_path[i] && randNum < sum) {
			
			if (i == PT_END || size >= maxLen){
				// current_path->add_floor((PathType)i);
				if (size < minLen) {
					i = -1;
					sum = 0.0;
					randNum = (double)rand() / RAND_MAX;
					std::cout << "New Rand: " << randNum << std::endl;
					continue;
				}
				std::cout << "Found end" << std::endl;
				current_path->fix_last_floor();
				return;
			}
			f = current_path->make_floor((PathType)i);
			
			if (valid_placement(f)) {
				floors.push_back(f);
				current_path->attach_floor(f);
				std::cout << "--valid--" << std::endl;
				std::cout << "Adding: " << f << std::endl;
				size++;


				// print valid array
				std::cout << "[ ";
				for (int j = 0; j < PT_END; ++j) {
					std::cout << invalid_path[j] << ", ";
					matrix[i][j] != 0.0 ? invalid_path[j] = false : invalid_path[j] = true;
				}
				
				invalid_path[PT_END - 1] = size < minLen;
				std::cout << invalid_path[PT_END] << " ]\n\n" << std::endl;

			}
			else {
				if (i == PT_DOWN || i == PT_UP) {
					printf("floor: {%d, %d, %d, %d}\n", f->get_hitbox()->x, f->get_hitbox()->y, f->get_hitbox()->w, f->get_hitbox()->h);
				}

				invalid_path[i] = true;
				delete f;
			}
			
			i = -1;
			sum = 0.0;
			randNum = (double) rand() / RAND_MAX;
			std::cout << "New Rand: " << randNum << std::endl;
			
		}
		else if (i == matrix.size()-1 && size < minLen) {
			//for (bool& invld_pth : invalid_path) {
			for (int j = 0; j < PT_END-1; ++j) {
				if (!invalid_path[j]) {
					i = -1;
					sum = 0.0;
					randNum = (double)rand() / RAND_MAX;
					std::cout << "\nNew Rand: " << randNum << std::endl;
					return;
				}
			}
			std::cout << "No Valid Path Forward\n" << std::endl;
			current_path->fix_last_floor();
		}
	}

	std::cout << "found way out of loop\nsum: " << sum << "\nsize: "<< size << "\n" << std::endl;
}

bool GameMap::valid_placement(Floor* p_f) {
	SDL_Rect floorRec;
	int xmod = 0, ymod = 0;
	if (p_f->getPtype() == PT_DOWN) {
		ymod = 20;
	}
	else if (p_f->getPtype() == PT_UP) {
		ymod = -60;
	}

	floorRec = { (int)p_f->getPos()->x+xmod, (int)p_f->getPos()->y+ymod, p_f->getRect()->w, p_f->getRect()->h };

	for (Floor* floor : floors) {
		
		
		if (floor->floor_collision(&floorRec)) {
			// printf("downHall y, bottom y: %d, %d\n", (int)floorRec.y, (int)floor->getPos()->y + floor->getRect()->h);
			std::cout << "Invalid from pType:" << floor->getPtype() << ", &id:" << floor << std::endl;

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
			std::cout << " UP ";
			*attachSide = AP_TOP;
			break;
		case PT_LEFT:
			std::cout << " LEFT ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_RIGHT:
			std::cout << " RIGHT ";
			*attachSide = AP_RIGHT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_AREA:
			std::cout << " AREA ";
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
			std::cout << " DOWN ";
			*attachSide = AP_BOTTOM;
			break;
		case PT_LEFT:
			std::cout << " LEFT ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_BOTTOM;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_RIGHT:
			std::cout << " RIGHT ";
			*attachSide = AP_RIGHT;
			*attachPoint = AP_BOTTOM;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_AREA:
			std::cout << " AREA ";
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
			std::cout << " UP ";
			*attachSide = AP_TOP;
			*attachPoint = AP_LEFT;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_DOWN:
			std::cout << " DOWN ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_LEFT;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_LEFT:
			std::cout << " LEFT ";
			*attachSide = AP_LEFT;
			break;
		case PT_AREA:
			std::cout << " AREA ";
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
			std::cout << " UP ";
			*attachSide = AP_TOP;
			*attachPoint = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_DOWN:
			std::cout << " DOWN ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_RIGHT:
			std::cout << " RIGHT ";
			*attachSide = AP_RIGHT;
			break;
		case PT_AREA:
			std::cout << " AREA ";
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
			std::cout << " UP ";
			*attachSide = AP_TOP;
			break;
		case PT_DOWN:
			std::cout << " DOWN ";
			*attachSide = AP_BOTTOM;
			break;
		case PT_LEFT:
			std::cout << " LEFT ";
			*attachSide = AP_LEFT;
			break;
		case PT_RIGHT:
			std::cout << " RIGHT ";
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