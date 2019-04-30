#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <string>
#include "lodepng/lodepng.h"

// For mkdir()
#ifdef _WIN32
#include <direct.h>
#define MY_MKDIR(x) _mkdir(x)
#else
#include <sys/stat.h>
#define MY_MKDIR(x) mkdir(x, 0777)
#endif

using namespace std;

// The width/height of a square tile measured in pixels:
#define TILE_WIDTH 3

// The amount of space within a tile around the center pixel:
#define MARGIN_LEFT 1
#define MARGIN_RIGHT 1
#define MARGIN_TOP 1
#define MARGIN_BOTTOM 1

enum MapColorCode {
	wall        = 0x334e65,  //This is a Wall - R: 51 G : 78 B : 101 (#334e65)
	itemSpawn   = 0xe2a9e6,  //Can Spawn Item - R : 226 G : 169 B : 230 (#e2a9e6)
	playerSpawn = 0xeb2f71,  //Player Spawn - R : 235 G : 47 B : 113 (#eb2f71)
	playerTrap  = 0xdeff00,  //This is a Trap - R : 222 G : 255 B : 0 (#deff00)
	playerExit  = 0x824959,  //This is an Exit - R : 130 G : 73 B : 89 (#824959)
	keyDeposit  = 0xb605bd,  //This is a Deposit for Keys - R : 182 G : 5 B : 189 (#b605bd)
	table       = 0x875315,  //This is a Table - R : 135 G : 83 B : 21 (#875315)
	northRamp   = 0xd8d8d8,  //Ramp going higher UP - R : 216 G : 216 B : 216 (#d8d8d8)
	westRamp    = 0xadadad,  //Ramp going higher LEFT - R : 173 G : 173 B : 173 (#adadad)
	eastRamp    = 0x737373,  //Ramp going higher RIGHT - R : 115 G : 115 B : 115 (#737373)
	southRamp   = 0x3f3f3f,  //Ramp going higher DOWN - R : 63 G : 63 B : 63 (#3f3f3f)
	secondLevel = 0xb3dcff   //2nd level - R : 179 G : 220 B : 255 (#b3dcff)
};

enum DirectionBitmask {
	westSide  = (0x1u << 3),  //1000 = left = 8
	northSide = (0x1u << 2),  //0100 = up = 4
	southSide = (0x1u << 1),  //0010 = down = 2
	eastSide  = (0x1u << 0),  //0001 = right = 1
	noSide    = 0,            //0000 = no wall = 0 
};

static const struct {
	int xOffset;              // where to look within a tile for this wall
	int yOffset;              // where to look within a tile for this wall
	DirectionBitmask mask;    // mask to set for this wall
	const char *name;         // name of this wall for error messages
} wallProperties[] = {
	{ 0, -1, DirectionBitmask::northSide, "north" },
	{ 1, 0, DirectionBitmask::eastSide, "east" },
	{ 0, 1, DirectionBitmask::southSide, "south" },
	{ -1, 0, DirectionBitmask::westSide, "west" },
};

bool decodePixel(MapColorCode &colorCode, const unsigned char *pixelBytes) {
	unsigned pixel = ((unsigned)pixelBytes[0] << 16) | ((unsigned)pixelBytes[1] << 8) | pixelBytes[2];

	switch ((MapColorCode)pixel) {
	case wall:
	case itemSpawn:
	case playerSpawn:
	case playerTrap:
	case playerExit:
	case keyDeposit:
	case table:
	case northRamp:
	case westRamp:
	case eastRamp:
	case southRamp:
	case secondLevel:
		colorCode = (MapColorCode)pixel;
		return true;
	default:
		return false;
	}
}

