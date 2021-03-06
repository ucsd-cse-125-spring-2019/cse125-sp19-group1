#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>
#include "lodepng/lodepng.h"
#include "../../graphics/GraphicsClient/ItemModelType.h"

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

enum class ExitGroup {
	front = 1u,
	bathroom,
	vent,
};

enum MapColorCode {
	wall              = 0x334e64,  //This is a Wall - R: 51 G : 78 B : 100 (#334e64)
	itemSpawn         = 0xe2a9e6,  //Can Spawn Item - R : 226 G : 169 B : 230 (#e2a9e6)
	playerSpawn       = 0xeb2f71,  //Player Spawn - R : 235 G : 47 B : 113 (#eb2f71)
	playerTrap        = 0xdeff00,  //This is a Trap - R : 222 G : 255 B : 0 (#deff00)
	playerExit        = 0x824959,  //This is an Exit - R : 130 G : 73 B : 89 (#824959)
	frontDeposit      = 0xb605bd,  //This is a Deposit for Keys - R : 182 G : 5 B : 189 (#b605bd)
	bathroomDeposit   = 0x5e3ba5,  //This is a Deposit for Bathroom - R : 94 G : 59 B : 165 (#5e3ba5)
	ventDeposit       = 0x7e6e9e,  //This is a Deposit for Vent - R : 126 G : 110 B : 158 (#7e6e9e)
	table             = 0x875315,  //This is a Table - R : 135 G : 83 B : 21 (#875315)
	northRamp         = 0xd8d8d8,  //Ramp going higher UP - R : 216 G : 216 B : 216 (#d8d8d8)
	westRamp          = 0xadadad,  //Ramp going higher LEFT - R : 173 G : 173 B : 173 (#adadad)
	eastRamp          = 0x737373,  //Ramp going higher RIGHT - R : 115 G : 115 B : 115 (#737373)
	southRamp         = 0x3f3f3f,  //Ramp going higher DOWN - R : 63 G : 63 B : 63 (#3f3f3f)
	secondLevel       = 0xb3dcff,  //2nd level - R : 179 G : 220 B : 255 (#b3dcff)
	bathroomExit      = 0x831a53,  //bathroom window exit - R : 131 G : 26 B : 83 (#831a53)
	frontExit         = 0x9f788d,  //front exit - R : 159 G : 120 B : 141 (#9f788d)
	ventExit          = 0xae457e,  //vent exit - R : 174 G : 69 B : 126 (#ae457e)
	stove             = 0xe3b25d,  //stove(takes ~2 tiles, item) - R : 227 G : 178 B : 93 (#e3b25d)
	toilet            = 0xca8c1f,  //toilet(takes 1 tile, item) - R : 202 G : 140 B : 31 (#ca8c1f)
	chair             = 0x8d7c5d,  //chair(takes 1 tile, item) - R : 141 G : 124 B : 93 (#8d7c5d)
	wallWithPainting  = 0x193044,  //wall with painting(in wall spot) - R : 25 G : 48 B : 68 (#193044)
};

MapColorCode allColorCodes[] = {
	wall,
	itemSpawn,
	playerSpawn,
	playerTrap,
	playerExit,
	frontDeposit,
	bathroomDeposit,
	ventDeposit,
	table,
	northRamp,
	westRamp,
	eastRamp,
	southRamp,
	secondLevel,
	bathroomExit,
	frontExit,
	ventExit,
	stove,
	toilet,
	chair,
	wallWithPainting,
};

enum DirectionBitmask {
	westSide  = (0x1u << 3),  //1000 = left = 8
	northSide = (0x1u << 2),  //0100 = up = 4
	southSide = (0x1u << 1),  //0010 = down = 2
	eastSide  = (0x1u << 0),  //0001 = right = 1
	noSide    = 0,            //0000 = no wall = 0 
};

static const struct {
	ItemModelType modelType;
	MapColorCode colorCode;
} modelTypes[] = {
	{ItemModelType::keyDropFrontExit, frontDeposit},
	{ItemModelType::keyDropBathroom, bathroomDeposit},
	{ItemModelType::keyDropVent, ventDeposit},
	{ItemModelType::window, bathroomExit},
	{ItemModelType::door, frontExit},
	{ItemModelType::vent, ventExit},
	{ItemModelType::stove, stove},
	{ItemModelType::toilet, toilet},
	{ItemModelType::restaurantChair, chair},
	{ItemModelType::painting, wallWithPainting},
};

