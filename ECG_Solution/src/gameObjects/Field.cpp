#include "Field.h"
#include "plants\Seed.h"
#include "plants\CornSeed.h"
#include "plants\CornPlant.h"
#include "plants\CarrotSeed.h"
#include "plants\CarrotPlant.h"
#include "plants\MelonSeed.h"
#include "plants\MelonPlant.h"
#include "plants\TomatoSeed.h"
#include "plants\TomatoPlant.h"
#include "plants\Crop.h"
#include "plants\SaleSign.h"

physx::PxShape *Field::shape;
Field::Field(glm::vec3 size, glm::vec3 position) : Interactable(SceneObject::model_manager->get_model(ModelManager::ModelID::FIELD), glm::mat4(1.0f), position), pos(position), Collidable(current_model, position)
{
	if (!shape) shape = generate_shape();
	set_actor(generate_actor(shape));

	physx::PxMaterial* gMaterial = SceneObject::gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* triggerShape = SceneObject::gPhysicsSDK->createShape(physx::PxBoxGeometry((size.x / 2), (size.y / 2) + 1, (size.z / 2)), *gMaterial, true);

	triggerShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

	actor->attachShape(*triggerShape);

	actor->userData = static_cast<void*>(this);
	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x*2, position.y*2, position.z*2));
	current_model.transform(transform_matrix);
}


Field::~Field()
{
}


void Field::plant_a_seed(Player & p)
{
	if (dynamic_cast<CornSeed*>(p.inventory.get_selected_item())) {
		p.inventory.delete_selected();
		plant = std::make_unique<CornPlant>(this);
	}
	else if (dynamic_cast<CarrotSeed*>(p.inventory.get_selected_item())) {
		p.inventory.delete_selected();
		plant = std::make_unique<CarrotPlant>(this);
	}
	else if (dynamic_cast<MelonSeed*>(p.inventory.get_selected_item())) {
		p.inventory.delete_selected();
		plant = std::make_unique<MelonPlant>(this);
	}
	else if (dynamic_cast<TomatoSeed*>(p.inventory.get_selected_item())) {
		p.inventory.delete_selected();
		plant = std::make_unique<TomatoPlant>(this);
	}
}

void Field::harvest(Player & p)
{
	if (!dynamic_cast<SaleSign*>(plant.get())) {
		p.inventory.insert(plant->harvest());

		Plant *pl = plant.release();
		delete pl;
	}
	else {
		if (p.inventory.get_money() >= SaleSign::price) {
			p.inventory.addMoney(-SaleSign::price);
			Plant *pl = plant.release();
			delete pl;
		}
	}
	

	
}

void Field::update(double timediff, glm::vec3 dir)
{
	if(plant)
		plant->update(timediff, dir);
	
}

void Field::apply_model()
{
	
}

void Field::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, view_frustum);
	if (plant)
		plant->draw(cam_pos, view_frustum);
}


void Field::interact(Player &p)
{
	if (!plant) {
		if (dynamic_cast<Seed*>(p.inventory.get_selected_item())) {
			plant_a_seed(p);
		}
	}
	else if (p.inventory.space_left() != 0 ||dynamic_cast<SaleSign*>(plant.get())) {
		if(plant->is_maxed())
			harvest(p);
	}
}