// filename must be preceeded by a path separator character
static void writeFile(string &folderName, const char *filename, vector<uint8_t> &map, unsigned mapWidth, unsigned mapHeight) {
	FILE *file = nullptr;
	string fullpath = folderName + filename;
	if (fopen_s(&file, fullpath.c_str(), "w")) {
		cout << "Error opening " << fullpath << endl;
		exit(EXIT_FAILURE);
	}
	fprintf(file, "%d %d\n", mapWidth, mapHeight);
	int i = 0;
	for (unsigned y = 0; y < mapHeight; y++) {
		for (unsigned x = 0; x < mapWidth; x++) {
			fprintf(file, "%d ", map[i]);
			i++;
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "Usage: create_map filename.png new_folder_name" << endl;
		return EXIT_FAILURE;
	}

	const char *pngFilename = argv[1];
	string folderName = argv[2];

	vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, pngFilename);

	//if there's an error, display it
	if (error) {
		cout << "Decoder error " << error << ": " << lodepng_error_text(error) << endl;
		return EXIT_FAILURE;
	}

	if (width < TILE_WIDTH) {
		cout << "Invalid width " << width << endl;
		return EXIT_FAILURE;
	}
	if (height < TILE_WIDTH) {
		cout << "Invalid height " << height << endl;
		return EXIT_FAILURE;
	}

	if ((width % TILE_WIDTH) != 0) {
		cout << "WARNING: width " << width << " not a multiple of " << TILE_WIDTH << endl;
	}
	if ((height % TILE_WIDTH) != 0) {
		cout << "WARNING: height " << height << " not a multiple of " << TILE_WIDTH << endl;
	}

	unsigned counts[TILE_WIDTH][TILE_WIDTH] = {{0}};

	unsigned mapWidth = width / TILE_WIDTH;
	unsigned mapHeight = height / TILE_WIDTH;

	vector<uint8_t> walls;
	vector<uint8_t> heights;
	vector<uint8_t> rampDirections;
	
	walls.resize(mapWidth * mapHeight);
	heights.resize(mapWidth * mapHeight);
	rampDirections.resize(mapWidth * mapHeight);

	static struct {
		MapColorCode colorCode;
		vector<uint8_t> map;
		const char *filename;
	} booleanMaps[] = {
		{ itemSpawn,   vector<uint8_t>(mapWidth * mapHeight, 0), "/item_spawn.txt" },
		{ playerSpawn, vector<uint8_t>(mapWidth * mapHeight, 0), "/player_spawn.txt" },
		{ playerTrap,  vector<uint8_t>(mapWidth * mapHeight, 0), "/player_trap.txt" },
		{ playerExit,  vector<uint8_t>(mapWidth * mapHeight, 0), "/player_exit.txt" },
		{ keyDeposit,  vector<uint8_t>(mapWidth * mapHeight, 0), "/key_deposit.txt" },
		{ table,       vector<uint8_t>(mapWidth * mapHeight, 0), "/table.txt" },
	};

	// Loop over tiles where (mapX, mapY) is the map position and
	// (x, y) is the pixel position of the center of the tile
	unsigned mapX = 0, mapY = 0;
	for (unsigned y = MARGIN_TOP; y < height - MARGIN_BOTTOM; y += TILE_WIDTH) {
		mapX = 0;
		for (unsigned x = MARGIN_LEFT; x < width - MARGIN_RIGHT; x += TILE_WIDTH) {
			MapColorCode colorCode;

			uint8_t sides = DirectionBitmask::noSide;

			for (const auto &prop : wallProperties) {
				// x and y are tile center
				unsigned pixelX = x + prop.xOffset;
				unsigned pixelY = y + prop.yOffset;
				size_t offset = (pixelY * width + pixelX) * 4;

				int sum = (int)image[offset] + image[offset + 1] + image[offset + 2];

				if (sum < 0xFF * 3 - 8) {
					if (!decodePixel(colorCode, &image[offset]) || colorCode != MapColorCode::wall) {
						cout << "WARNING: pixel in " << prop.name
							<< " wall position is not wall colored at ("
							<< pixelX << ", " << pixelY << ")" << endl;
					}

					sides |= prop.mask;
				}
			}

			walls[mapY * mapWidth + mapX] = sides;
			
			DirectionBitmask rampDirection = DirectionBitmask::noSide;
			uint8_t height = 0;

			for (unsigned pixelY = y - MARGIN_TOP; 
				pixelY < y - MARGIN_TOP + TILE_WIDTH; 
				pixelY++)
			{
				for (unsigned pixelX = x - MARGIN_LEFT;
					pixelX < x - MARGIN_TOP + TILE_WIDTH;
					pixelX++)
				{
					size_t offset = (pixelY * width + pixelX) * 4;
					if (!decodePixel(colorCode, &image[offset])) continue;

					counts[pixelY + MARGIN_TOP - y][pixelX + MARGIN_LEFT - x]++;

					switch (colorCode) {
					case secondLevel:
						height = 2;
						break;
					case northRamp:
						rampDirection = DirectionBitmask::northSide;
						height = 1;
						break;
					case eastRamp:
						rampDirection = DirectionBitmask::eastSide;
						height = 1;
						break;
					case southRamp:
						rampDirection = DirectionBitmask::southSide;
						height = 1;
						break;
					case westRamp:
						rampDirection = DirectionBitmask::westSide;
						height = 1;
						break;
					default:
						for (auto &boolMap : booleanMaps) {
							if (colorCode == boolMap.colorCode) {
								boolMap.map[mapY * mapWidth + mapX] = 1;
							}
						}
						break;
					}
				}

			}

			rampDirections[mapY * mapWidth + mapX] = rampDirection;
			heights[mapY * mapWidth + mapX] = height;

			mapX++;
		}
		mapY++;
	}

	MY_MKDIR(folderName.c_str());

	writeFile(folderName, "/walls.txt", walls, mapWidth, mapHeight);
	writeFile(folderName, "/heights.txt", heights, mapWidth, mapHeight);
	writeFile(folderName, "/ramps.txt", rampDirections, mapWidth, mapHeight);

	for (auto &boolMap : booleanMaps) {
		writeFile(folderName, boolMap.filename, boolMap.map, mapWidth, mapHeight);
	}

	return EXIT_SUCCESS;
}