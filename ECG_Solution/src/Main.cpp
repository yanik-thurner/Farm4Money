/*
* Copyright 2017 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


#include "Utils.h"
#include <glm/glm.hpp>

#include <sstream>
#include <exception>

#include "Geometry.h"

#include "Camera.h"
#include "Shader.h"

#include "Material.h"
#include "Light.h"

#include "NewTexture.h"

#include "PhysX\PxPhysicsAPI.h"
#include "PhysX\pvd\PxPvd.h"

#include "gameObjects\Player.h"
#include "FarmSimulationEventCallback.h"
#include "gameObjects\Field.h"
#include "gameObjects\Decoration.h"
#include "gameObjects\SellBox.h"
#include "gameObjects\Shop.h"
#include "gameObjects\plants\CornSeed.h"
#include "gameObjects\plants\CarrotSeed.h"
#include "gameObjects\plants\MelonSeed.h"
#include "gameObjects\plants\TomatoSeed.h"
#include "gameObjects\plants\TomatoPlant.h"
#include "gameObjects\plants\CarrotPlant.h"
#include "gameObjects\plants\MelonPlant.h"
#include "gameObjects\plants\CornPlant.h"
#include <memory>
#include "Model.h"
#include "Postp.h"
#include "ModelManager.h"
#include <ft2build.h>
#include "TextRender.h"
#include "Particle.h"
#include "AnimModel.h"
#include "gameObjects\plants\SaleSign.h"
#include "gameObjects\Collidable.h"
#include "gameObjects\Water.h"
#include "gameObjects\Screen.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale\swscale.h"
#include "libavutil\imgutils.h"
}

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void setPerFrameUniforms(Shader* shader, PlayerCamera& camera, DirectionalLight& dirL, std::vector<PointLight>& pointL);
void setParticleUniforms(Shader* shader, PlayerCamera& camera);
void setWaterUniforms(Shader* shader, float time);

void handle_input(GLFWwindow* window, PlayerCamera* camera, Player* player, float dt);
void interact(Player* player);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool DEBUG = true;

static bool _framerate = false;
static bool _wireframe = false;
static bool _culling = false;
static bool _dragging = false;
static bool _strafing = false;
static bool _hud = true;
static float _zoom = 12.0f;
static float _view_frustum = 0;
static float _view_frustum_dist = 5;

/*
0 ... playing
1 ... won
-1 ... lost
*/
static bool _won = false;

std::vector<std::unique_ptr<SceneObject>> sceneObjects;

Player *player;
Shop *shop;
Water *water;

physx::PxPhysics* SceneObject::gPhysicsSDK;
ModelManager *SceneObject::model_manager;
physx::PxScene* SceneObject::gScene;
physx::PxCooking* SceneObject::cooking;

physx::PxPhysics* Collidable::gPhysicsSDK;
physx::PxScene* Collidable::gScene;
physx::PxCooking* Collidable::cooking;

// win/lose text
std::string major_text = "Congratulations, you won!!!";
std::string minor_text = "Press ESC to exit.";
float text_scale_major = 1.2f;
float text_scale_minor = 0.6f;

int margin_major = 50;
int margin_minor = 20;

int char_length = 29;
int char_height = 34;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	INIReader reader("assets/settings.ini");

	if (reader.ParseError() < 0) {
		EXIT_WITH_ERROR("Failed to load 'settings.ini'")
	}

	std::string window_title = "Farm 4 Money";

	int window_width = reader.GetInteger("window", "width", 1280);
	int window_height = reader.GetInteger("window", "height", 768);
	std::string fullscreen = reader.Get("window", "fullscreen", "false");
	int ref = reader.GetInteger("window", "refreshrate", 60);

	float fov = float(reader.GetReal("camera", "fov", 60.0f));
	float nearZ = float(reader.GetReal("camera", "near", 0.1f));
	float farZ = float(reader.GetReal("camera", "far", 100.0f));

	float light = float(reader.GetReal("light", "brightness", 0.5));

	_view_frustum_dist = float(reader.GetReal("misc","view_frustum_culling_distance", 5));

	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW")
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile													  
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Prevent window resizing because viewport would have to resize as well (-> not needed in this course)
	//ref = 144;
	//fullscreen = "true";

	// Open window
	GLFWwindow* window;
	if (fullscreen == "true") {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, ref);

		window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, NULL);
	}
	else {
		window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
	}

	if (!window) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to create window")
	}
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to init GLEW")
	}
	

