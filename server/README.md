# Data required for the game

## Player
- id?
- type (chef/animal)
- model: chef/racoon/cat/dog
- location (x, y, z)
- bounding box size: (xdim, ydim, zdim)
- inventory: held item/empty
	- for chef, this is the captured animal
	- for animals, this is the key/cake
- status: captured/notcaptured (N/A for chef)


## General 
- game time (used for chef rage bar)
- key locations :
	- key 1: location(x, y, z)
	- key 2: location(x, y, z)
	- ...
- key bounding box size: (xdim, ydim, zdim)
- cake location: location(x, y, z)
- cake bounding box size: (xdim, ydim, zdim)
- exit locations:
	- exit 1: location(x, y, z)
	- exit 2: location(x, y, z)
	- ...
- exit bounding box size: (xdim, ydim, zdim)
- exit statuses:
	- exit 1:
		- key 1: found/notfound
		- key 2: found/notfound
		- key 3: found/notfound
	- exit 2:
		- ...

- captured animals:
	- animal 1
	- ...

## Data Format
For game data sent from server to client:

client: 1 \n  
Type: animal \n  
Class: cat \n  
Location: x y z \n  
(etc...)  
client: 2 \n  
Type: animal \n  
Class: cat \n  
(etc...)  
client: -10 \n  
Exit1 status: true false true \n                  (true means that corresponding key # is found)  
key locations: key1: x y z || key2: x y z || ...  \n  
