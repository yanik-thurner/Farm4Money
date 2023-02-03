#include "Decoration.h"



Decoration::Decoration(glm::vec3 position, std::vector<Model> models, bool vf_affected) : SceneObject(models, glm::mat4(1.0f), position), Collidable(current_model, position)
{

	if (!shape) shape = generate_shape();
	set_actor(generate_actor(shape));


	actor->userData = static_cast<void*>(this);


	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * 2, position.y * 2, position.z * 2));
	current_model.transform(transform_matrix);
}


Decoration::~Decoration()
{
}

void Decoration::setModelMatVec(std::vector<std::shared_ptr<Material>> mats)
{
	current_model.setMatVec(mats);
	idle_model.setMatVec(mats);
}

void Decoration::apply_model()
{

}


void Decoration::update(double timediff, glm::vec3 dir)
{
}

void Decoration::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, view_frustum);
}