#if _DEBUG
	// Register your callback function.
	glDebugMessageCallback(DebugCallbackDefault, NULL);
	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. 
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */
	if (!initFramework()) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to init framework")
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set some GL defaults
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// ffmpeg
		std::vector<std::unique_ptr<GLuint>> textureID_vec;
		//opencv_setup(textureID_vec);

		
		std::shared_ptr<Shader> shaderpost = std::make_shared<Shader>("outline.vert", "outline.frag");
		std::shared_ptr<Shader> shadertext = std::make_shared<Shader>("textshader.vert", "textshader.frag");
		std::shared_ptr<Shader> shaderquad = std::make_shared<Shader>("quadshader.vert", "quadshader.frag");
		std::shared_ptr<Shader> shaderpart = std::make_shared<Shader>("particle.vert", "particle.frag");
		std::shared_ptr<Shader> shaderanim = std::make_shared<Shader>("skinning.vert", "toon.frag");

		ModelManager *m = ModelManager::instance();
		SceneObject::model_manager = m;


		//init Particle System
		Particle leaf("//textures/leaf.png", m->shaderpart, glm::vec3(2,2,2));

		//create Post processor
		Postp post(m->shaderpost,window_width,window_height);
		//create text renderer
		TextRender textr(m->shadertext, m->shaderquad, window_width, window_height);


		std::shared_ptr<NewTexture> grassTexture = std::make_shared<NewTexture>("grass.dds");
		std::shared_ptr<Material> grassMaterial = std::make_shared<TextureMaterial>(SceneObject::model_manager->shader, glm::vec3(0.9f, 0.8f, 0.4f), 5.0f, grassTexture);
		
		
		// Initialize camera
		PlayerCamera camera(fov, float(window_width) / float(window_height), nearZ, farZ);
		
		std::vector<PointLight> plight;
		PointLight p1 = PointLight(glm::vec3(1, 1, 1), glm::vec3(3, 1, -3), glm::vec3(0.8, 0.2, 0.1));
		plight.push_back(p1);
		PointLight p2 = PointLight(glm::vec3(1, 1, 1), glm::vec3(-3, 1, -3), glm::vec3(0.8, 0.2, 0.1));
		plight.push_back(p2);
		PointLight p3 = PointLight(glm::vec3(1, 1, 1), glm::vec3(38, 1, -49), glm::vec3(0.8, 0.2, 0.1));
		plight.push_back(p3);
		PointLight p4 = PointLight(glm::vec3(1, 1, 1), glm::vec3(38, 1, -43), glm::vec3(0.8, 0.2, 0.1));
		plight.push_back(p4);
		PointLight p5 = PointLight(glm::vec3(1, 1, 1), glm::vec3(20, 1, -5), glm::vec3(0.8, 0.2, 0.1));
		plight.push_back(p5);
		DirectionalLight dirL(glm::vec3(1,1,1), glm::vec3(0.0f), light);
		
		/* --------------------------------------------- */
		// PhysX Stuff
		/* --------------------------------------------- */
		physx::PxDefaultErrorCallback gDefaultErrorCallback;
		physx::PxDefaultAllocator gDefaultAllocatorCallback;
		physx::PxFoundation* gFoundation = nullptr;
		
		// Create foundation
		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		
		// Create instance of SDK
		physx::PxPhysics* gPhysicsSDK;

		if (DEBUG) {
			physx::PxPvd*  pvd = physx::PxCreatePvd(*gFoundation);
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
			gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, pvd);
		}
		else {
			gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale());
		}
		
		if (gPhysicsSDK == nullptr) {
			EXIT_WITH_ERROR("Failed to create the physics SDK")
		}

		SceneObject::gPhysicsSDK = gPhysicsSDK;
		Collidable::gPhysicsSDK = gPhysicsSDK;

		physx::PxCooking *cooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(gPhysicsSDK->getTolerancesScale()));
		if (!cooking) {
			EXIT_WITH_ERROR("PxCreateCooking failed!")
		}
		SceneObject::cooking = cooking;
		Collidable::cooking = cooking;

		//Create scene
		physx::PxScene* gScene = nullptr;
		physx::PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
		sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		gScene = gPhysicsSDK->createScene(sceneDesc);
		SceneObject::gScene = gScene;
		Collidable::gScene = gScene;

		// Set simulation event callback
		FarmSimulationEventCallback FSECallback;
		gScene->setSimulationEventCallback(&FSECallback);

		//Create Ground Plane Actor
		physx::PxTransform planePos = physx::PxTransform(physx::PxVec3(0.0f, -0.5f, 0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
		physx::PxRigidStatic* planeActor = gPhysicsSDK->createRigidStatic(planePos);
		physx::PxMaterial* mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
		planeActor->createShape(physx::PxPlaneGeometry(), *mMaterial);
		//gScene->addActor(*planeActor);

		// load Groundplane
		Model plane_obj = Model("assets/3d/Scene.blend", SceneObject::model_manager->shader, SceneObject::model_manager->shadercolor);

		sceneObjects.push_back(std::move(std::make_unique<Decoration>(glm::vec3(0,0,0), SceneObject::model_manager->get_model(ModelManager::BODEN), false)));
		Decoration *boden = dynamic_cast<Decoration*>(sceneObjects.back().get());

		// Create Fields
		const float field_width = 2.0f;
		const float field_depth = 2.0f;
		float field_height = 0.5f;
		float field_start_x = 10.0f;
		float field_start_y = 10.0f;
		float field_margin = 0.5f;
		glm::vec2 generation_direction(-5.0f, -3.0f);
		
		/*for (int i = 0; i < std::abs(generation_direction.x); i++) {
			for (int j = 0; j < std::abs(generation_direction.y); j++) {
				float x = field_start_x + i*(field_width + field_margin)*sgn(generation_direction.x);
				float y = 0;
				float z = field_start_y + j * (field_depth + field_margin)*sgn(generation_direction.y);
				sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(x, y, z))));
			}
		}*/

		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(6,0,0)/2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(0, 0, 0) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(-6, 0, 0) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(6, 0, -6) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(0, 0, -6) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(-6, 0, -6) / 2)));

		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(41, 0, -52) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(41, 0, -46) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(41, 0, -40) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(35, 0, -52) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(35, 0, -46) / 2)));
		sceneObjects.push_back(std::move(std::make_unique<Field>(glm::vec3(field_width, field_height, field_depth), glm::vec3(35, 0, -40) / 2)));

		// Lock fields
		int locked_fields = 12;
		for (int i = 0; i < locked_fields; i++) {
			Field *f = dynamic_cast<Field*>(sceneObjects[sceneObjects.size() - 1 - i].get());
			f->plant = std::make_unique<SaleSign>(f);
		}

		// Create player controller
		physx::PxMaterial* pMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
		
		sceneObjects.push_back(std::move(std::make_unique<Player>(gScene, pMaterial)));
		player = dynamic_cast<Player*>(sceneObjects.back().get());

		// Fill inventory with seeds
		player->inventory.insert(std::move(std::make_unique<CornSeed>()));
		player->inventory.insert(std::move(std::make_unique<CarrotSeed>()));
		player->inventory.insert(std::move(std::make_unique<MelonSeed>()));
		player->inventory.insert(std::move(std::make_unique<TomatoSeed>()));
		
		// Create SellBox
		sceneObjects.push_back(std::move(std::make_unique<SellBox>(glm::vec3(2, 1, 1), glm::vec3(10, 0, 0))));

		// Create Lantern
		sceneObjects.push_back(std::move(std::make_unique<Decoration>(glm::vec3(10, 0, -2.5), SceneObject::model_manager->get_model(ModelManager::LANTERN), true)));
		
		// Create Shop
		sceneObjects.push_back(std::move(std::make_unique<Shop>(glm::vec3(2, 1, 1), glm::vec3(10, 0, -5.5))));
		shop = dynamic_cast<Shop*>(sceneObjects.back().get());

		// Create Water
		std::unique_ptr<Decoration> waterbox = std::make_unique<Decoration>(glm::vec3(-10, -4.5, -30), SceneObject::model_manager->get_model(ModelManager::WATERBOX), true);
		sceneObjects.push_back(std::move(std::make_unique<Water>(player, glm::vec3(6, 0.3, 6), glm::vec3(-10, -4, -30))));
		water = dynamic_cast<Water*>(sceneObjects.back().get());


		// Create Screen
		sceneObjects.push_back(std::move(std::make_unique<Screen>(glm::vec2(1, 1), glm::vec3(0.3, 0.1, 3.05))));
		Screen *screen = dynamic_cast<Screen*>(sceneObjects.back().get());
		screen->shader = m->shadervideo.get();

		// Import Sprites
		textr.createTex("assets/sprites/Fox3.jpg", "gras");
		textr.createTex("assets/sprites/quad.png", "quad");

		textr.createTex("assets/sprites/fish.png", "fish");
		textr.createTex("assets/sprites/corn.png", "corn");
		textr.createTex("assets/sprites/carrot.png", "carrot");
		textr.createTex("assets/sprites/melon.png", "melon");
		textr.createTex("assets/sprites/tomato.png", "tomato");

		textr.createTex("assets/sprites/corn_seed.png", "corn_seed");
		textr.createTex("assets/sprites/carrot_seed.png", "carrot_seed");
		textr.createTex("assets/sprites/melon_seed.png", "melon_seed");
		textr.createTex("assets/sprites/tomato_seed.png", "tomato_seed");

		textr.createTex("assets/sprites/rod.png", "rod");
		textr.createTex("assets/sprites/rod_caught1.png", "rod_caught1");
		textr.createTex("assets/sprites/rod_caught2.png", "rod_caught2");

		textr.createTex("assets/sprites/backpack_upgrade.png", "backpack_upgrade");
		textr.createTex("assets/sprites/graphics_card.png", "graphics_card");

		// initialising variables used in the render loop
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_prev = float(glfwGetTime());
		float updt = 1.0f / 60.0f;
		glm::vec3 oldPos = glm::vec3(0, 0, 0);

		// vars for framerate calc
		float fps_smoothing = 0.995f;
		float fps_t_now = float(glfwGetTime());
		float fps_t_prev = fps_t_now;
		float fps_dt = 0;
		float fps = 0.0f;

		// Render loop
		while (!glfwWindowShouldClose(window)) {
			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_BLEND);

			// check win condition
			if (player->inventory.size() != 0) {
				if (player->inventory.contains("graphics_card"))
					_won = 1;
			}
			
			// Compute frame time
			float t_now = float(glfwGetTime());
			dt = t_now - t_prev;
			fps_dt = t_now - fps_t_prev;
			fps = (fps * fps_smoothing) + ((1.0f / fps_dt) * (1.0f - fps_smoothing));

			while (dt > updt) {

				// Handle mouse/keyboard input
				handle_input(window, &camera, player, updt);
			
				// get player actor position
				physx::PxExtendedVec3 temppos = player->getPos();
				glm::vec3 pos = glm::vec3(temppos.x+oldPos.x,temppos.y+oldPos.y,temppos.z+oldPos.z);

				// Update Camera
				double mouse_x, mouse_y;
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				camera.update(int(mouse_x), int(mouse_y), glm::vec3(pos.x, pos.y + 0.5, pos.z), updt, true); //update camera position with player controller position
				
				// update player model position
				player->resetModelMatrix();
				player->transform(pos,camera);
				oldPos = glm::vec3(temppos.x, temppos.y, temppos.z);
				//std::cout << "x:" << oldPos.x << "y:" << oldPos.y << "z:" << oldPos.z << std::endl;

				// update scene
				gScene->simulate(updt);
				gScene->fetchResults(true);

				//update all objects in the scene
				for (size_t i = 0; i < sceneObjects.size(); i++) {
					sceneObjects[i]->update(updt, glm::vec3(1, 1, 1));
				}

				//set time
				t += dt;
				dt -= updt;
			}
			t_prev = t_now - dt;
			
			// Set per-frame uniforms
			setPerFrameUniforms(SceneObject::model_manager->shader.get(), camera, dirL, plight);
			setPerFrameUniforms(SceneObject::model_manager->shadercolor.get(), camera, dirL, plight);
			setPerFrameUniforms(SceneObject::model_manager->shaderwater.get(), camera, dirL, plight);
			setPerFrameUniforms(SceneObject::model_manager->shadervideo.get(), camera, dirL, plight);
			setParticleUniforms(SceneObject::model_manager->shaderpart.get(), camera);
			setWaterUniforms(SceneObject::model_manager->shaderwater.get(), t_now);

			// render scene
			if (!_wireframe) post.BeginRender();
			plane_obj.draw(camera.getPosition(), _view_frustum);
