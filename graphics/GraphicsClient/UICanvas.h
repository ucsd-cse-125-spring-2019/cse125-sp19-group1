
#include "UIObject.h"

#define NUM_ITEMS 20
#define CANVAS_PATH    "../Canvas/"

#define CANVAS_MDL_PATH		(CANVAS_PATH "canvas.fbx")
#define UI_BASE_TEX (CANVAS_PATH "base.ppm")
#define RACCOON_HAPPY_TEX (CANVAS_PATH "raccoonHappy.ppm")
#define CAT_HAPPY_TEX (CANVAS_PATH "catHappy.ppm")
#define DOG_HAPPY_TEX (CANVAS_PATH "dogHappy.ppm")
#define RACCOON_JAIL_TEX (CANVAS_PATH "raccoonJail.ppm")
#define CAT_JAIL_TEX (CANVAS_PATH "catJail.ppm")
#define DOG_JAIL_TEX (CANVAS_PATH "dogJail.ppm")
#define RACCOON_ITEM_TEX (CANVAS_PATH "raccoonItem.ppm")
#define CAT_ITEM_TEX (CANVAS_PATH "catItem.ppm")
#define DOG_ITEM_TEX (CANVAS_PATH "dogItem.ppm")
#define CAKE_ITEM_TEX (CANVAS_PATH "cakeItem.ppm")
#define YELLOW_KEY_ITEM_TEX (CANVAS_PATH "yellowKeyItem.ppm")
#define BLUE_KEY_ITEM_TEX (CANVAS_PATH "blueKeyItem.ppm")
#define GREEN_KEY_ITEM_TEX (CANVAS_PATH "greenKeyItem.ppm")
#define PLUNGER_ITEM_TEX (CANVAS_PATH "plungerItem.ppm")
#define ROPE_ITEM_TEX (CANVAS_PATH "ropeItem.ppm")
#define TOILET_PAPER_ITEM_TEX (CANVAS_PATH "toiletPaperItem.ppm")
#define RED_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "redScrewdriverItem.ppm")
#define GREEN_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "greenScrewdriverItem.ppm")
#define YELLOW_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "yellowScrewdriverItem.ppm")

#ifndef UICANVAS_H
#define UICANVAS_H
class UICanvas {
public:
	enum UIType {
		UI_BASE = 0,
		RACCOON_HAPPY = 1,
		CAT_HAPPY = 2,
		DOG_HAPPY = 3,
		RACCOON_JAIL = 4,
		CAT_JAIL = 5,
		DOG_JAIL = 6,
		RACCOON_ITEM = 7,
		CAT_ITEM = 8,
		DOG_ITEM = 9,
		CAKE_ITEM = 10,
		YELLOW_KEY_ITEM = 11,
		BLUE_KEY_ITEM = 12,
		GREEN_KEY_ITEM = 13,
		PLUNGER_ITEM = 14,
		ROPE_ITEM = 15,
		TOILET_PAPER_ITEM = 16,
		RED_SCREWDRIVER_ITEM = 17,
		GREEN_SCREWDRIVER_ITEM = 18,
		YELLOW_SCREWDRIVER_ITEM = 19,
	};
	void draw(glm::mat4 * V, glm::mat4 * P, glm::mat4 model);
	void setAngerRatio(float newAngerRatio);
	void setVisible(UIType item, bool visible);
	UICanvas(GLuint shaderProgram);
	~UICanvas();
private:
	bool visibleItems[NUM_ITEMS];
	class UIObject *uiItems[NUM_ITEMS];
	GLuint shaderProgram;
	float angerRatio;
};

#endif
