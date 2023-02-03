#pragma once
#include "SceneObject.h"
#include "PhysX\PxPhysicsAPI.h"
#include <glm\glm.hpp>
#include <exception>
#include "..\PlayerCamera.h"
#include <cmath>
#include "..\Model.h"
#include "Inventory.h"
#include "plants\Crop.h"
#include "plants\Seed.h"
#include "BackpackUpgrade.h"
#include "GraphicsCard.h"


class Water;
class Player : public SceneObject
{
private:
	physx::PxController *player;
	physx::PxControllerManager* manager;
	float jumpheight;
	float jumpspeed;
	float lasth;
	float lastrot = 0;
	bool onGround;

	// general settings
	int char_length = 29;
	int char_height = 34;

	// text settings
	int fishing_text_scale = 1;
	std::string fishing_text = "Fishing...";
	int fishing_text_y_offset = 20;

	// colors
	glm::vec3 text_color = glm::vec3(0, 0, 0);

protected:
	virtual void apply_model() override;
public:
	Player(physx::PxScene* gScene, physx::PxMaterial* pMaterial);
	~Player();
	physx::PxExtendedVec3 getPos();
	float getJumpHeight(float dt);
	void jump();
	void move(double dt, glm::vec3 dir);
	void transform(glm::mat4 trans);
	void transform(glm::vec3 pos, PlayerCamera &camera);
	void resetModelMatrix();

	Inventory inventory;

	bool movehead = false;
	bool buy(std::unique_ptr<Item> item);
	bool upgrade_backpack();
	Water *fishing = nullptr;

	// Inherited via SceneObject
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;

	void render(TextRender& r, int window_width, int window_height);
	
};

