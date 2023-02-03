#include "SellBox.h"

physx::PxShape *SellBox::shape;
SellBox::SellBox(glm::vec3 size, glm::vec3 position) : Interactable(SceneObject::model_manager->get_model(ModelManager::ModelID::SELLBOX), glm::mat4(1.0f), position), Collidable(current_model, position)
{
	if (!shape) shape = generate_shape();
	set_actor(generate_actor(shape));

	physx::PxMaterial* gMaterial = SceneObject::gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* triggerShape = SceneObject::gPhysicsSDK->createShape(physx::PxBoxGeometry((size.x / 2)+0.5, (size.y / 2)+0.5, (size.z / 2)+0.5), *gMaterial, true);

	triggerShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

	actor->attachShape(*triggerShape);
	actor->userData = static_cast<void*>(this);

	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * 2, position.y * 2, position.z * 2));
	current_model.transform(transform_matrix);
}


SellBox::~SellBox()
{
}

int SellBox::sell(std::unique_ptr<Item> i)
{
	int price = i->get_value();
	i.reset();
	return price;
}

void SellBox::apply_model()
{
}

void SellBox::update(double timediff, glm::vec3 dir)
{
}

void SellBox::draw(glm::vec3 cam_pos, float view_frustum){
	current_model.draw(cam_pos, view_frustum);
}

void SellBox::interact(Player &p)
{
	if (p.inventory.is_selected_item_valid()) {
		std::unique_ptr<Item> item = p.inventory.remove_selected();
		p.inventory.addMoney(sell(std::move(item)));
	}
}