;			boden->draw(camera.getPosition(), 0);
			for (size_t i = 0; i < sceneObjects.size(); i++) {
				sceneObjects[i]->draw(camera.getPosition(), _view_frustum);
			}

			if (!_wireframe)post.EndRender();

			if (!_wireframe)post.Render();
			
			// Text, X-Offs, Y-Offs, Scale, Color	->offset 0 is lower left corner
			if (_framerate)textr.renderText(std::to_string(int(fps)), 15.f, window_height -50.f, 1.0f, glm::vec3(0.1, 1, 0.1));
			//glDisable(GL_BLEND);
			
			
			//textr.renderquad("gras", glm::vec2(200.f,200.f), glm::vec2(200.f, 200.f), glm::vec4(1, 1, 1, 1));
			//textr.renderquad("quad", glm::vec2(200.f, 200.f), glm::vec2(200.f, 200.f), glm::vec3(1, 1, 1));

			if (_hud) {
				player->render(textr, window_width, window_height);
				player->inventory.render(textr, window_width, window_height);
				shop->render(textr, window_width, window_height);
				water->render(textr, window_width, window_height);

				if (_won) {
					int x_offset_major = int((window_width - major_text.size() * char_length * text_scale_major) / 2);
					int x_offset_minor = int((window_width - minor_text.size() * char_length * text_scale_minor) / 2);
					int y_offset_major = int(window_height - margin_major - char_height * text_scale_major);
					int y_offset_minor = int(y_offset_major - margin_minor - char_height * text_scale_minor);

					textr.renderText(major_text, x_offset_major, y_offset_major, text_scale_major, glm::vec3(0, 0, 0));
					textr.renderText(minor_text, x_offset_minor, y_offset_minor, text_scale_minor, glm::vec3(0, 0, 0));
				}
			}	
			// Poll events and swap buffers
			glfwPollEvents();
			glfwSwapBuffers(window);

			fps_t_prev = t_now;
		}

		/* --------------------------------------------- */
		// Destroy Physx
		/* --------------------------------------------- */
		gScene->release();
		gPhysicsSDK->release();
		gFoundation->release();
	}

	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}


