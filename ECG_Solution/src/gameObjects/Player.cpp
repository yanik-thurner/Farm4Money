#include "Player.h"
#include "Water.h"
Player::Player(physx::PxScene* gScene, physx::PxMaterial* pMaterial) : SceneObject(SceneObject::model_manager->get_model(ModelManager::PLAYER), glm::mat4(1.0f), glm::vec3()), inventory(Inventory())
{
	//set variables
	jumpheight = 0;
	jumpspeed = -0.5;
	lasth = 0;
	onGround = false;

	//create charactercontroller
	player = nullptr;
	manager = PxCreateControllerManager(*gScene);

	physx::PxCapsuleControllerDesc desc;
	desc.position = physx::PxExtendedVec3(0.0f, 1.0f, 0.0f);
	//desc.contactOffset = 0.05f;
	//desc.slopeLimit = 0.1f;
	//desc.stepOffset = 0.01f;
	desc.upDirection = physx::PxVec3(0, 1, 0);
	desc.material = pMaterial;
	desc.height = 1.0f;
	desc.radius = 0.3f;
	desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;

	player = manager->createController(desc);

	if (player == nullptr) throw std::exception();
}


Player::~Player()
{
	//throws an exception when closing, probably because all phyx stuff is already released at the end of the game loop
	//manager->release();
}

physx::PxExtendedVec3 Player::getPos()
{
	return player->getPosition();
}

bool Player::buy(std::unique_ptr<Item> item)
{
	int value = item->get_value();
	if (inventory.get_money() >= value) {
		bool bought = false;
		if (dynamic_cast<Crop*>(item.get()) || dynamic_cast<Seed*>(item.get()))
			bought = inventory.insert(std::move(item));
		else if (dynamic_cast<BackpackUpgrade*>(item.get()))
			bought = upgrade_backpack();
		else if (dynamic_cast<GraphicsCard*>(item.get()))
			bought = inventory.insert(std::move(item));

		if (bought) {
			inventory.addMoney(-value);
			return true;
		}
		return false;
	}
	else {
		return false;
	}

}

bool Player::upgrade_backpack()
{
	if (inventory.get_money() >= BackpackUpgrade::value) {
		int bought = inventory.upgrade_backpack();

		if (bought) {
			inventory.addMoney(-BackpackUpgrade::value);
			return true;
		}
	}
	return false;
}

void Player::update(double dt, glm::vec3 dir)
{
	return;
}

void Player::move(double dt, glm::vec3 dir)
{
	if(dir.x != 0 || dir.z != 0)
		fishing = nullptr;

	physx::PxVec3 disp = physx::PxVec3(dir.x, dir.y, dir.z);
	disp = disp * dt * 8;
	physx::PxControllerCollisionFlags collisionFlags = player->move(disp, 0.01f, dt, physx::PxControllerFilters(), NULL);
	if (collisionFlags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN)) {
		onGround = true;
	}
}

void Player::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, 0);
}

void Player::render(TextRender& r, int window_width, int window_height)
{
	

}


void Player::jump() {
	fishing = nullptr;

	//std::cout << player->getPosition().y - oldheight << std::endl;
	float h = player->getPosition().y;
	if (onGround) {
		jumpspeed = 7.0f;
		//jumpheight = 0;
		onGround = false;
	}
}

void Player::transform(glm::mat4 trans)
{
	current_model.setTransformMatrix(trans);
}

void Player::transform(glm::vec3 pos, PlayerCamera &camera)
{
	if (movehead == false) {
		lastrot = camera.getBeta();
		transform(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3((pos.x), (pos.y) + 0.1f, (pos.z))), glm::radians(lastrot + 180), glm::vec3(0, 1, 0)));
	}
	else {
		transform(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3((pos.x), (pos.y) + 0.1f, (pos.z))), glm::radians(lastrot + 180), glm::vec3(0, 1, 0)));

		float turn = camera.getBeta() - lastrot;
		if (turn > 45) turn = 45;
		if (turn < -45) turn = -45;
		current_model.movehead(glm::rotate(glm::mat4(1.0f), glm::radians(turn), glm::vec3(0, 1, 0)));
	}
}

void Player::resetModelMatrix()
{
	current_model.resetModelMatrix();
}

float Player::getJumpHeight(float dt)
{
	//std::cout << jumping << "   " << onGround << std::endl;
	float h = player->getPosition().y;
	if (lasth - player->getPosition().y > 0.0001) onGround = false;
	if (onGround) {
		jumpheight = 0;
		jumpspeed = -5;
	}
	jumpheight = jumpheight + jumpspeed * dt;
	jumpspeed = jumpspeed - 30.0f * dt;
	lasth = player->getPosition().y;
	return jumpheight;
}



void Player::apply_model()
{
}
