#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "GameMap.hpp"


GameMap::~GameMap() {
	for (MapPath* mp : paths) {
		delete mp;
	}
}

void GameMap::init_map(SDL_Texture** p_textures) {
	init_matrix();
	float sum;
	for (int i = 0; i < PT_END+1; i++) {
		sum = 0.0;
		for (int j = 0; j < PT_END + 1; j++) {
			sum += matrix[i][j];
		}
		printf("Matrix %d = %f\n", i, sum);
	}
	textures = p_textures;
	homeBase = Floor(Vector2f(350, 250), *(textures + T_BIG_OPENING), FT_BIG_AREA, PT_AREA);
	std::cout << "\n&homebase: " << &homeBase << std::endl;
	floors = { &homeBase };
}


void GameMap::init_matrix() {

	for (int i = 0; i < PT_END + 1; ++i) {
		for (int j = 0; j < PT_END + 1; ++j) {
			matrix[i][j] = 0.0;
		}
	}

	double continue_path = 0.4;
	double corner = 0.15; // x2
	double path_to_area = 0.3;

	// AREA
	matrix[PT_AREA][PT_UP] = 0.24;
	matrix[PT_AREA][PT_DOWN] = 0.24;
	matrix[PT_AREA][PT_LEFT] = 0.24;
	matrix[PT_AREA][PT_RIGHT] = 0.24;
	matrix[PT_AREA][PT_END] = 0.04;

	// UP
	matrix[PT_UP][PT_UP] = continue_path;
	matrix[PT_UP][PT_CORNER_TL] = corner;
	matrix[PT_UP][PT_CORNER_TR] = corner;
	matrix[PT_UP][PT_AREA] = path_to_area;

	// DOWN
	matrix[PT_DOWN][PT_DOWN] = continue_path;
	matrix[PT_DOWN][PT_CORNER_BL] = corner;
	matrix[PT_DOWN][PT_CORNER_BR] = corner;
	matrix[PT_DOWN][PT_AREA] = path_to_area;

	// LEFT
	matrix[PT_LEFT][PT_LEFT] = continue_path;
	matrix[PT_LEFT][PT_CORNER_TL] = corner;
	matrix[PT_LEFT][PT_CORNER_BL] = corner;
	matrix[PT_LEFT][PT_AREA] = path_to_area;

	// RIGHT
	matrix[PT_RIGHT][PT_RIGHT] = continue_path;
	matrix[PT_RIGHT][PT_CORNER_TR] = corner;
	matrix[PT_RIGHT][PT_CORNER_BR] = corner;
	matrix[PT_RIGHT][PT_AREA] = path_to_area;

	//CORNERS
	matrix[PT_CORNER_TL][PT_DOWN] = 0.5;
	matrix[PT_CORNER_TL][PT_RIGHT] = 0.5;

	matrix[PT_CORNER_TR][PT_DOWN] = 0.5;
	matrix[PT_CORNER_TR][PT_LEFT] = 0.5;

	matrix[PT_CORNER_BL][PT_UP] = 0.5;
	matrix[PT_CORNER_BL][PT_RIGHT] = 0.5;

	matrix[PT_CORNER_BR][PT_UP] = 0.5;
	matrix[PT_CORNER_BR][PT_LEFT] = 0.5;

	matrix[PT_END][PT_DOWN] = 0.25;
	matrix[PT_END][PT_UP] = 0.25;
	matrix[PT_END][PT_LEFT] = 0.25;
	matrix[PT_END][PT_RIGHT] = 0.25;
}

void GameMap::generate() {
	generate(time(NULL));
}

void GameMap::generate(int seed) {
	srand(seed);
	std::cout << "Seed: " << seed << std::endl;
	add_path(&homeBase);
	
	double new_path_rate = 0.011;
	int minSize = 500;
	int numAreas = 0;
	while (floors.size() < minSize) {
		for (Floor* f: floors) {
			if (f->getFtype() == FT_BIG_AREA) {
				numAreas++;
			}
			if (f->getFtype() == FT_BIG_AREA && ((double)rand() / RAND_MAX) < new_path_rate) {
				add_path(f);
				numAreas = 0;
				if (paths.back()->getFloors().size() > 0) {
					break;
				}
				else {
					numAreas--;
				}
			}
		}
		if (paths.back()->getFloors().size() == 0 && numAreas == 0) {
			break;
		}
	}
	std::cout << "Generation Complete." << std::endl;
}

void GameMap::reset_invalid_path(bool* invalid_path, PathType lastPath) {
	for (int j = 0; j < PT_END; ++j) {
		*(invalid_path + j) = matrix[lastPath][j] == 0.0;
	}
}

