#include <iostream>

#include "basetsd.h"
#include <AntTweakBar/TweakBar/include/AntTweakBar.h>
#include <Glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>
#include <plf_nanotimer/plf_nanotimer.h>

#include <thread>
#include <chrono>
#include <vector>
#if defined(_WIN32)
#include <Windows.h>
#endif
#include <assert.h>
#include <iostream>

#include "Utils.h"
#include "Renderer.h"
#include "Obj_Parser/wavefront_obj.h"

#include "osstring.h"

// My include:

#include <map>
#include <tuple>
#include <initializer_list>
#include <condition_variable>

#include "Renderer.h"

#include "Draw.h"
#include "DrawBuffer.h"
#include "ScreenPixels.h"

#include "Vector4.h"
#include "Matrix4.h"

#include "Object.h"
#include "Camera.h"
#include "Light.h"

#include "static_vector.h"

#include "Keys.h"
#include "Mouse.h"
#include "Motion.h"
#include "Screen.h"
#include "ToggleMap.h"
#include "PI.h"

#include "DefaultKeybind.h"
#include "Variables.h"

#include "Defines.h"

// Number of hardware supported parallel threads
int num_hw_threads;

// Number of control threads
int num_control_threads;

// Number of render threads
int num_render_threads;

// Keyboard-Action variables
typedef KeyPress<int> KeyboardPress;
typedef KeyPress<int> MousePress;
typedef KeyBind<int, Action, DEFAULT_NONE, Action::NONE> KeyboardBind;
typedef KeyBind<int, Action, DEFAULT_NONE, Action::NONE> MouseBind;
KeyboardPress keyboardPress;
MousePress mousePress;
KeyboardBind keyboardBind;
MouseBind mouseBind;

// State variables
size_t frame_number = 0;
Mouse mouse;
bool exit_flag = false;


// Lighting mode enum
typedef enum { LIGHT_POINT, LIGHT_DIR, NUM_OF_LIGHTING_MODES } LightingEnum;

// Lighting mode
LightingEnum light1_mode;
LightingEnum light2_mode;

TwEnumVal lightingEnumString[] = { {LIGHT_POINT, "Point"}, {LIGHT_DIR, "Directional"} };
TwType lightingTwType;

TwEnumVal shadingEnumString[] = { {SHADING_WIRE, "Wireframe"}, {SHADING_FLAT, "Flat"}, {SHADING_GOURAUD, "Gouraud"}, {SHADING_PHONG, "Phong"}, {SHADING_FLAT_TEST, "Flat Test"}, {SHADING_GOURAUD_TEST, "Gouraud Test"} };
TwType shadingTwType;

// Renderer
Renderer renderer;

// AntTweakBar
TwBar* bar;

// GLFW window
GLFWwindow *window = NULL;

// Render timer
plf::nanotimer render_timer;
// Control loop timer
plf::nanotimer control_timer;
// Elapsed render time
UINT32 render_elapsed_us;
// Elapsed control loop time
UINT32 control_elapsed_us;

//obj data type
Wavefront_obj objScene;

// Camera motion
Motion cam_motion;
// Object motion
Motion obj_motion;
// Object scale
Motion obj_scale_motion; // Only use the x coordinate

// Unique variables
VarsUnique uVarsArr[2];
VarsUnique *uVars;
VarsUnique *prev_uVars;
VarsUnique renderer_uVars;

// Shared variables
VarsShared sVars;

// Mesh model pending
MeshModel meshModel_pending;
bool is_meshModel_pending = false;

// Mutex between the main thread and the renderer thread
std::mutex mtx_main_renderer;

// Condition variable that signals the request to draw a new thread
std::condition_variable cv_main_renderer;
bool render_next = false;

static void assert_m(const bool expr, const char *err) {
	if (!expr) {
		std::cerr << err << std::endl;
		exit(1);
	}
}

static inline void sleep_frame() {
	std::this_thread::sleep_for(std::chrono::microseconds(FRAME_US_I));
}

void glUseScreenCoordinates(int width, int height);

void control();
void TW_CALL loadOBJModel(void* clientData);
void initScene();
void initGraphics(GLFWwindow *&window);
static inline void drawScene();
void display();
void window_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void terminate(void);

void initCallbacks();
void initTweakBar();
void initMaterial();
void storeMaterial();
void loadMaterial();
void initObject();
void initVariables();
void reInitVariables();
void initKeybindings();
void performAction(Action action, bool press);
void update_motion(Motion &motion,
	Action left, Action right,
	Action forward, Action backward,
	Action up, Action down);