void handle_input(GLFWwindow* window, PlayerCamera* camera, Player* player, float dt) {
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);

	glm::vec3 viewDir = camera->getViewDir(); // get camera view direction to use as move direction
	viewDir = glm::normalize(glm::vec3(viewDir.x, 0, viewDir.z));

	int state_space = glfwGetKey(window, GLFW_KEY_SPACE);
	if (state_space == GLFW_PRESS) {
		player->jump();
	}
	float jump = player->getJumpHeight(dt);

	int state_w = glfwGetKey(window, GLFW_KEY_W);
	if (state_w == GLFW_PRESS) {
		player->move(dt, glm::vec3(-viewDir.x, jump, -viewDir.z)); //move forward
		jump = 0;
	}

	int state_s = glfwGetKey(window, GLFW_KEY_S);
	if (state_s == GLFW_PRESS) {
		player->move(dt, glm::vec3(viewDir.x, jump, viewDir.z)); //move back
		jump = 0;
	}

	int state_a = glfwGetKey(window, GLFW_KEY_A);
	if (state_a == GLFW_PRESS) {
		glm::vec3 cross = glm::normalize(glm::cross(glm::normalize(glm::vec3(viewDir.x, 0, viewDir.z)), glm::normalize(glm::vec3(viewDir.x, 1, viewDir.z))));
		player->move(dt, glm::vec3(cross.x, jump, cross.z));
		jump = 0;
	}

	int state_d = glfwGetKey(window, GLFW_KEY_D);
	if (state_d == GLFW_PRESS) {
		glm::vec3 cross = glm::normalize(glm::cross(glm::normalize(glm::vec3(-viewDir.x, 0, -viewDir.z)), glm::normalize(glm::vec3(-viewDir.x, 1, -viewDir.z))));
		player->move(dt, glm::vec3(cross.x, jump, cross.z));
		jump = 0;
	}

	if (state_w != GLFW_PRESS && state_s != GLFW_PRESS && state_a != GLFW_PRESS && state_d != GLFW_PRESS )
		player->move(dt, glm::vec3(0, jump, 0));
	
	int state_alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT); 
	if (state_alt == GLFW_PRESS) {
		player->movehead = true;
	}
	else player->movehead = false;
}