bool GameMap::no_valid_direction(bool* invalid_paths) {
	for (int i = 0; i < PT_END; ++i) {
		if (!*(invalid_paths + i)) {
			return false;
		}
	}
	return true;
}

void GameMap::add_path(Floor* entry) {
	paths.push_back(new MapPath(textures, entry));
	MapPath* current_path = paths.back();
	printf("\n\nNew Path:\n");
	
	bool invalid_path[PT_END+1] = { false };
	reset_invalid_path(&invalid_path[0], current_path->lastPathType());
	int minLen = 50, maxLen = 200, size = 0;

	double randNum;
	double sum;

	PathType next_pType;
	Floor* f;

	while (minLen > size || next_pType != PT_END) {
		next_pType = next_path_type(current_path->lastPathType(), &randNum, &sum);
		
		if (next_pType == PT_END || size > maxLen || no_valid_direction(&invalid_path[0])) {
			// current_path->add_floor((PathType)i);
			if (size < minLen && !no_valid_direction(&invalid_path[0])) {
				continue;
			}
			std::cout << "Found end (and fixing)" << std::endl;
			current_path->fix_last_floor();
			return;
		}

		f = current_path->make_floor(next_pType);
		if (!invalid_path[next_pType] && valid_placement(f)) {
			floors.push_back(f);
			current_path->attach_floor(f);
			reset_invalid_path(&invalid_path[0], next_pType);
			std::cout << "--valid--" << std::endl;
			std::cout << "Adding: " << f << std::endl;
			size++;
		}
		else if (!invalid_path[next_pType]) {
			std::cout << " new invalid" << std::endl;
			invalid_path[next_pType] = true;
			delete f;
		}
		else {
			std::cout << " invalid" << std::endl;
			delete f;
		}
		

	}


	return;
	/*
	for (int i = 0; i < PT_END+1; ++i) {
		if (size >= maxLen) {
			current_path->fix_last_floor();
			return;
		}
		sum += matrix[current_path->lastPathType()][i];
		if (!invalid_path[i] && randNum < sum) {
			
			if (i == PT_END){
				// current_path->add_floor((PathType)i);
				if (size < minLen) {
					reset_markov_variables(&i, &sum, &randNum, &invalid_path[0], current_path->lastPathType());
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
			std::cout << "[ ";
			for (int j = 0; j < PT_END; ++j) {
				std::cout << invalid_path[j] << ", ";
			}

			invalid_path[PT_END - 1] = size < minLen;
			std::cout << invalid_path[PT_END] << " ]" << std::endl;
			reset_markov_variables(&i, &sum, &randNum, &invalid_path[0], current_path->lastPathType());
			std::cout << "New Rand 1: " << randNum << std::endl;
			for (int j = 0; j < PT_END-1; ++j) {
				if (!invalid_path[j]) {
					break;
				}
				if (!(j + 1 < PT_END-1)) {
					continue;
				}
			}
			
		}
		else if (i == PT_END && size < minLen) {
			//for (bool& invld_pth : invalid_path) {
			for (int j = 0; j < PT_END; ++j) {
				if (!invalid_path[j]) {
					reset_markov_variables(&i, &sum, &randNum, &invalid_path[0], current_path->lastPathType());
					std::cout << "\nNew Rand 2: " << randNum << std::endl;
					break;
				}
			}
			if (size > 0) {
				current_path->fix_last_floor();
			}
			std::cout << "No Valid Path Forward\n" << std::endl;
			
			return;
		}
	}
	*/
	//std::cout << "found way out of loop\nsum: " << sum << "\nsize: "<< size << "\n" << std::endl;
}

PathType GameMap::next_path_type(PathType last_pType, double* randNum, double* sum) {
	while (true) {
		*randNum = (double)rand() / RAND_MAX;
		*sum = 0.0;
		for (int i = 0; i < PT_END + 1; ++i) {
			*sum += matrix[(int)last_pType][i];
			if (*randNum < *sum) { // next in markov change
				return (PathType)i;
			}
		}
		std::cout << "Warning: No path Determined. Resetting." << std::endl;
	}
	
	std::cout << "Next Path Error" << std::endl;
}