void setLightMode(int light_num, LightingEnum &light_mode, const LightingEnum &new_light_mode);
void TW_CALL setLight1Mode(const void *value, void *clientData) { setLightMode(1, light1_mode, *(LightingEnum*)value); }
void TW_CALL setLight2Mode(const void *value, void *clientData) { setLightMode(2, light2_mode, *(LightingEnum*)value); }
void TW_CALL getLight1Mode(void *value, void *clientData) { *(LightingEnum*)value = light1_mode; }
void TW_CALL getLight2Mode(void *value, void *clientData) { *(LightingEnum*)value = light2_mode; }
#define SET_VAR_TEMPLATE(NAME, TYPE, VAR) void TW_CALL NAME (const void *value, void *clientData) { VAR = *( TYPE *) value; }
#define GET_VAR_TEMPLATE(NAME, TYPE, VAR) void TW_CALL NAME (void *value, void *clientData) { *( TYPE *) value = VAR ; }
SET_VAR_TEMPLATE(setShadingMode, ShadingEnum, uVars->shading_mode)
GET_VAR_TEMPLATE(getShadingMode, ShadingEnum, uVars->shading_mode)
SET_VAR_TEMPLATE(setObjColor, Color, uVars->obj_color)
GET_VAR_TEMPLATE(getObjColor, Color, uVars->obj_color)
SET_VAR_TEMPLATE(setBBoxColor, Color, uVars->bbox_color)
GET_VAR_TEMPLATE(getBBoxColor, Color, uVars->bbox_color)
SET_VAR_TEMPLATE(setNormalsColor, Color, uVars->normals_color)
GET_VAR_TEMPLATE(getNormalsColor, Color, uVars->normals_color)
SET_VAR_TEMPLATE(setNormalsLength, double, uVars->object.normals_length)
GET_VAR_TEMPLATE(getNormalsLength, double, uVars->object.normals_length)
SET_VAR_TEMPLATE(setAmbientCoefficient, Color, uVars->material.k_ambient)
GET_VAR_TEMPLATE(getAmbientCoefficient, Color, uVars->material.k_ambient)
SET_VAR_TEMPLATE(setDiffuseCoefficient, Color, uVars->material.k_diffuse)
GET_VAR_TEMPLATE(getDiffuseCoefficient, Color, uVars->material.k_diffuse)
SET_VAR_TEMPLATE(setSpecularCoefficient, Color, uVars->material.k_specular)
GET_VAR_TEMPLATE(getSpecularCoefficient, Color, uVars->material.k_specular)
SET_VAR_TEMPLATE(setSpecularPower, double, uVars->material.n_specular)
GET_VAR_TEMPLATE(getSpecularPower, double, uVars->material.n_specular)
SET_VAR_TEMPLATE(setAmbientIntensity, double, uVars->lighting[0].getIntensity())
GET_VAR_TEMPLATE(getAmbientIntensity, double, uVars->lighting[0].getIntensity())
SET_VAR_TEMPLATE(setLight1Intensity, double, uVars->lighting[1].getIntensity())
GET_VAR_TEMPLATE(getLight1Intensity, double, uVars->lighting[1].getIntensity())
SET_VAR_TEMPLATE(setLight2Intensity, double, uVars->lighting[2].getIntensity())
GET_VAR_TEMPLATE(getLight2Intensity, double, uVars->lighting[2].getIntensity())
SET_VAR_TEMPLATE(setLight1Vec, Vector3, uVars->lighting[1].getVector())
GET_VAR_TEMPLATE(getLight1Vec, Vector3, uVars->lighting[1].getVector())
SET_VAR_TEMPLATE(setLight2Vec, Vector3, uVars->lighting[2].getVector())
GET_VAR_TEMPLATE(getLight2Vec, Vector3, uVars->lighting[2].getVector())

void render_worker();
void main_loop();
void frame_start(plf::nanotimer &timer);
void frame_end(plf::nanotimer &timer);


int main(int argc, char *argv[])
{
	exit_flag = false;
	std::atexit(terminate);

	// Get the number of hardware supported parallel threads
	num_hw_threads = std::thread::hardware_concurrency();
	// Appoint one control thread
	num_control_threads = 1;
	// Appoint the remaining threads (or at least one) to rendering
	num_render_threads = num_hw_threads - num_control_threads;
	if (num_render_threads <= 0) num_render_threads = 1;

	// Initialize the keybindings
	initKeybindings();

	// Initialize openGL, glfw, glew
	initGraphics(window);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Initialize the times
	render_elapsed_us = 0;
	control_elapsed_us = 0;

	// Set GLFW callbacks
	initCallbacks();

	// Teminates AntTweakBar and GLFW on program exit
	atexit(terminate);

	// Initialize all the other variables
	initVariables();

	// Create a tweak bar
	initTweakBar();

	glUseScreenCoordinates(sVars.screen.x, sVars.screen.y);
	TwWindowSize(sVars.screen.x, sVars.screen.y);

	std::thread thrd_render_loop(render_worker);

	main_loop(); // Must run from the main thread

	for (std::thread *thrd : { &thrd_render_loop }) {
		thrd->join();
	}

	return 0;
}

