#include "Plant.h"
#include <iostream>

Plant::Plant(Field *parent, std::vector<Model> models) : SceneObject(models, glm::mat4(1.0f), parent->_position), particle_system(Particle("//textures/leaf.png", SceneObject::model_manager->shaderpart, glm::vec3(parent->pos.x*2, parent->pos.y * 2, parent->pos.z* 2)))
{
	transform_matrix = parent->transform_matrix;
	current_model.transform(transform_matrix);
}

void Plant::grow()
{
	if(plantState < level_times.size())
		plantState += 1;
}

void Plant::update(double timediff, glm::vec3 dir)
{
	if (level_times.size() == 0) return;

	total_grow_time += timediff;

	if (plantState < (level_times.size() - 1)) {
		if (level_times[plantState] <= total_grow_time) {
			grow();
			total_grow_time = 0;
			apply_model();
		}
	}

	if (plantState == level_times.size() - 1) {
		particle_system.generate(timediff);
		particle_system.update(timediff, dir);
	}
}

void Plant::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, view_frustum);
	if (plantState == level_times.size() - 1) {
		particle_system.draw();
	}
}

void Plant::apply_model()
{
	if (plantState == 0)
		current_model = idle_model;
	else {
		if (plantState < models.size())
			current_model = models[plantState];
	}

	current_model.setTransformMatrix(transform_matrix);
}