bool GameMap::valid_placement(Floor* p_f) {
	SDL_Rect floorRec;
	int xmod = 0, ymod = 0;
	if (p_f->getPtype() == PT_DOWN) {
		ymod = 20;
	}
	else if (p_f->getPtype() == PT_UP) {
		ymod = -40;
	}
	else if (p_f->getPtype() == PT_AREA && paths.back()->lastPathType() == PT_UP) {
		ymod = -20;
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


MapPath::~MapPath() {
	for (Floor* f : floors) {
		delete f;
	}
}

Texture MapPath::fType_texture(FloorType fType){
	if (fType == FT_BIG_AREA){
		return T_BIG_OPENING;
	}
	else if (fType == FT_H_HALL){
		return T_HORIZONTAL_PATH;
	}
	else if (fType == FT_H_HALL_LEFT) {
		return T_HORIZONTAL_LEFT;
	}
	else if (fType == FT_H_HALL_RIGHT) {
		return T_HORIZONTAL_RIGHT;
	}
	else if (fType == FT_V_HALL){
		return T_VERTICAL_PATH;
	}
	else if (fType == FT_V_HALL_TOP) {
		return T_VERTICAL_UP;
	}
	else if (fType == FT_V_HALL_BOTTOM) {
		return T_VERTICAL_DOWN;
	}
	else if (fType == FT_TL_CORNER) {
		return T_TL_CORNER;
	}
	else if (fType == FT_TR_CORNER) {
		return T_TR_CORNER;
	}
	else if (fType == FT_BL_CORNER) {
		return T_BL_CORNER;
	}
	else if (fType == FT_BR_CORNER) {
		return T_BR_CORNER;
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
		case PT_CORNER_BL:
			return FT_BL_CORNER;
		case PT_CORNER_TR:
			return FT_TR_CORNER;
		case PT_CORNER_BR:
			return FT_BR_CORNER;
		case PT_CORNER_TL:
			return FT_TL_CORNER;
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
		case PT_CORNER_TL:
			std::cout << " TL ";
			*attachSide = AP_TOP;
			//lastFloor->set_fType(FT_V_HALL_TOP);
			break;
		case PT_CORNER_TR:
			std::cout << " TR ";
			*attachSide = AP_TOP;
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
		case PT_CORNER_BL:
			std::cout << " BL ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_LEFT;
			//lastFloor->set_fType(FT_V_HALL_BOTTOM);
			break;
		case PT_CORNER_BR:
			std::cout << " BR ";
			*attachSide = AP_BOTTOM;
			*attachPoint = AP_LEFT;
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
		case PT_CORNER_TL:
			std::cout << " TL ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_CORNER_BL:
			std::cout << " BL ";
			*attachSide = AP_LEFT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
			break;
		case PT_LEFT:
			std::cout << " LEFT ";
			*attachSide = AP_LEFT;
			//lastFloor->set_fType(FT_H_HALL_LEFT);
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
		case PT_CORNER_TR:
			std::cout << " TR ";
			*attachSide = AP_RIGHT;
			*attachPoint = AP_TOP;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_CORNER_BR:
			std::cout << " BR ";
			*attachSide = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
			break;
		case PT_RIGHT:
			std::cout << " RIGHT ";
			*attachSide = AP_RIGHT;
			//lastFloor->set_fType(FT_H_HALL_RIGHT);
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
	case PT_CORNER_TL:
		switch (pType) {
		case PT_DOWN:
			*attachSide = AP_BOTTOM;
			break;
		case PT_RIGHT:
			*attachSide = AP_RIGHT;
			*attachPoint = AP_TOP;
			break;
		};
		break;
	case PT_CORNER_TR:
		switch (pType) {
		case PT_DOWN:
			*attachSide = AP_BOTTOM;
			break;
		case PT_LEFT:
			*attachSide = AP_LEFT;
			break;
		};
		break;
	case PT_CORNER_BL:
		switch (pType) {
		case PT_UP:
			*attachSide = AP_TOP;
			break;
		case PT_RIGHT:
			*attachSide = AP_RIGHT;
			break;
		};
		break;
	case PT_CORNER_BR:
		switch (pType) {
		case PT_UP:
			*attachSide = AP_TOP;
			break;
		case PT_LEFT:
			*attachSide = AP_LEFT;
			break;
		};
		break;
	};
	if (*attachSide == AP_NULL) {
		printf("No AttachSide Found\n");
	}
}

Floor* MapPath::make_floor(PathType pType) {
	AttachmentPoint attachSide, attachPoint;
	// std::cout << "Last floor type: " << lastFloor->getType() << std::endl;
	getAttachmentPoint(pType, &attachSide, &attachPoint);

	Floor* f = new Floor(*(textures + fType_texture(pType_fType(pType))), pType_fType(pType), pType, attachSide, attachPoint, lastFloor);
	if (lastFloor->getPtype() == PT_UP and pType == PT_AREA) {
		f->move(0.0, 20.0);
	}
	
	return f;

}

void MapPath::fix_last_floor() {
	if (floors.size() == 0) {
		return;
	}
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