void frame_start(plf::nanotimer &timer) {
	timer.start();
}
void frame_end(plf::nanotimer &timer) {
	double time = timer.get_elapsed_us();
	double remaining_time = FRAME_US - time;
	uint64_t sleep_time = (uint64_t)remaining_time;
	if (remaining_time > 0 && sleep_time > 0) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}
}

void render_worker() {
	while (!exit_flag) {
		std::unique_lock<std::mutex> lk(mtx_main_renderer);
		{
			// Wait until ready to render the next frame
			cv_main_renderer.wait(lk, []{ return render_next && !exit_flag; });
			render_next = false; // We are handling that request

			// Synchronize the unique variables
			renderer_uVars = *prev_uVars;

			// Synchronize the mesh model
			if (is_meshModel_pending) {
				size_t numPoints = meshModel_pending.vertices.size();
				sVars.draw_arr.resize_pending(numPoints);

				sVars.meshModel = meshModel_pending; // Update the mesh model

				is_meshModel_pending = false;
			}
		}
		lk.unlock();

		// Synchronize the shared variables
		sVars.sync();

		// Render time measure -- start
		render_timer.start();

		// Calculate the scene
		drawScene();

		// Render time measure -- end
		render_elapsed_us = static_cast<UINT32>(render_timer.get_elapsed_us());

		// Advance the next screen buffer
		sVars.screen_buffers.next();
	}
}

void main_loop() {
	static plf::nanotimer timer;
	while (!glfwWindowShouldClose(window)) {
		frame_start(timer);

		{
			std::lock_guard<std::mutex> lk(mtx_main_renderer);
			render_next = true;
		}
		cv_main_renderer.notify_all();

		// Handle events (e.g. mouse movement, window resize)
		glfwPollEvents(); // Must run from the main thread

		control();

		{
			std::lock_guard<std::mutex> lk(mtx_main_renderer);
			std::swap(uVars, prev_uVars);
			*uVars = *prev_uVars;
		}

		display();

		frame_end(timer);
	}
	exit_flag = true;
	cv_main_renderer.notify_all();
}

void initCallbacks() {
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, keyboard_callback);
}