void setPerFrameUniforms(Shader* shader, PlayerCamera& camera, DirectionalLight& dirL, std::vector<PointLight>& pointL)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
	shader->setUniform("dirL.amb", dirL.amb);

	shader->setUniform("pointL[0].color", pointL[0].color);
	shader->setUniform("pointL[0].position", pointL[0].position);
	shader->setUniform("pointL[0].attenuation", pointL[0].attenuation);

	shader->setUniform("pointL[1].color", pointL[1].color);
	shader->setUniform("pointL[1].position", pointL[1].position);
	shader->setUniform("pointL[1].attenuation", pointL[1].attenuation);

	shader->setUniform("pointL[2].color", pointL[2].color);
	shader->setUniform("pointL[2].position", pointL[2].position);
	shader->setUniform("pointL[2].attenuation", pointL[2].attenuation);

	shader->setUniform("pointL[3].color", pointL[3].color);
	shader->setUniform("pointL[3].position", pointL[3].position);
	shader->setUniform("pointL[3].attenuation", pointL[3].attenuation);

	shader->setUniform("pointL[4].color", pointL[4].color);
	shader->setUniform("pointL[4].position", pointL[4].position);
	shader->setUniform("pointL[4].attenuation", pointL[4].attenuation);
}


void setParticleUniforms(Shader* shader, PlayerCamera& camera)
{
	shader->use();
	shader->setUniform("CameraRight", glm::vec3(camera.getViewMatrix()[0][0], camera.getViewMatrix()[1][0], camera.getViewMatrix()[2][0]));
	shader->setUniform("CameraUp", glm::vec3(camera.getViewMatrix()[0][1], camera.getViewMatrix()[1][1], camera.getViewMatrix()[2][1]));
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
}