static const struct {
	int xOffset;              // where to look within a tile for this wall
	int yOffset;              // where to look within a tile for this wall
	DirectionBitmask mask;    // mask to set for this wall
	const char *name;         // name of this wall for error messages
} wallProperties[] = {
	{  0, -1, DirectionBitmask::northSide, "north" },
	{  1,  0, DirectionBitmask::eastSide , "east"  },
	{  0,  1, DirectionBitmask::southSide, "south" },
	{ -1,  0, DirectionBitmask::westSide , "west"  },
};

int linearDistance(unsigned color1, unsigned color2) {
	int redDist   = ((color1 >> 16) & 0xFF) - ((color2 >> 16) & 0xFF);
	int greenDist = ((color1 >> 8 ) & 0xFF) - ((color2 >> 8 ) & 0xFF);
	int blueDist  = ((color1      ) & 0xFF) - ((color2      ) & 0xFF);

	return abs(redDist) + abs(greenDist) + abs(blueDist);
}

bool decodePixel(MapColorCode &colorCode, const unsigned char *pixelBytes) {
	unsigned pixel = ((unsigned)pixelBytes[0] << 16) | ((unsigned)pixelBytes[1] << 8) | pixelBytes[2];

	for (auto color : allColorCodes) {
		if (linearDistance(color, pixel) <= 1) {
			colorCode = color;
			return true;
		}
	}

	return false;
}

#if !defined(_WIN32) && !defined(WIN32) && !defined(fopen_s)
errno_t fopen_s(FILE **openedFile, const char *name, const char *mode) {
	if (!openedFile || !name || !mode) return EINVAL;
	FILE *f = fopen(name, mode);
	*openedFile = f;
	return f ? 0 : errno;
}
#endif

