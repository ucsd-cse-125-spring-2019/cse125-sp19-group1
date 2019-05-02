#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }
Player::Player(int anID) : playerID(anID), inventory(ItemName::EMPTY), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	location = Location();
	addDecodeFunctions();
	if (anID % 2 == 1) {
		isChef = true;
		modelType = ModelType::CHEF;
	}
}

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory(ItemName::EMPTY), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	addDecodeFunctions();
	if (anID % 2 == 1) {
		isChef = true;
		modelType = ModelType::CHEF;
	}
}

Location Player::getLocation() { return location; }
ItemName Player::getInventory() { return inventory; }
bool Player::getInteracting() { return interacting; }

void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
}

void Player::setInventory(ItemName anItem)
{
	inventory = anItem;
}

void Player::setInteracting() {
	interacting = !interacting;
}

ModelType Player::getModelType() {
	return modelType;
}

bool Player::getIsChef() {
	return isChef;
}

void Player::setCaughtAnimal() {
	caughtAnimal = !caughtAnimal;
}

void Player::setIsCaught() {
	isCaught = !isCaught;
}

bool Player::getCaughtAnimal() {
	return caughtAnimal;
}

bool Player::getIsCaught() {
	return isCaught;
}

bool Player::inRange(std::vector<float> & myLoc, std::vector<float> & theirLoc) {
	double dist = sqrt(pow(myLoc[0] - theirLoc[0], 2) +
		pow(myLoc[1] - theirLoc[1], 2) + 
		pow(myLoc[2] - theirLoc[2], 2) * 1.0);
	if (dist < radius) {
		return true;
	}
	return false;
}

void Player::setStartTime() {
	start = std::chrono::system_clock::now();
}

double Player::checkProgress() {
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - start;
	return elapsed_seconds.count();
}

std::string Player::encodePlayerData()
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;
	encodedData << "isChef: " << isChef << std::endl;
	encodedData << "model: " << static_cast<int>(modelType) << std::endl;
	encodedData << "inventory: " << static_cast<int>(inventory) << std::endl;
	encodedData << "hasCake: " << hasCake << std::endl;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;

	return encodedData.str();
}

void Player::decodePlayerData(std::string key, std::string value)
{
	// Safety check in case decoding function for the key doesn't exist
	if (decodingFunctions.count(key) > 0)
		(this->*decodingFunctions[key])(value); // Format for calling the functions from the map
	else
		std::cout << "No decoding function for key: " << key << std::endl;
}

void Player::decodeLocation(std::string value)
{
	std::stringstream valueStream(value);
	std::string x, y, z;
	valueStream >> x >> y >> z;
	setLocation(std::stof(x), std::stof(y), std::stof(z));
}

void Player::decodeInventory(std::string value)
{
	inventory = static_cast<ItemName>(std::stoi(value));
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
