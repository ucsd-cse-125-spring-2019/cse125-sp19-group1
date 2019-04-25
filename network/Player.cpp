#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }
Player::Player(int anID) : playerID(anID), inventory("empty"), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	location = Location();
	addDecodeFunctions();
}

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory("empty"), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	addDecodeFunctions();
}

Location Player::getLocation() { return location; }

void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
}

std::string Player::encodePlayerData()
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;
	encodedData << "isChef: " << isChef << std::endl;
	encodedData << "model: " << static_cast<int>(modelType) << std::endl;
	encodedData << "inventory: " << inventory << std::endl;
	encodedData << "hasCake: " << hasCake << std::endl;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;

	return encodedData.str();
}

void Player::decodePlayerData(std::string key, std::string value)
{
	//Player * tmp;
	//decodeFunctionType val = decodingFunctions[key];// (value);
	(this->*decodingFunctions[key])(value);
}

void Player::decodeLocation(std::string value)
{
	std::cout << value << std::endl;
	std::stringstream valueStream(value);
	std::string x, y, z;
	valueStream >> x >> y >> z;
	setLocation(std::stof(x), std::stof(y), std::stof(z));
}

void Player::decodeInventory(std::string value)
{
	inventory = value;
}
void Player::decodeCakeStatus(std::string value)
{
	hasCake = std::stoi(value) == 1;
}
void Player::decodeChefStatus(std::string value)
{
	isChef = std::stoi(value) == 1;
}
void Player::decodeModelType(std::string value)
{
	modelType = static_cast<ModelType>(std::stoi(value));
}

void Player::addDecodeFunctions()
{
	decodingFunctions["location"] = &Player::decodeLocation;
	decodingFunctions["isChef"] = &Player::decodeChefStatus;
	decodingFunctions["model"] = &Player::decodeModelType;
	decodingFunctions["hasCake"] = &Player::decodeCakeStatus;
	decodingFunctions["inventory"] = &Player::decodeInventory;
}