// filename must be preceeded by a path separator character
static int writeFile(string &folderName, const char *filename, vector<uint8_t> &map, unsigned mapWidth, unsigned mapHeight) {
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

	int count = 0;
	for (auto elem : map) {
		count += (elem != 0);
	}
	return count;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "Usage: create_map filename.png new_folder_name" << endl;
		return EXIT_FAILURE;
	}

	const char *pngFilename = argv[1];
	string folderName = argv[2];

	cout << "Opening PNG: " << pngFilename << endl;
	cout << "Writing to folder: " << folderName << endl;

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
	vector<uint8_t> envObjects;
	vector<uint8_t> playerExits;
	vector<uint8_t> keyDeposits;
	
	walls.resize(mapWidth * mapHeight);
	heights.resize(mapWidth * mapHeight);
	rampDirections.resize(mapWidth * mapHeight);
	envObjects.resize(mapWidth * mapHeight);
	playerExits.resize(mapWidth * mapHeight);
	keyDeposits.resize(mapWidth * mapHeight);

	static struct {
		MapColorCode colorCode;
		vector<uint8_t> map;
		const char *filename;
	} booleanMaps[] = {
		{ itemSpawn,    vector<uint8_t>(mapWidth * mapHeight, 0), "/item_spawn.txt" },
		{ playerSpawn,  vector<uint8_t>(mapWidth * mapHeight, 0), "/player_spawn.txt" },
		{ playerTrap,   vector<uint8_t>(mapWidth * mapHeight, 0), "/player_trap.txt" },
		//{ playerExit,   vector<uint8_t>(mapWidth * mapHeight, 0), "/player_exit.txt" },
		//{ frontDeposit, vector<uint8_t>(mapWidth * mapHeight, 0), "/key_deposit.txt" },  /* deprecated */
		{ table,        vector<uint8_t>(mapWidth * mapHeight, 0), "/table.txt" },        /* deprecated */
	};

	// Loop over tiles where (mapX, mapY) is the map position and
	// (x, y) is the pixel position of the center of the tile
	unsigned mapX = 0, mapY = 0;
	for (unsigned y = MARGIN_TOP; y < height - MARGIN_BOTTOM; y += TILE_WIDTH) {
		mapX = 0;
		for (unsigned x = MARGIN_LEFT; x < width - MARGIN_RIGHT; x += TILE_WIDTH) {
			const size_t mapOffset = mapY * mapWidth + mapX;
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
							<< " wall position is not wall colored (#" << hex 
							<< colorCode << dec << ") at ("
							<< pixelX << ", " << pixelY << ")" << endl;
					}

					sides |= prop.mask;
				}
			}

			walls[mapOffset] = sides;
			
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
					case bathroomDeposit:
						keyDeposits[mapOffset] = static_cast<uint8_t>(ExitGroup::bathroom);
						goto defaultColorCodeCase;
					case frontDeposit:
						keyDeposits[mapOffset] = static_cast<uint8_t>(ExitGroup::front);
						goto defaultColorCodeCase;
					case ventDeposit:
						keyDeposits[mapOffset] = static_cast<uint8_t>(ExitGroup::vent);
						goto defaultColorCodeCase;
					case bathroomExit:
						playerExits[mapOffset] = static_cast<uint8_t>(ExitGroup::bathroom);
						goto defaultColorCodeCase;
					case frontExit:
						playerExits[mapOffset] = static_cast<uint8_t>(ExitGroup::front);
						goto defaultColorCodeCase;
					case ventExit:
						playerExits[mapOffset] = static_cast<uint8_t>(ExitGroup::vent);
						goto defaultColorCodeCase;
					defaultColorCodeCase:
					default:
						bool found = false;
						for (auto &boolMap : booleanMaps) {
							if (colorCode == boolMap.colorCode) {
								boolMap.map[mapOffset] = 1;
								found = true;
								break;
							}
						}
						if (found) break;

						for (auto &m : modelTypes) {
							if (colorCode == m.colorCode) {
								if (m.modelType == ItemModelType::painting) {
									printf("Suppressed painting at (%d, %d)\n", mapX, mapY);
								}
								else {
									envObjects[mapOffset] = static_cast<uint8_t>(m.modelType);
									found = true;
								}
								break;
							}
						}

						break;
					}
				}

			}

			rampDirections[mapOffset] = rampDirection;
			heights[mapOffset] = height;

			mapX++;
		}
		mapY++;
	}

	int injectX = floorf(189.535995f / 20);
	int injectZ = floorf(167.119995f / 20);
	envObjects[injectZ * mapWidth + injectX] = static_cast<uint8_t>(ItemModelType::painting);
	printf("Injected painting at %d, %d\n", injectX, injectZ);

	MY_MKDIR(folderName.c_str());

	writeFile(folderName, "/walls.txt", walls, mapWidth, mapHeight);
	writeFile(folderName, "/heights.txt", heights, mapWidth, mapHeight);
	writeFile(folderName, "/ramps.txt", rampDirections, mapWidth, mapHeight);
	writeFile(folderName, "/env_objs.txt", envObjects, mapWidth, mapHeight);
	writeFile(folderName, "/player_exit.txt", playerExits, mapWidth, mapHeight);
	writeFile(folderName, "/key_deposit.txt", keyDeposits, mapWidth, mapHeight);

	for (auto &boolMap : booleanMaps) {
		int count = writeFile(folderName, boolMap.filename, boolMap.map, mapWidth, mapHeight);
		if (count <= 0) {
			cout << "WARNING: " << (boolMap.filename + 1) << " is all zeros" << endl;

			unsigned needle[3] = { 
				(boolMap.colorCode >> 16) & 0xFFu, 
				(boolMap.colorCode >> 8) & 0xFFu, 
				boolMap.colorCode & 0xFFu
			};
			int pixCount = 0;
			for (size_t i = 0; i < image.size(); i += 4) {
				unsigned haystack[3] = {image[i], image[i+1], image[i+2]};

				int diff = 0;
				for (int x = 0; x < 3; x++) {
					diff += (needle[x] - haystack[x]) * (needle[x] - haystack[x]);
				}
				
				if (sqrt(diff) < 3) {
					++pixCount;
					printf("\t%02X%02X%02X\n", haystack[0], haystack[1], haystack[2]);
				}
			}
			cout << "\tFound " << pixCount << " pixels\n";
		}

	}

	return EXIT_SUCCESS;
}
