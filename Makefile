ALL:
	g++ -I src/include -L src/lib -o DeadMansTreasure main.cpp engine.cpp renderwindow.cpp entity.cpp math.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image