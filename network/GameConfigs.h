#pragma once
#ifndef GAME_CONFIGS_H
#define GAME_CONFIGS_H

// Player data
#define SWING_RADIUS 10
#define VISION_RADIUS 85.0f

// Tile data
#define TILE_SIZE 20
#define TILE_HEIGHT 10

// Collision checking data
#define PLAYER_RADIUS 2
#define WALL_SIZE 2

// Item data
#define MAX_ITEMS 14
#define ITEM_DROP_DURATION 10
#define POWERUP_DROP_CHANCE 4 // drop chance is 1 divided by the value shown here
#define TIME_TO_RESPAWN_BOX 10

// Player interact data
#define TIME_TO_OPEN_BOX 2 // in seconds
#define TIME_TO_CONSTRUCT_GATE 30
// DELAY = time to wait before you can act again
#define SWING_NET_DELAY 0.5
#define UNLOCK_JAIL_DELAY 0.25
// COOLDOWN = time to wait before you can do this action again
#define UNLOCK_JAIL_COOLDOWN 1.5 

#endif