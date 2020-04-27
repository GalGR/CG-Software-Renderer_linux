#include <iostream>

#include <AntTweakBar/TweakBar/include/AntTweakBar.h>
#include <Glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>
#include <plf_nanotimer/plf_nanotimer.h>

#include <thread>
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

#include "Draw.h"
#include "DrawBuffer.h"
#include "ScreenPixels.h"

#include "Vector4.h"
#include "Matrix4.h"

#include "Scene.h"

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
//typedef std::map<Action, TypeDefVal<bool, false>> ToggleMap;
ToggleMap<Action> state;

// State variables
size_t time_frame_ms = 16;
size_t frame_number = 0;
Mouse mouse;


// Lighting mode enum
typedef enum { LIGHT_POINT, LIGHT_DIR, NUM_OF_LIGHTING_MODES } LightingEnum;

// Lighting mode
LightingEnum light1_mode;
LightingEnum light2_mode;

TwEnumVal lightingEnumString[] = { {LIGHT_POINT, "Point"}, {LIGHT_DIR, "Directional"} };
TwType lightingTwType;

// Shading mode
ShadingEnum shading_mode;

TwEnumVal shadingEnumString[] = { {SHADING_WIRE, "Wireframe"}, {SHADING_FLAT, "Flat"}, {SHADING_GOURAUD, "Gouraud"}, {SHADING_PHONG, "Phong"}, {SHADING_FLAT_TEST, "Flat Test"}, {SHADING_GOURAUD_TEST, "Gouraud Test"} };
TwType shadingTwType;

// AntTweakBar
TwBar* bar;

// GLFW window
GLFWwindow *window = NULL;

// Screen dimensions
ScreenState screen = { START_WIDTH, START_HEIGHT };
//size_t g_width = START_WIDTH;
//size_t g_height = START_HEIGHT;

// Render timer
plf::nanotimer render_timer;
// Control loop timer
plf::nanotimer control_timer;
// Elapsed render time
UINT32 render_elapsed_us;
// Elapsed control loop time
UINT32 control_elapsed_us;

// Color
Color color = WHITE;

//obj data type
Wavefront_obj objScene;

// Scene
Scene scene;

// Object
Object &object = scene.object;

// Camera
Camera &camera = scene.camera;

// Lighting
Lighting &lighting = scene.lighting;

// Mesh draw buffers
DrawBuffer mesh_buffer;

// Bounding Box draw buffers
DrawBuffer bbox_buffer;

// Normals draw buffers
DrawBuffer normals_buffer;

// World/Object axes draw buffers
DrawBuffer axes_buffer;

// Pixels screen buffer
ScreenPixels pixels;

// Camera motion
Motion cam_motion;

// Object motion
Motion obj_motion;

// Object scale
Motion obj_scale_motion; // Only use the x coordinate

// Colors
Color obj_color = WHITE;
Color bbox_color = BLUE;
Color normals_color = YELLOW;

// Material
Material material;

void assert_m(const bool expr, const char *err) {
	if (!expr) {
		std::cerr << err << std::endl;
		exit(1);
	}
}