void setWaterUniforms(Shader* shader, float time)
{
	shader->use();
	shader->setUniform("time", time);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	player->inventory.selected_item_change(-yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Framerate
	// Esc - Exit

	if (action != GLFW_RELEASE) return;

	if (key >= 48 && key <= 57) {
		shop->buy(key - 48 - 1, *player);
	}
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_F1:
			// Help if available
			break;
		case GLFW_KEY_F2:
			_framerate = !_framerate;
			break;
		case GLFW_KEY_F3:
			_wireframe = !_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_F4:
			_hud = !_hud;
			break;
		case GLFW_KEY_F8:
			if (_view_frustum)
				_view_frustum = 0;
			else
				_view_frustum = _view_frustum_dist;

			break;
		case GLFW_KEY_F:
			interact(player);
			break;
		case GLFW_KEY_E:
			std::cout << player->inventory.toString() << std::endl;
			break;
		case GLFW_KEY_P:
			player->inventory.addMoney(1000);
			break;
	}
}

void interact(Player* player) {
	for (int i = 0; i < sceneObjects.size(); i++) {
		Interactable *interactable = dynamic_cast<Interactable*>(sceneObjects[i].get());
		if (interactable != nullptr && interactable->playerInRange) {
			interactable->interact(*player);
			return;
		}
	}
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings (buffer uses GPU memory, shader recompilation) from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	switch (source) {
		case GL_DEBUG_SOURCE_API: {
			sourceString = "API";
			break;
		}
		case GL_DEBUG_SOURCE_APPLICATION: {
			sourceString = "Application";
			break;
		}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			sourceString = "Window System";
			break;
		}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			sourceString = "Shader Compiler";
			break;
		}
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			sourceString = "Third Party";
			break;
		}
		case GL_DEBUG_SOURCE_OTHER: {
			sourceString = "Other";
			break;
		}
		default: {
			sourceString = "Unknown";
			break;
		}
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			typeString = "Error";
			break;
		}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			typeString = "Deprecated Behavior";
			break;
		}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			typeString = "Undefined Behavior";
			break;
		}
		case GL_DEBUG_TYPE_PORTABILITY_ARB: {
			typeString = "Portability";
			break;
		}
		case GL_DEBUG_TYPE_PERFORMANCE: {
			typeString = "Performance";
			break;
		}
		case GL_DEBUG_TYPE_OTHER: {
			typeString = "Other";
			break;
		}
		default: {
			typeString = "Unknown";
			break;
		}
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			severityString = "High";
			break;
		}
		case GL_DEBUG_SEVERITY_MEDIUM: {
			severityString = "Medium";
			break;
		}
		case GL_DEBUG_SEVERITY_LOW: {
			severityString = "Low";
			break;
		}
		default: {
			severityString = "Unknown";
			break;
		}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}