void initTweakBar() {
	bar = TwNewBar("TweakBar");

	TwDefine(
		" GLOBAL help='"
		  "Press Insert to load an object."
		"\nPress WASD/Space/Shift to move."
		"\nPress F to use the mouse for free look."
		"\nPress G to use the mouse for FPS look."
		"\nPress V to toggle view object."
		"\nPress the Arrow Keys/Page Up/Page Down to move the object."
		"\nPress Alt to move the object relative to world coordinates."
		"\nPress Right Mouse Button to rotate the object."
		"\nPress +/- to change the object scale."
		"\nPress Ctrl to change the object in model coordinates instead of world coordinates."
		"\nPress P to toggle between the projection types."
		"\nPress R to reset the scene."
		"\nPress Esc to turn off all mouse modes."
		"\nPress B to show/hide the bounding box."
		"\nPress N to show/hide the vertex normals."
		"\nPress X to show/hide the world axes."
		"\nPress U to show/hide the object axes."
	"'"); // Message added to the help bar.
	TwDefine(" TweakBar size='200 600' color='96 216 224' "); // change default tweak bar size and color


	shadingTwType = TwDefineEnum("ShadingMode", shadingEnumString, NUM_OF_SHADERS); // Define the shading type
	TwAddVarCB(bar, "Shading", shadingTwType, setShadingMode, getShadingMode, NULL, " help='The shading mode' ");

	TwAddSeparator(bar, NULL, NULL);

	// Define the color bars
	TwAddVarCB(bar, "Object Color", TW_TYPE_COLOR32, setObjColor, getObjColor, NULL , " help='The objects color' ");
	TwAddVarCB(bar, "Box Color", TW_TYPE_COLOR32, setBBoxColor, getBBoxColor, NULL, " help='The objects bounding box color' ");
	TwAddVarCB(bar, "Normals Color", TW_TYPE_COLOR32, setNormalsColor, getNormalsColor, NULL, " help='The objects normals color' ");
	TwAddVarCB(bar, "Normals Length", TW_TYPE_DOUBLE, setNormalsLength, getNormalsLength, NULL, " step=0.01, min=-10.0, max=10.0, help='The objects normals length' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarCB(bar, "Ambient Coefficient", TW_TYPE_COLOR32, setAmbientCoefficient, getAmbientCoefficient, NULL, " help='The ambient coefficient' ");
	TwAddVarCB(bar, "Diffuse Coefficient", TW_TYPE_COLOR32, setDiffuseCoefficient, getDiffuseCoefficient, NULL, " help='The diffuse coefficient' ");
	TwAddVarCB(bar, "Specular Coefficient", TW_TYPE_COLOR32, setSpecularCoefficient, getSpecularCoefficient, NULL, " help='The specular coefficient' ");
	TwAddVarCB(bar, "Specular Power", TW_TYPE_DOUBLE, setSpecularPower, getSpecularPower, NULL, " min=1.0, help='The specular power' ");

	TwAddSeparator(bar, NULL, NULL);

	lightingTwType = TwDefineEnum("LightingMode", lightingEnumString, NUM_OF_LIGHTING_MODES); // Define the lighting type
	TwAddVarCB(bar, "Light 1 Mode", lightingTwType, setLight1Mode, getLight1Mode, NULL, " help='Light 1 type' ");
	TwAddVarCB(bar, "Light 2 Mode", lightingTwType, setLight2Mode, getLight2Mode, NULL, " help='Light 2 type' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarCB(bar, "Ambient Light Intensity", TW_TYPE_DOUBLE, setAmbientIntensity, getAmbientIntensity, NULL, " min=0.0, max=1.0, step=0.01, help='The ambient lights intensity' ");
	TwAddVarCB(bar, "Light 1 Intensity", TW_TYPE_DOUBLE, setLight1Intensity, getLight1Intensity, NULL, " min=0.0, max=1.0, step=0.01, help='Light 1s intensity' ");
	TwAddVarCB(bar, "Light 2 Intensity", TW_TYPE_DOUBLE, setLight2Intensity, getLight2Intensity, NULL, " min=0.0, max=1.0, step=0.01, help='Light 1s intensity' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarCB(bar, "Light 1 Position/Direction", TW_TYPE_DIR3D, setLight1Vec, getLight1Vec, NULL, " help='Light 1s position/direction' ");
	TwAddVarCB(bar, "Light 2 Position/Direction", TW_TYPE_DIR3D, setLight2Vec, getLight2Vec, NULL, " help='Light 1s position/direction' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarRO(bar, "Render (us)", TW_TYPE_UINT32, &render_elapsed_us, " help='Shows the drawing time in micro seconds' ");
	TwAddVarRO(bar, "Control (us)", TW_TYPE_UINT32, &control_elapsed_us, " help='Shows the main control function time in micro seconds' ");

	TwAddSeparator(bar, NULL, NULL);
	
	//add 'g_Scale' to 'bar': this is a modifiable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [z] and [Z].
	//TwAddVarRW(bar, "Scale", TW_TYPE_DOUBLE, &g_Scale, " min=0.01 max=2.5 step=0.01 keyIncr=+ keyDecr=- help='Scale the object (1=original size).' ");

	//add 'g_quaternion' to 'bar': this is a variable of type TW_TYPE_QUAT4D which defines the object's orientation using quaternions
	//TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4D, &g_quaternion, " label='Object rotation' opened=true help='Change the object orientation.' ");

	TwAddButton(bar, "open", loadOBJModel, NULL, " label='Open OBJ File...' ");
}

void initMaterial() {
	uVars->material.k_ambient = K_AMBIENT;
	uVars->material.k_diffuse = K_DIFFUSE;
	uVars->material.k_specular = K_SPECULAR;
	uVars->material.n_specular = N_SPECULAR;
}

void storeMaterial() {
	uVars->material = uVars->object.material;
}

void loadMaterial() {
	uVars->object.material = uVars->material;
}

void initObject() {
	uVars->object.model = Matrix4::I();
	uVars->object.world = Matrix4::I();
	uVars->object.world_pos = Vector4(0.0, 0.0, 0.0);
	uVars->object.model_pos = Vector4(0.0, 0.0, 0.0);
	uVars->object.rot = Matrix4::I();
	loadMaterial();
}

void initVariables() {
	// Initialize uVars
	uVars = &uVarsArr[0];
	prev_uVars = &uVarsArr[1];
	uVars->object.p_meshModel = &sVars.meshModel;

	// Initialize sVars
	sVars.init();

	// Add light sources
	uVars->lighting.push_back(AmbientLight(AMBIENT_LIGHT_INTENSITY));
	uVars->lighting.push_back(PointLight(POINT_LIGHT1_POS, LIGHT1_INTENSITY));
	uVars->lighting.push_back(PointLight(POINT_LIGHT2_POS, LIGHT2_INTENSITY));

	reInitVariables();

	std::swap(uVars, prev_uVars);
	*uVars = *prev_uVars;
}

void reInitVariables() {
	// Reinitialize light sources
	uVars->lighting[0] = AmbientLight(AMBIENT_LIGHT_INTENSITY);
	uVars->lighting[1] = PointLight(POINT_LIGHT1_POS, LIGHT1_INTENSITY);
	uVars->lighting[2] = PointLight(POINT_LIGHT2_POS, LIGHT2_INTENSITY);

	// Initialize the camera motion
	cam_motion = Motion(GO_ACCEL, STOP_ACCEL, MAX_ACCEL, MAX_VELOC);
	// Initialize the object motion
	obj_motion = Motion(GO_ACCEL, STOP_ACCEL, MAX_ACCEL, MAX_VELOC);
	// Initialize the object scale (implemented with motion)
	obj_scale_motion = Motion(0.01, -0.1, 0.5, 0.05);

	// Initialize camera
	uVars->camera.inv_view = Matrix4::inv_translation(CAMERA_X, CAMERA_Y, CAMERA_Z); // Test Camera
	uVars->camera.pos = Vector4(CAMERA_X, CAMERA_Y, CAMERA_Z);
	uVars->camera.v = Vector4(0.0, 1.0, 0.0);
	uVars->camera.w = Vector4(0.0, 0.0, 1.0);
	uVars->camera.u_angle = 0.0;
	uVars->camera.rot = Matrix4::I();

	// Initialize the lighting
	light1_mode = INITIAL_LIGHT1_MODE;
	light2_mode = INITIAL_LIGHT2_MODE;

	// Initialize the shading mode
	uVars->shading_mode = INITIAL_SHADING_MODE;

	// Initialize the material
	initMaterial();

	// Initialize Object
	initObject();

	// Initialize projection matrix
	uVars->camera.n = CAMERA_N;
	uVars->camera.f = CAMERA_F;
	uVars->camera.fovy = CAMERA_FOVY;
	uVars->camera.aspect_ratio = sVars.screen.aspect_ratio();
	uVars->camera.ortho_fov_scale = CAMERA_ORTHO_FOV_SCALE;
	uVars->camera.update_projections();
	uVars->camera.proj_type = ProjectionType::PERSPECTIVE;

	// Initialize the mouse
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mouse = Mouse(window, xpos, ypos);
	mouse.normal(); // Make the cursor visible

	// Initialize the actions
	uVars->state[Action::BOUNDING_BOX] = false;
	uVars->state[Action::VERTEX_NORMALS] = false;
	uVars->state[Action::WORLD_AXES] = true;
	uVars->state[Action::MOUSE_LOOK] = false;
	uVars->state[Action::FPS_CAMERA] = false;
	uVars->state[Action::OBJ_ROTATE] = false;
	uVars->state[Action::VIEW_OBJECT] = false;
	uVars->state[Action::OBJ_CONTROL_MODEL] = false;
}

void initKeybindings() {
	// Bind keyboard
	keyboardBind.set(DEFAULT_LOAD_MESH_MODEL, Action::LOAD_MESH_MODEL);
	keyboardBind.set(DEFAULT_CAM_LEFT, Action::CAM_LEFT);
	keyboardBind.set(DEFAULT_CAM_RIGHT, Action::CAM_RIGHT);
	keyboardBind.set(DEFAULT_CAM_FORWARD, Action::CAM_FORWARD);
	keyboardBind.set(DEFAULT_CAM_BACKWARD, Action::CAM_BACKWARD);
	keyboardBind.set(DEFAULT_MOUSE_LOOK, Action::MOUSE_LOOK);
	keyboardBind.set(DEFAULT_CAM_UP, Action::CAM_UP);
	keyboardBind.set(DEFAULT_BOUNDING_BOX, Action::BOUNDING_BOX);
	keyboardBind.set(DEFAULT_VERTEX_NORMALS, Action::VERTEX_NORMALS);
	keyboardBind.set(DEFAULT_WORLD_AXES, Action::WORLD_AXES);
	keyboardBind.set(DEFAULT_OBJECT_AXES, Action::OBJECT_AXES);
	keyboardBind.set(DEFAULT_OBJ_SCALE_INC, Action::OBJ_SCALE_INC);
	keyboardBind.set(DEFAULT_OBJ_SCALE_DEC, Action::OBJ_SCALE_DEC);
	keyboardBind.set(DEFAULT_PROJECTION_TOGGLE, Action::PROJECTION_TOGGLE);
	keyboardBind.set(DEFAULT_RESET_SCENE, Action::RESET_SCENE);
	keyboardBind.set(DEFAULT_FPS_CAMERA, Action::FPS_CAMERA);
	keyboardBind.set(DEFAULT_VIEW_OBJECT, Action::VIEW_OBJECT);
	keyboardBind.set(DEFAULT_ESCAPE_ALL, Action::ESCAPE_ALL);
	keyboardBind.set(DEFAULT_OBJ_LEFT, Action::OBJ_LEFT);
	keyboardBind.set(DEFAULT_OBJ_RIGHT, Action::OBJ_RIGHT);
	keyboardBind.set(DEFAULT_OBJ_FORWARD, Action::OBJ_FORWARD);
	keyboardBind.set(DEFAULT_OBJ_BACKWARD, Action::OBJ_BACKWARD);
	keyboardBind.set(DEFAULT_OBJ_UP, Action::OBJ_UP);
	keyboardBind.set(DEFAULT_OBJ_DOWN, Action::OBJ_DOWN);
	keyboardBind.set(DEFAULT_CAM_DOWN, Action::CAM_DOWN);
	keyboardBind.set(DEFAULT_OBJ_ALT_MOVE, Action::OBJ_ALT_MOVE);
	keyboardBind.set(DEFAULT_OBJ_CONTROL_MODEL, Action::OBJ_CONTROL_MODEL);

	// Mouse button bindings
	mouseBind.set(DEFAULT_OBJ_ROTATE, Action::OBJ_ROTATE);
}


void TW_CALL loadOBJModel(void *data)
{
	string str = getOpenFileName();

	// Exit the function if the string is empty (the user cancelled opening a file)
	if (str.empty()) return;

	// Clear objScene before reading from file
	objScene.clear();

	bool result = objScene.load_file(str);

	if(result)
	{
		std::cout << "The obj file was loaded successfully" << std::endl;
	}
	else
	{
		std::cerr << "Failed to load obj file" << std::endl;
	}

	std::cout << "The number of vertices in the model is: " << objScene.m_points.size() << std::endl;
	std::cout << "The number of triangles in the model is: " << objScene.m_faces.size() << std::endl;

	// Initialize the scene
	initScene();
}

void initScene() {
	// Store the material
	storeMaterial();

	// Import the new object
	mtx_main_renderer.lock();
	{
		meshModel_pending = MeshModel(objScene);
		is_meshModel_pending = true;
	}
	mtx_main_renderer.unlock();

	// Initialize the object
	initObject();
}


//do not change this function unless you really know what you are doing!
void initGraphics(GLFWwindow *&window)
{
	// Initialize GLFW
	assert_m(glfwInit(), "GLFW initialization failed");
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	assert_m(monitor, "GLFW get primary monitor failed");
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	assert_m(mode, "GLFW get video mode failed");
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	window = glfwCreateWindow(
		/*width*/ START_WIDTH, /*height*/ START_HEIGHT,
		/*title*/ "Model renderer using AntTweakBar and GLFW",
		/*monitor*/ NULL, // Windowed Mode
		/*share*/ NULL // Don't share resources with another window
	);
	assert_m(window, "GLFW create window failed");
	glfwMakeContextCurrent(window); // Needed for GLEW (and OpenGL?)

	// Initialize OpenGL
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.0, 0.0);

	// Initialize GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		assert(0);
		return;
	}
}


// Calls the appropriate method for the current homework
static inline void drawScene()
{
	Draw::drawScene(
		renderer_uVars,
		sVars,
		sVars.screen_buffers.currCalc()
	);
}


//this will make sure that integer coordinates are mapped exactly to corresponding pixels on screen
void glUseScreenCoordinates(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Render screen
void display()
{
	glClearColor(0, 0, 0, 1); //background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.drawScreenPixels(sVars.screen_buffers.currDraw());

	// Draw tweak bars
	TwDraw();

	//swap back and front frame buffers
	glfwSwapBuffers(window);
	++frame_number;
}


// Callback function called by GLFW when window size changes
void window_size_callback(GLFWwindow *window, int width, int height)
{
	glUseScreenCoordinates(width, height);

	// Update the screen dimensions
	sVars.resize_screen_pending(width, height);

	// Update the camera projections
	uVars->camera.aspect_ratio = sVars.screen.aspect_ratio();
	uVars->camera.update_projections();

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}



void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	TwEventMouseButtonGLFW3(window, button, action, mods);

	switch (action) {
	case GLFW_PRESS:
		mousePress.press(button);
		break;
	case GLFW_RELEASE:
		mousePress.release(button);
		break;
	}
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
	TwEventCursorPosGLFW3(window, xpos, ypos);

	mouse.update_pos(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	TwEventScrollGLFW3(window, xoffset, yoffset);
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW3(window, key, scancode, action, mods);

	switch (action) {
	case GLFW_PRESS:
		keyboardPress.press(key);
		break;
	case GLFW_RELEASE:
		keyboardPress.release(key);
		break;
	}
}

// Function called at exit
void terminate(void)
{ 
	TwTerminate();
	glfwTerminate();
}

// Perform the given action
void performAction(Action action, bool press) {
	switch (action) {
	default:
		// assert(0);
		break;
	case Action::LOAD_MESH_MODEL:
		if (press) loadOBJModel(NULL);
		break;
	case Action::CAM_LEFT:
		uVars->state[Action::CAM_LEFT] = press;
		break;
	case Action::CAM_RIGHT:
		uVars->state[Action::CAM_RIGHT] = press;
		break;
	case Action::CAM_FORWARD:
		uVars->state[Action::CAM_FORWARD] = press;
		break;
	case Action::CAM_BACKWARD:
		uVars->state[Action::CAM_BACKWARD] = press;
		break;
	case Action::CAM_UP:
		uVars->state[Action::CAM_UP] = press;
		break;
	case Action::CAM_DOWN:
		uVars->state[Action::CAM_DOWN] = press;
		break;
	case Action::OBJ_LEFT:
		uVars->state[Action::OBJ_LEFT] = press;
		break;
	case Action::OBJ_RIGHT:
		uVars->state[Action::OBJ_RIGHT] = press;
		break;
	case Action::OBJ_FORWARD:
		uVars->state[Action::OBJ_FORWARD] = press;
		break;
	case Action::OBJ_BACKWARD:
		uVars->state[Action::OBJ_BACKWARD] = press;
		break;
	case Action::OBJ_UP:
		uVars->state[Action::OBJ_UP] = press;
		break;
	case Action::OBJ_DOWN:
		uVars->state[Action::OBJ_DOWN] = press;
		break;
	case Action::MOUSE_LOOK:
		if (press) {
			if (!uVars->state[Action::MOUSE_LOOK]) { // Switch to mouse look mode
				mouse.disable(); // Make the cursor invisible
			}
			else { // Switch to normal cursor mode
				mouse.normal(); // Make the cursor visible
			}
			uVars->state[Action::MOUSE_LOOK].toggle();
			uVars->state[Action::OBJ_ROTATE] = false;
			uVars->state[Action::FPS_CAMERA] = false;
			//state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::BOUNDING_BOX:
		if (press) uVars->state[Action::BOUNDING_BOX].toggle();
		break;
	case Action::VERTEX_NORMALS:
		if (press) uVars->state[Action::VERTEX_NORMALS].toggle();
		break;
	case Action::WORLD_AXES:
		if (press) uVars->state[Action::WORLD_AXES].toggle();
		break;
	case Action::OBJECT_AXES:
		if (press) uVars->state[Action::OBJECT_AXES].toggle();
		break;
	case Action::OBJ_SCALE_INC:
		uVars->state[Action::OBJ_SCALE_INC] = press;
		break;
	case Action::OBJ_SCALE_DEC:
		uVars->state[Action::OBJ_SCALE_DEC] = press;
		break;
	case Action::OBJ_ROTATE:
		// mouse.normal(); // Make the cursor visible
		uVars->state[Action::MOUSE_LOOK] = false;
		uVars->state[Action::OBJ_ROTATE].toggle();
		uVars->state[Action::FPS_CAMERA] = false;
		break;
	case Action::PROJECTION_TOGGLE:
		if (press) uVars->camera.toggle();
		break;
	case Action::RESET_SCENE:
		if (press) reInitVariables();
		break;
	case Action::FPS_CAMERA:
		if (press) {
			if (!uVars->state[Action::FPS_CAMERA]) { // Switch to mouse look mode
				mouse.disable(); // Make the cursor invisible
				mouse.reset();
			}
			else { // Switch to normal cursor mode
				mouse.normal(); // Make the cursor visible
			}
			uVars->state[Action::MOUSE_LOOK] = false;
			uVars->state[Action::OBJ_ROTATE] = false;
			uVars->state[Action::FPS_CAMERA].toggle();
			//uVars->state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::VIEW_OBJECT:
		if (press) uVars->state[Action::VIEW_OBJECT].toggle();
		break;
	case Action::OBJ_ALT_MOVE:
		uVars->state[Action::OBJ_ALT_MOVE] = press;
		break;
	case Action::ESCAPE_ALL:
		if (press) {
			mouse.normal(); // Make the cursor visible
			uVars->state[Action::MOUSE_LOOK] = false;
			uVars->state[Action::FPS_CAMERA] = false;
			uVars->state[Action::OBJ_ROTATE] = false;
			uVars->state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::OBJ_CONTROL_MODEL:
		uVars->state[Action::OBJ_CONTROL_MODEL] = press;
	}
}

void update_motion(Motion &motion,
	Action left, Action right,
	Action forward, Action backward,
	Action up, Action down
) {
	if (uVars->state[left]) motion.go_left();
	else motion.stop_left();
	if (uVars->state[right]) motion.go_right();
	else motion.stop_right();
	if (uVars->state[forward]) motion.go_forward();
	else motion.stop_forward();
	if (uVars->state[backward]) motion.go_backward();
	else motion.stop_backward();
	if (uVars->state[up]) motion.go_up();
	else motion.stop_up();
	if (uVars->state[down]) motion.go_down();
	else motion.stop_down();
}

void control() {
	// Measure the time it takes to do the main control loop
	control_timer.start();

	// Check keys and perform actions
	while (keyboardPress.hasNext()) {
		KeyboardPress::KeyAndPress next = keyboardPress.next();
		int k = next.key;
		bool press = next.press;
		performAction(keyboardBind.atKey(k), press);
	}
	while (mousePress.hasNext()) {
		MousePress::KeyAndPress next = mousePress.next();
		int k = next.key;
		bool press = next.press;
		performAction(mouseBind.atKey(k), press);
	}

	// Update the camera motion
	update_motion(cam_motion,
		Action::CAM_LEFT, Action::CAM_RIGHT,
		Action::CAM_FORWARD, Action::CAM_BACKWARD,
		Action::CAM_UP, Action::CAM_DOWN
	);

	// Update the object motion
	update_motion(obj_motion,
		Action::OBJ_LEFT, Action::OBJ_RIGHT,
		Action::OBJ_FORWARD, Action::OBJ_BACKWARD,
		Action::OBJ_UP, Action::OBJ_DOWN
	);

	// Increase/Decrease the object scale
	if (uVars->state[Action::OBJ_SCALE_INC]) obj_scale_motion.go_right();
	else obj_scale_motion.stop_right();
	if (uVars->state[Action::OBJ_SCALE_DEC]) obj_scale_motion.go_left();
	else obj_scale_motion.stop_left();

	// Calculate the move vectors
	Vector4 cam_move = cam_motion.calcMove();
	Vector4 obj_move = obj_motion.calcMove();
	Vector4 obj_scale_vector = obj_scale_motion.calcMove();

	// Update the object scale
	Matrix4 obj_scale = Matrix4::iso_scaling(1.0 + obj_scale_vector[0]);
	//if (uVars->state[Action::OBJ_CONTROL_MODEL]) {
	//	uVars->object.model = object.model * obj_scale;
	//}
	//else {
	//	uVars->object.world = uVars->object.world * obj_scale;
	//}
	uVars->object.model = uVars->object.model * obj_scale;

	// Calculate the rotation matrices
	double u_angle = PI * -(double)mouse.move.y * mouse.sensitivity.y;
	Matrix4 rotY = Matrix4::rotationY(PI * -(double)mouse.move.x * mouse.sensitivity.x);
	Matrix4 rotX = Matrix4::rotationX(u_angle);
	Matrix4 rot = rotX * rotY;
	Matrix4 rotUW = Matrix4::rotation(uVars->camera.v ^ uVars->camera.w, uVars->camera.v, uVars->camera.w);

	// Calculate the camera current rotation
	if (uVars->state[Action::MOUSE_LOOK] || uVars->state[Action::FPS_CAMERA]) {
		uVars->camera.w = rotY * uVars->camera.w;
		uVars->camera.update_u_angle(-u_angle);

		if (uVars->state[Action::FPS_CAMERA]) { // FPS view
			uVars->camera.update_fps_rot();
		}
		else if (uVars->state[Action::MOUSE_LOOK]) { // Free view
			uVars->camera.rot = uVars->camera.rot * rotY * rotX;
		}
	}
	else if (uVars->state[Action::OBJ_ROTATE]) {
		if (uVars->state[Action::OBJ_CONTROL_MODEL]) {
			uVars->object.model = uVars->object.model * Matrix4::transpose(rot);
		}
		else {
			uVars->object.world = uVars->object.world * Matrix4::transpose(rot);
		}
		uVars->object.rot = rot * uVars->object.rot;
	}
	// Update the mouse rest position
	if (!uVars->state[Action::MOUSE_LOOK] && !uVars->state[Action::FPS_CAMERA]) mouse.update_rest(mouse.curr);
	mouse.reset();

	// Calculate the translation matrix
	Matrix4 cam_translate;
	if (uVars->state[Action::FPS_CAMERA]) {
		cam_translate = Matrix4::translation(rotUW * cam_move);
	}
	else {
		cam_translate = Matrix4::translation(uVars->camera.rot * cam_move);
	}
	uVars->camera.pos = cam_translate * uVars->camera.pos;

	// Update the object position
	Matrix4 obj_translate;
	if (!uVars->state[Action::OBJ_ALT_MOVE]) {
		obj_translate = Matrix4::translation(rotUW * obj_move);
	}
	else {
		obj_translate = Matrix4::translation(obj_move);
	}
	if (uVars->state[Action::OBJ_CONTROL_MODEL]) {
		uVars->object.model_pos = obj_translate * uVars->object.model_pos;
		uVars->object.model = obj_translate * uVars->object.model;
	}
	else {
		uVars->object.world_pos = obj_translate * uVars->object.world_pos;
		uVars->object.world = obj_translate * uVars->object.world;
	}

	// If "view object" is pressed, look at the object
	if (uVars->state[Action::VIEW_OBJECT]) uVars->camera.look_at(uVars->object.world * uVars->object.model_pos);

	// Update the camera inverse view transformation
	uVars->camera.update_inv_view();

	// Swap the current and previous key maps
	keyboardPress.swap();
	mousePress.swap();

	// Clear the lists
	keyboardPress.clear_list();
	mousePress.clear_list();

	// End the control time measure
	control_elapsed_us = static_cast<UINT32>(control_timer.get_elapsed_us());
}

void setLightMode(int light_num, LightingEnum &light_mode, const LightingEnum &new_light_mode) {
	light_mode = new_light_mode;
}