void TW_CALL loadOBJModel(void* clientData);
void initScene();
void initGraphics(int argc, char *argv[]);
static inline void drawScene();
void display();
void window_size_callback(int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void PassiveMouseMotion(int x, int y);
void KeyboardDown(unsigned char k, int x, int y);
void SpecialDown(int k, int x, int y);
void terminate(void);

void KeyboardUp(unsigned char k, int x, int y);
void SpecialUp(int k, int x, int y);

void initCallbacks();
void initTweakBar();
void initMaterial();
void storeMaterial();
void loadMaterial();
void initObject();
void initVariables();
void initKeybindings();
void control_loop(int value);
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

int main(int argc, char *argv[])
{
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
	initGraphics(argc, argv);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Initialize the times
	render_elapsed_us = 0;
	control_elapsed_us = 0;

	// Set GLFW callbacks
	initCallbacks();
	glutDisplayFunc(display);

	glutTimerFunc(time_frame_ms, control_loop, frame_number); // Timer callback function

	// Teminates AntTweakBar and GLFW on program exit
	atexit(terminate);

	// Initialize all the other variables
	initVariables();

	// Create a tweak bar
	initTweakBar();

	// Reserve the bounding box buffers
	bbox_buffer.clear_reserve_resize(BOUNDING_BOX_VERTICES);

	// Reserve the world axes buffers
	axes_buffer.clear_reserve_resize(WORLD_AXES_VERTICES);


	// GLFW main loop
	while (glfwWindowShouldClose(window)) {

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
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
		" GLOBAL help='Press WASD/Space/Shift to move."
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
	TwAddVarRW(bar, "Shading", shadingTwType, &shading_mode, " help'The shading mode' ");

	TwAddSeparator(bar, NULL, NULL);

	// Define the color bars
	TwAddVarRW(bar, "Object Color", TW_TYPE_COLOR32, &obj_color, " help='The objects color' ");
	TwAddVarRW(bar, "Box Color", TW_TYPE_COLOR32, &bbox_color, " help='The objects bounding box color' ");
	TwAddVarRW(bar, "Normals Color", TW_TYPE_COLOR32, &normals_color, " help='The objects normals color' ");
	TwAddVarRW(bar, "Normals Length", TW_TYPE_DOUBLE, &object.normals_length, " step=0.01, min=-10.0, max=10.0, help='The objects normals length' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarRW(bar, "Ambient Coefficient", TW_TYPE_COLOR32, &object.material.k_ambient, " help='The ambient coefficient' ");
	TwAddVarRW(bar, "Diffuse Coefficient", TW_TYPE_COLOR32, &object.material.k_diffuse, " help='The diffuse coefficient' ");
	TwAddVarRW(bar, "Specular Coefficient", TW_TYPE_COLOR32, &object.material.k_specular, " help='The specular coefficient' ");
	TwAddVarRW(bar, "Specular Power", TW_TYPE_DOUBLE, &object.material.n_specular, " min=1.0, help='The specular power' ");

	TwAddSeparator(bar, NULL, NULL);

	lightingTwType = TwDefineEnum("LightingMode", lightingEnumString, NUM_OF_LIGHTING_MODES); // Define the lighting type
	TwAddVarCB(bar, "Light 1 Mode", lightingTwType, setLight1Mode, getLight1Mode, NULL, " help'Light 1 type' ");
	TwAddVarCB(bar, "Light 2 Mode", lightingTwType, setLight2Mode, getLight2Mode, NULL, " help'Light 2 type' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarRW(bar, "Ambient Light Intensity", TW_TYPE_DOUBLE, &lighting[0].getIntensity(), " min=0.0, max=1.0, step=0.01, help='The ambient light's intensity' ");
	TwAddVarRW(bar, "Light 1 Intensity", TW_TYPE_DOUBLE, &lighting[1].getIntensity(), " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");
	TwAddVarRW(bar, "Light 2 Intensity", TW_TYPE_DOUBLE, &lighting[2].getIntensity(), " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarRW(bar, "Light 1 Position/Direction", TW_TYPE_DIR3D, &lighting[1].getVector(), " help='Light 1's position/direction' ");
	TwAddVarRW(bar, "Light 2 Position/Direction", TW_TYPE_DIR3D, &lighting[2].getVector(), " help='Light 1's position/direction' ");

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
	material.k_ambient = K_AMBIENT;
	material.k_diffuse = K_DIFFUSE;
	material.k_specular = K_SPECULAR;
	material.n_specular = N_SPECULAR;
}

void storeMaterial() {
	material = object.material;
}

void loadMaterial() {
	object.material = material;
}

void initObject() {
	object.model = Matrix4::I();
	object.world = Matrix4::I();
	object.world_pos = Vector4(0.0, 0.0, 0.0);
	object.model_pos = Vector4(0.0, 0.0, 0.0);
	object.rot = Matrix4::I();
	loadMaterial();
}

void initVariables() {
	// Add light sources
	if (lighting.size() < 3) {
		lighting.push_back(AmbientLight(AMBIENT_LIGHT_INTENSITY));
		lighting.push_back(PointLight(POINT_LIGHT1_POS, LIGHT1_INTENSITY));
		lighting.push_back(PointLight(POINT_LIGHT2_POS, LIGHT2_INTENSITY));
	}

	// Reserve the number of pixels
	pixels.resize(screen.x, screen.y);

	// Initialize the camera motion
	cam_motion = Motion(GO_ACCEL, STOP_ACCEL, MAX_ACCEL, MAX_VELOC);
	// Initialize the object motion
	obj_motion = Motion(GO_ACCEL, STOP_ACCEL, MAX_ACCEL, MAX_VELOC);
	// Initialize the object scale (implemented with motion)
	obj_scale_motion = Motion(0.01, -0.1, 0.5, 0.05);

	// Initialize camera
	camera.inv_view = Matrix4::inv_translation(CAMERA_X, CAMERA_Y, CAMERA_Z); // Test Camera
	camera.pos = Vector4(CAMERA_X, CAMERA_Y, CAMERA_Z);
	camera.v = Vector4(0.0, 1.0, 0.0);
	camera.w = Vector4(0.0, 0.0, 1.0);
	camera.u_angle = 0.0;
	camera.rot = Matrix4::I();

	// Initialize the lighting
	light1_mode = INITIAL_LIGHT1_MODE;
	light2_mode = INITIAL_LIGHT2_MODE;

	// Initialize the shading mode
	shading_mode = INITIAL_SHADING_MODE;

	// Initialize the material
	initMaterial();

	// Initialize Object
	initObject();

	// Initialize projection matrix
	camera.n = CAMERA_N;
	camera.f = CAMERA_F;
	camera.fovy = CAMERA_FOVY;
	camera.aspect_ratio = screen.aspect_ratio();
	camera.ortho_fov_scale = CAMERA_ORTHO_FOV_SCALE;
	camera.update_projections();
	camera.proj_type = ProjectionType::PERSPECTIVE;

	// Initialize the mouse
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mouse = Mouse(window, xpos, ypos);
	mouse.normal(); // Make the cursor visible

	// Initialize the actions
	state[Action::BOUNDING_BOX] = false;
	state[Action::VERTEX_NORMALS] = false;
	state[Action::WORLD_AXES] = true;
	state[Action::MOUSE_LOOK] = false;
	state[Action::FPS_CAMERA] = false;
	state[Action::OBJ_ROTATE] = false;
	state[Action::VIEW_OBJECT] = false;
	state[Action::OBJ_CONTROL_MODEL] = false;
}

void initKeybindings() {
	// Bind keyboard
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
	object = Object(objScene);

	size_t numPoints = object.meshModel().vertices.size();
	size_t numVertexNormals = numPoints;

	// Reserve the mesh buffers
	mesh_buffer.clear_reserve_resize(numPoints);

	// Reserve the normals buffers
	normals_buffer.clear_reserve_resize(numVertexNormals);

	// Initialize the object
	initObject();
}


//do not change this function unless you really know what you are doing!
void initGraphics(int argc, char *argv[], GLFWwindow *&window)
{
	// Initialize GLFW
	assert_m(glfwInit(), "GLFW initialization failed");
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	assert_m(!monitor, "GLFW get primary monitor failed");
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	assert_m(!mode, "GLFW get video mode failed");
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	window = glfwCreateWindow(
		/*width*/ START_WIDTH, /*height*/ START_HEIGHT,
		/*title*/ "Model renderer using AntTweakBar and GLFW",
		/*monitor*/ NULL, // Windowed Mode
		/*share*/ NULL // Don't share resources with another window
	);
	assert_m(!window, "GLFW create window failed");
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
		camera,
		object,
		lighting,
		shading_mode,
		mesh_buffer,
		bbox_buffer,
		normals_buffer,
		axes_buffer,
		pixels,
		screen,
		state[Action::BOUNDING_BOX],
		state[Action::VERTEX_NORMALS],
		state[Action::WORLD_AXES],
		state[Action::OBJECT_AXES],
		obj_color,
		bbox_color,
		normals_color
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


// Callback function called by GLUT to render screen
void display()
{
//  	static int counter = 0;
//  	std::cout << "C: " << counter << std::endl;
//  	counter++;

	glClearColor(0, 0, 0, 1); //background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//time measuring - don't delete
	render_timer.start();

	drawScene();

	//time measuring - don't delete
	render_elapsed_us = static_cast<UINT32>(render_timer.get_elapsed_us());

	// Draw tweak bars
	TwDraw();

	//swap back and front frame buffers
	glutSwapBuffers();
	++frame_number;
}


// Callback function called by GLUT when window size changes
void window_size_callback(int width, int height)
{
	glUseScreenCoordinates(width, height);

	// Update the screen dimensions
	screen.x = width;
	screen.y = height;

	// Update the screen pixels buffer
	pixels.resize(screen.x, screen.y);

	// Update the mouse rest position
	mouse.update_rest(screen.mid_point());

	// Update the camera projections
	camera.aspect_ratio = screen.aspect_ratio();
	camera.update_projections();

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

void KeyboardDown(unsigned char k, int x, int y)
{
	TwEventKeyboardGLUT(k, x, y);

	// Lower case the character
	k = tolower(k);
	
	keyAlpPress.press(k);

	//glutPostRedisplay();
}

void KeyboardUp(unsigned char k, int x, int y)
{
	TwEventKeyboardGLUT(k, x, y);

	// Lower case the character
	k = tolower(k);

	keyAlpPress.release(k);

	//glutPostRedisplay();
}

void SpecialDown(int k, int x, int y)
{
	TwEventKeyboardGLUT(k, x, y);

	keySplPress.press(k);

	//glutPostRedisplay();
}

void SpecialUp(int k, int x, int y)
{
	TwEventKeyboardGLUT(k, x, y);

	keySplPress.release(k);

	//glutPostRedisplay();
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
		assert(0);
		break;
	case Action::CAM_LEFT:
		state[Action::CAM_LEFT] = press;
		break;
	case Action::CAM_RIGHT:
		state[Action::CAM_RIGHT] = press;
		break;
	case Action::CAM_FORWARD:
		state[Action::CAM_FORWARD] = press;
		break;
	case Action::CAM_BACKWARD:
		state[Action::CAM_BACKWARD] = press;
		break;
	case Action::CAM_UP:
		state[Action::CAM_UP] = press;
		break;
	case Action::CAM_DOWN:
		state[Action::CAM_DOWN] = press;
		break;
	case Action::OBJ_LEFT:
		state[Action::OBJ_LEFT] = press;
		break;
	case Action::OBJ_RIGHT:
		state[Action::OBJ_RIGHT] = press;
		break;
	case Action::OBJ_FORWARD:
		state[Action::OBJ_FORWARD] = press;
		break;
	case Action::OBJ_BACKWARD:
		state[Action::OBJ_BACKWARD] = press;
		break;
	case Action::OBJ_UP:
		state[Action::OBJ_UP] = press;
		break;
	case Action::OBJ_DOWN:
		state[Action::OBJ_DOWN] = press;
		break;
	case Action::MOUSE_LOOK:
		if (press) {
			if (!state[Action::MOUSE_LOOK]) { // Switch to mouse look mode
				mouse.disable(); // Make the cursor invisible
			}
			else { // Switch to normal cursor mode
				mouse.normal(); // Make the cursor visible
			}
			state[Action::MOUSE_LOOK].toggle();
			state[Action::OBJ_ROTATE] = false;
			state[Action::FPS_CAMERA] = false;
			//state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::BOUNDING_BOX:
		if (press) state[Action::BOUNDING_BOX].toggle();
		break;
	case Action::VERTEX_NORMALS:
		if (press) state[Action::VERTEX_NORMALS].toggle();
		break;
	case Action::WORLD_AXES:
		if (press) state[Action::WORLD_AXES].toggle();
		break;
	case Action::OBJECT_AXES:
		if (press) state[Action::OBJECT_AXES].toggle();
		break;
	case Action::OBJ_SCALE_INC:
		state[Action::OBJ_SCALE_INC] = press;
		break;
	case Action::OBJ_SCALE_DEC:
		state[Action::OBJ_SCALE_DEC] = press;
		break;
	case Action::OBJ_ROTATE:
		// mouse.normal(); // Make the cursor visible
		state[Action::MOUSE_LOOK] = false;
		state[Action::OBJ_ROTATE].toggle();
		state[Action::FPS_CAMERA] = false;
		break;
	case Action::PROJECTION_TOGGLE:
		if (press) camera.toggle();
		break;
	case Action::RESET_SCENE:
		if (press) initVariables();
		break;
	case Action::FPS_CAMERA:
		if (press) {
			if (!state[Action::FPS_CAMERA]) { // Switch to mouse look mode
				mouse.disable(); // Make the cursor invisible
				mouse.reset();
			}
			else { // Switch to normal cursor mode
				mouse.normal(); // Make the cursor visible
			}
			mouse.update_rest(screen.mid_point());
			state[Action::MOUSE_LOOK] = false;
			state[Action::OBJ_ROTATE] = false;
			state[Action::FPS_CAMERA].toggle();
			//state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::VIEW_OBJECT:
		if (press) state[Action::VIEW_OBJECT].toggle();
		break;
	case Action::OBJ_ALT_MOVE:
		state[Action::OBJ_ALT_MOVE] = press;
		break;
	case Action::ESCAPE_ALL:
		if (press) {
			mouse.normal(); // Make the cursor visible
			state[Action::MOUSE_LOOK] = false;
			state[Action::FPS_CAMERA] = false;
			state[Action::OBJ_ROTATE] = false;
			state[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::OBJ_CONTROL_MODEL:
		state[Action::OBJ_CONTROL_MODEL] = press;
	}
}

void update_motion(Motion &motion,
	Action left, Action right,
	Action forward, Action backward,
	Action up, Action down
) {
	if (state[left]) motion.go_left();
	else motion.stop_left();
	if (state[right]) motion.go_right();
	else motion.stop_right();
	if (state[forward]) motion.go_forward();
	else motion.stop_forward();
	if (state[backward]) motion.go_backward();
	else motion.stop_backward();
	if (state[up]) motion.go_up();
	else motion.stop_up();
	if (state[down]) motion.go_down();
	else motion.stop_down();
}

void control_loop(int value) {
	// Measure the time it takes to do the main control loop
	control_timer.start();

	if (frame_number == (unsigned int)value) { // No frame was rendered at that time frame
		glutTimerFunc(time_frame_ms, control_loop, value); // Register the timer callback again -- with the same frame number
		return;
	}

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
	if (state[Action::OBJ_SCALE_INC]) obj_scale_motion.go_right();
	else obj_scale_motion.stop_right();
	if (state[Action::OBJ_SCALE_DEC]) obj_scale_motion.go_left();
	else obj_scale_motion.stop_left();

	// Calculate the move vectors
	Vector4 cam_move = cam_motion.calcMove();
	Vector4 obj_move = obj_motion.calcMove();
	Vector4 obj_scale_vector = obj_scale_motion.calcMove();

	// Update the object scale
	Matrix4 obj_scale = Matrix4::iso_scaling(1.0 + obj_scale_vector[0]);
	//if (state[Action::OBJ_CONTROL_MODEL]) {
	//	object.model = object.model * obj_scale;
	//}
	//else {
	//	object.world = object.world * obj_scale;
	//}
	object.model = object.model * obj_scale;

	// Calculate the rotation matrices
	double u_angle = PI * -(double)mouse.move.y * mouse.sensitivity.y;
	Matrix4 rotY = Matrix4::rotationY(PI * -(double)mouse.move.x * mouse.sensitivity.x);
	Matrix4 rotX = Matrix4::rotationX(u_angle);
	Matrix4 rot = rotX * rotY;
	Matrix4 rotUW = Matrix4::rotation(camera.v ^ camera.w, camera.v, camera.w);

	// Calculate the camera current rotation
	if (state[Action::MOUSE_LOOK] || state[Action::FPS_CAMERA]) {
		camera.w = rotY * camera.w;
		camera.update_u_angle(-u_angle);

		if (state[Action::FPS_CAMERA]) { // FPS view
			camera.update_fps_rot();
		}
		else if (state[Action::MOUSE_LOOK]) { // Free view
			camera.rot = camera.rot * rotY * rotX;
		}

		// Reset the mouse position to the middle of the screen
		glutWarpPointer(screen.mid_x_int(), screen.mid_y_int()); // Set the cursor to the middle of the screen
	}
	else if (state[Action::OBJ_ROTATE]) {
		if (state[Action::OBJ_CONTROL_MODEL]) {
			object.model = object.model * Matrix4::transpose(rot);
		}
		else {
			object.world = object.world * Matrix4::transpose(rot);
		}
		object.rot = rot * object.rot;
	}
	// Update the mouse rest position
	if (!state[Action::MOUSE_LOOK] && !state[Action::FPS_CAMERA]) mouse.update_rest(mouse.curr);
	mouse.reset();

	// Calculate the translation matrix
	Matrix4 cam_translate;
	if (state[Action::FPS_CAMERA]) {
		cam_translate = Matrix4::translation(rotUW * cam_move);
	}
	else {
		cam_translate = Matrix4::translation(camera.rot * cam_move);
	}
	camera.pos = cam_translate * camera.pos;

	// Update the object position
	Matrix4 obj_translate;
	if (!state[Action::OBJ_ALT_MOVE]) {
		obj_translate = Matrix4::translation(rotUW * obj_move);
	}
	else {
		obj_translate = Matrix4::translation(obj_move);
	}
	if (state[Action::OBJ_CONTROL_MODEL]) {
		object.model_pos = obj_translate * object.model_pos;
		object.model = obj_translate * object.model;
	}
	else {
		object.world_pos = obj_translate * object.world_pos;
		object.world = obj_translate * object.world;
	}

	// If "view object" is pressed, look at the object
	if (state[Action::VIEW_OBJECT]) camera.look_at(object.world * object.model_pos);

	// Update the camera inverse view transformation
	camera.update_inv_view();

	// Swap the current and previous key maps
	keyAlpPress.swap();
	keySplPress.swap();
	keyModPress.swap();

	// Clear the lists
	keyAlpPress.clear_list();
	keySplPress.clear_list();
	keyModPress.clear_list();

	// End the control time measure
	control_elapsed_us = static_cast<UINT32>(control_timer.get_elapsed_us());

	glutTimerFunc(time_frame_ms, control_loop, frame_number); // Register the timer callback again
	TwRefreshBar(bar);
	glutPostRedisplay();
}

void setLightMode(int light_num, LightingEnum &light_mode, const LightingEnum &new_light_mode) {
	if (light_mode == new_light_mode) return;
	switch (light_mode) {
	default:
		assert(0);
		break;
	case LIGHT_POINT:
	{
		light_mode = LIGHT_DIR;
		lighting[light_num].setCalc<PointLight>();
		break;
	}
	case LIGHT_DIR:
	{
		light_mode = LIGHT_POINT;
		lighting[light_num].setCalc<DirectionalLight>();
		break;
	}
	}
}
