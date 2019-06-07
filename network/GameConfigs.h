#pragma once
#ifndef GAME_CONFIGS_H
#define GAME_CONFIGS_H

//logs
#define PRINT_DEBUG_STATEMENTS

#ifdef PRINT_DEBUG_STATEMENTS
#endif

// Minimap
#define MINIMAP_PING_INTERVAL_NO_ANGER 15.0
#define MINIMAP_PING_INTERVAL_FULL_ANGER 3.0

// Player data
#define CHEF_SPAWN_ROW 0
#define CHEF_SPAWN_COL 1
#define SWING_RADIUS 10
#define VISION_RADIUS 85.0f
#define ITEM_SLOWDOWN_MULTIPLIER 0.8
#define CAKE_SLOWDOWN_MULTIPLIER 0.7
#define CHEF_ANGER_INTERVAL 4
#define CHEF_MAX_ANGER 60
#define CHEF_CATCH_RADIUS 20
#define CHEF_MAX_VISION 100
#define BEAR_BUFF_CHEF_ANGER_BOOST 10
#define BEAR_BUFF_PLAYER_SPEED_BOOST 0.15
#define ABILITY_CHARGE_TIME 0.0
#define GHOST_MULTIPLIER 1.6
#define MAX_ANIMAL_GHOST_TIME 5
#define MAX_ANIMAL_SEARCH_TIME 5
#define FLASH_DISTANCE 14
#define DEFAULT_VISION 60
#define DEFAULT_CHEF_VISION_MULT 1
#define LIMIT_CHEF_VISION_MULT 0.4
#define LIMIT_CHEF_SLOW_MULT 0.8
#define DEFAULT_SPEED_MULTIPLIER 1.3
#define DEFAULT_CHEF_SPEED_MULTIPLIER 1.3
#define PLAYER_DASH_MULTIPLIER 3
#define MAX_PLAYER_DASH_TIME 0.1

// Tile data
#define TILE_SIZE 20
#define TILE_HEIGHT 10

// Collision checking data
#define PLAYER_RADIUS 2
#define WALL_SIZE 2

// Item data
#define MAX_ITEMS 14
#define ITEM_DROP_DURATION 10
#define POWERUP_DROP_CHANCE 1 // drop chance is 1 divided by the value shown here
#define TIME_TO_RESPAWN_BOX 10

// Player interact data
#define DISTANCE_TO_CONSTRUCT_GATE 10
#define TIME_TO_OPEN_BOX 0.75 // in seconds
#define TIME_TO_CONSTRUCT_GATE 10
#define HITS_TO_OPEN_JAIL 3
// DELAY = time to wait before you can act again
#define SWING_NET_DELAY 0.5
#define UNLOCK_JAIL_DELAY 0.25
#define KEY_DROP_DELAY 0.25
#define DESTROY_POWERUP_DELAY 0.25
// COOLDOWN = time to wait before you can do this action again
#define UNLOCK_JAIL_COOLDOWN 0.5
#define PLAYER_DASH_COOLDOWN 5
#define MAX_CHEF_TIME 2.5


#endif