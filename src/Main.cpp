#include <iostream>

#include <AntTweakBar/include/AntTweakBar.h>
#include <Glew/include/GL/glew.h>
#include <freeglut/include/GL/freeglut.h>
#include <plf_nanotimer/plf_nanotimer.h>

#include <vector>
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
// #include "MeasureTime.h"
#endif
#include <assert.h>
#include <iostream>

#include "Utils.h"
#include "Renderer.h"
#include "Obj_Parser/wavefront_obj.h"

#include "osstring.h"

// My include:

// #define SCREEN_PIXELS_BUFFER_IMPLEMENT

#include <map>
#include <tuple>

#include "Draw.h"
#include "ScreenPixels.h"

#include "Vector4.h"
#include "Matrix4.h"

#include "Scene.h"

#include "Object.h"
#include "Camera.h"
#include "Light.h"

#include "static_vector.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Motion.h"
#include "Screen.h"
#include "ActionsDone.h"
#include "PI.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

bool key_is_pressed(KeySym ks) {
	Display *dpy = XOpenDisplay(NULL);
	char keys_return[32];
	XQueryKeymap(dpy, keys_return);
	KeyCode kc2 = XKeysymToKeycode(dpy, ks);
	bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
	XCloseDisplay(dpy);
	return isPressed;
}

enum VK_Keys {
	VK_SHIFT = 0x10,
	VK_CONTROL = 0x11,
	VK_MENU = 0x12,
};

int GetKeyState(VK_Keys vk_key) {
	KeySym keysym_l;
	KeySym keysym_r;
	switch (vk_key) {
	case VK_SHIFT:
		keysym_l = XK_Shift_L;
		keysym_r = XK_Shift_R;
		break;
	case VK_CONTROL:
		keysym_l = XK_Control_L;
		keysym_r = XK_Control_R;
		break;
	case VK_MENU:
		keysym_l = XK_Alt_L;
		keysym_r = XK_Alt_R;
		break;
	}
	bool is_pressed_l = key_is_pressed(keysym_l);
	bool is_pressed_r = key_is_pressed(keysym_r);
	if (is_pressed_l || is_pressed_r) return 0x8000;
	return 0x0;
}
#endif

// My macros:
#define START_WIDTH 960
#define START_HEIGHT 640
static inline int in_range(int p, int lo, int hi) {
	return ((p = ((p > hi) ? hi : p)) < lo ? lo : p);
}
#define MAX_ACCEL 1.0
#define MAX_VELOC 0.8
#define GO_ACCEL 0.01
#define STOP_ACCEL -0.08
#define WORLD_AXES_VERTICES 4
#define START_PIXELS_WIDTH 8'000
#define START_PIXELS_HEIGHT 8'000
#define START_PIXELS (START_PIXELS_WIDTH * START_PIXELS_HEIGHT)
#define CAMERA_N 0.001
#define CAMERA_F 10'000.0
#define CAMERA_FOVY 60.0
#define CAMERA_ORTHO_FOV_SCALE 0.11477777777
#define CAMERA_X 0.0
#define CAMERA_Y 0.0
#define CAMERA_Z 20.0

//#define AMBIENT_LIGHT_INTENSITY 0.5
//#define LIGHT1_INTENSITY 0.7
//#define LIGHT2_INTENSITY 0.3
#define AMBIENT_LIGHT_INTENSITY 0.05
#define LIGHT1_INTENSITY 0.85
#define LIGHT2_INTENSITY 0.10

//#define POINT_LIGHT1_POS Vector4(1.0, 1.0, 1.0)
//#define POINT_LIGHT2_POS Vector4(1.0, 2.0, 2.0)
#define POINT_LIGHT1_POS Vector4(10.0, 0.0, 30.0)
#define POINT_LIGHT2_POS Vector4(-15.0, 20.0, -35.0)

//#define K_AMBIENT 0xff880088
//#define K_DIFFUSE 0xff33aa11
//#define K_SPECULAR 0xff808080
//#define N_SPECULAR 8
#define K_AMBIENT 0xff'a0'60'60
#define K_DIFFUSE 0xff'33'aa'11
#define K_SPECULAR 0xff'60'60'60
#define N_SPECULAR 8

#define INITIAL_LIGHTING_MODE LIGHT_POINT
#define INITIAL_LIGHT1_MODE INITIAL_LIGHTING_MODE
#define INITIAL_LIGHT2_MODE INITIAL_LIGHTING_MODE
#define INITIAL_SHADING_MODE SHADING_GOURAUD

enum class ModifierKey {
	NONE,
	SHIFT,
	CTRL,
	ALT
};

// Default keybindings
enum DefaultKeybind {
	DEFAULT_MOUSE_LOOK = 'f',
	DEFAULT_CAM_LEFT = 'a',
	DEFAULT_CAM_RIGHT = 'd',
	DEFAULT_CAM_FORWARD = 'w',
	DEFAULT_CAM_BACKWARD = 's',
	DEFAULT_CAM_UP = ' ',
	DEFAULT_CAM_DOWN = (int)ModifierKey::SHIFT,
	DEFAULT_BOUNDING_BOX = 'b',
	DEFAULT_VERTEX_NORMALS = 'n',
	DEFAULT_WORLD_AXES = 'x',
	DEFAULT_RESET_SCENE = 'r',
	DEFAULT_OBJECT_AXES = 'u',
	DEFAULT_VIEW_OBJECT = 'v',
	DEFAULT_OBJ_LEFT = GLUT_KEY_LEFT,
	DEFAULT_OBJ_RIGHT = GLUT_KEY_RIGHT,
	DEFAULT_OBJ_FORWARD = GLUT_KEY_UP,
	DEFAULT_OBJ_BACKWARD = GLUT_KEY_DOWN,
	DEFAULT_OBJ_UP = GLUT_KEY_PAGE_UP,
	DEFAULT_OBJ_DOWN = GLUT_KEY_PAGE_DOWN,
	DEFAULT_OBJ_SCALE_INC = '+',
	DEFAULT_OBJ_SCALE_DEC = '-',
	DEFAULT_OBJ_ROTATE = GLUT_RIGHT_BUTTON,
	DEFAULT_PROJECTION_TOGGLE = 'p',
	DEFAULT_FPS_CAMERA = 'g',
	DEFAULT_OBJ_ALT_MOVE = (int)ModifierKey::ALT,
	DEFAULT_ESCAPE_ALL = 27, // Escape ASCII code
	DEFAULT_OBJ_CONTROL_MODEL = (int)ModifierKey::CTRL,
};

// Action enums
enum class Action {
	NONE = -1,
	MOUSE_LOOK,
	CAM_LEFT,
	CAM_RIGHT,
	CAM_FORWARD,
	CAM_BACKWARD,
	CAM_UP,
	CAM_DOWN,
	BOUNDING_BOX,
	FACE_NORMALS,
	VERTEX_NORMALS,
	WORLD_AXES,
	RESET_SCENE,
	OBJECT_AXES,
	VIEW_OBJECT,
	OBJ_LEFT,
	OBJ_RIGHT,
	OBJ_FORWARD,
	OBJ_BACKWARD,
	OBJ_UP,
	OBJ_DOWN,
	OBJ_SCALE_INC,
	OBJ_SCALE_DEC,
	OBJ_ROTATE,
	PROJECTION_TOGGLE,
	FPS_CAMERA,
	OBJ_ALT_MOVE,
	ESCAPE_ALL,
	OBJ_CONTROL_MODEL,
	TOTAL_NUMBER_OF_ACTIONS
	// An enum to count the number of actions (assuming they start from 0 and don't jump randomly)
};


// Keyboard-Action variables
typedef KeyboardPress<unsigned char> KeyAlpPress;
typedef KeyboardPress<int> KeySplPress;
typedef KeyboardPress<ModifierKey> KeyModPress;
typedef KeyboardPress<int> MouseBttnPress;
typedef KeyboardBind<unsigned char, Action, 0, Action::NONE> KeyAlpBind;
typedef KeyboardBind<int, Action, 0, Action::NONE> KeySplBind;
typedef KeyboardBind<ModifierKey, Action, ModifierKey::NONE, Action::NONE> KeyModBind;
typedef KeyboardBind<int, Action, 0, Action::NONE> MouseBttnBind;
KeyAlpPress keyAlpPress;
KeySplPress keySplPress;
KeyModPress keyModPress;
MouseBttnPress mouseBttnPress;
KeyAlpBind keyAlpBind;
KeySplBind keySplBind;
KeyModBind keyModBind;
MouseBttnBind mouseBttnBind;
//typedef std::map<Action, TypeDefVal<bool, false>> ActionsDone;
ActionsDone<Action> actions_done;

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

// Screen dimensions
MyScreen screen = { START_WIDTH, START_HEIGHT };
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

double g_Scale = 1.0;
//double g_quaternion[4] = {0.0, 0.0, 0.0, 1.0};

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
AmbientLight ambient_light;
PointLight point_light1;
PointLight point_light2;
DirectionalLight directional_light1;
DirectionalLight directional_light2;
Lighting &lighting = scene.lighting;

// Mesh draw buffers
DrawBuffers mesh_buffers;

// Bounding Box draw buffers
DrawBuffers bbox_buffers;

// Normals draw buffers
DrawBuffers normals_buffers;

// World/Object axes draw buffers
DrawBuffers axes_buffers;

// Pixels array
std::vector<Pixel> list_pixels(START_PIXELS);

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

void TW_CALL loadOBJModel(void* clientData);
void initScene();
void initGraphics(int argc, char *argv[]);
static inline void drawScene();
void display();
void Reshape(int width, int height);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void PassiveMouseMotion(int x, int y);
void KeyboardDown(unsigned char k, int x, int y);
void SpecialDown(int k, int x, int y);
void Terminate(void);

void KeyboardUp(unsigned char k, int x, int y);
void SpecialUp(int k, int x, int y);

void initMaterial();
void storeMaterial();
void loadMaterial();
void initObject();
void initVariables();
void initKeybindings();
void Timer(int value); // Timer callback function
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

void TW_CALL setLight1Intensity(const void *value, void *clientData) { lighting.light1->intensity = *(double*)value; }
void TW_CALL setLight2Intensity(const void *value, void *clientData) { lighting.light2->intensity = *(double*)value; }
void TW_CALL getLight1Intensity(void *value, void *clientData) { *(double*)value = lighting.light1->intensity; }
void TW_CALL getLight2Intensity(void *value, void *clientData) { *(double*)value = lighting.light2->intensity; }

void TW_CALL setLight1PosDir(const void *value, void *clientData) { *((std::array<double, 3>*)(&((PointLight*)lighting.light1)->pos)) = *(std::array<double, 3>*)value; }
void TW_CALL setLight2PosDir(const void *value, void *clientData) { *((std::array<double, 3>*)(&((PointLight*)lighting.light2)->pos)) = *(std::array<double, 3>*)value; }
void TW_CALL getLight1PosDir(void *value, void *clientData) { *(std::array<double, 3>*)value = *((std::array<double, 3>*)(&((PointLight*)lighting.light1)->pos)); }
void TW_CALL getLight2PosDir(void *value, void *clientData) { *(std::array<double, 3>*)value = *((std::array<double, 3>*)(&((PointLight*)lighting.light2)->pos)); }


int main(int argc, char *argv[])
{
	// Initialize the keybindings
	initKeybindings();

	// Initialize openGL, glut, glew
	initGraphics(argc, argv);
	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);
	// Initialize the times
	render_elapsed_us = 0;
	control_elapsed_us = 0;
	// Set GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(PassiveMouseMotion);
	glutKeyboardFunc(KeyboardDown);
	glutSpecialFunc(SpecialDown);

	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialDown);
	glutSpecialUpFunc(SpecialUp);

	glutIgnoreKeyRepeat(true);
	glutTimerFunc(time_frame_ms, Timer, frame_number); // Timer callback function
	
	//send 'glutGetModifers' function pointer to AntTweakBar.
	//required because the GLUT key event functions do not report key modifiers states.
	//TwGLUTModifiersFunc(glutGetModifiers);


	atexit(Terminate);  //called after glutMainLoop ends


	// Initialize all the other variables
	initVariables();


	// Create a tweak bar
	//TwBar* bar = TwNewBar("TweakBar");
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

	TwAddVarRW(bar, "Ambient Light Intensity", TW_TYPE_DOUBLE, &lighting.ambient_light->intensity, " min=0.0, max=1.0, step=0.01, help='The ambient light's intensity' ");
	//TwAddVarRW(bar, "Light 1 Intensity", TW_TYPE_DOUBLE, &lighting.light1->intensity, " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");
	//TwAddVarRW(bar, "Light 2 Intensity", TW_TYPE_DOUBLE, &lighting.light2->intensity, " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");
	TwAddVarCB(bar, "Light 1 Intensity", TW_TYPE_DOUBLE, setLight1Intensity, getLight1Intensity, NULL, " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");
	TwAddVarCB(bar, "Light 2 Intensity", TW_TYPE_DOUBLE, setLight2Intensity, getLight2Intensity, NULL, " min=0.0, max=1.0, step=0.01, help='Light 1's intensity' ");

	TwAddSeparator(bar, NULL, NULL);

	//TwAddVarRW(bar, "Light 1 Position/Direction", TW_TYPE_DIR3D, &((PointLight*)lighting.light1)->pos, " help='Light 1's position/direction' ");
	//TwAddVarRW(bar, "Light 2 Position/Direction", TW_TYPE_DIR3D, &((PointLight*)lighting.light2)->pos, " help='Light 1's position/direction' ");
	TwAddVarCB(bar, "Light 1 Position/Direction", TW_TYPE_DIR3D, setLight1PosDir, getLight1PosDir, NULL, " help='Light 1's position/direction' ");
	TwAddVarCB(bar, "Light 2 Position/Direction", TW_TYPE_DIR3D, setLight2PosDir, getLight2PosDir, NULL, " help='Light 1's position/direction' ");

	TwAddSeparator(bar, NULL, NULL);

	TwAddVarRO(bar, "Render (us)", TW_TYPE_UINT32, &render_elapsed_us, " help='Shows the drawing time in micro seconds' ");
	TwAddVarRO(bar, "Control (us)", TW_TYPE_UINT32, &control_elapsed_us, " help='Shows the main control function time in micro seconds' ");

	TwAddSeparator(bar, NULL, NULL);
	
	//add 'g_Scale' to 'bar': this is a modifiable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [z] and [Z].
	//TwAddVarRW(bar, "Scale", TW_TYPE_DOUBLE, &g_Scale, " min=0.01 max=2.5 step=0.01 keyIncr=+ keyDecr=- help='Scale the object (1=original size).' ");

	//add 'g_quaternion' to 'bar': this is a variable of type TW_TYPE_QUAT4D which defines the object's orientation using quaternions
	//TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4D, &g_quaternion, " label='Object rotation' opened=true help='Change the object orientation.' ");

	TwAddButton(bar, "open", loadOBJModel, NULL, " label='Open OBJ File...' ");

	// Reserve the bounding box buffers
	bbox_buffers.clear_reserve_resize(BOUNDING_BOX_VERTICES);

	// Reserve the world axes buffers
	axes_buffers.clear_reserve_resize(WORLD_AXES_VERTICES);


	// Call the GLUT main loop
	glutMainLoop();

	return 0;
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
	ambient_light.intensity = AMBIENT_LIGHT_INTENSITY;
	point_light1.intensity = LIGHT1_INTENSITY;
	point_light2.intensity = LIGHT2_INTENSITY;
	point_light1.pos = POINT_LIGHT1_POS;
	point_light2.pos = POINT_LIGHT2_POS;
	directional_light1.intensity = point_light1.intensity;
	directional_light2.intensity = point_light2.intensity;
	directional_light1.dir = point_light1.pos;
	directional_light2.dir = point_light2.pos;
	lighting.ambient_light = &ambient_light;
	lighting.light1 = &point_light1;
	lighting.light2 = &point_light2;

	// Reserve the number of pixels
	list_pixels.clear();
	if (list_pixels.capacity() < (size_t)(screen.x * screen.y)) list_pixels.reserve(screen.x * screen.y);
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
	mouse = Mouse(screen.mid_point());
	glutSetCursor(GLUT_CURSOR_INHERIT); // Make the cursor visible

	// Initialize the actions
	actions_done[Action::BOUNDING_BOX] = false;
	actions_done[Action::VERTEX_NORMALS] = false;
	actions_done[Action::WORLD_AXES] = true;
	actions_done[Action::MOUSE_LOOK] = false;
	actions_done[Action::FPS_CAMERA] = false;
	actions_done[Action::OBJ_ROTATE] = false;
	actions_done[Action::VIEW_OBJECT] = false;
	actions_done[Action::OBJ_CONTROL_MODEL] = false;
}

void initKeybindings() {
	// Alphabetical keys bindings
	keyAlpBind.set(DEFAULT_CAM_LEFT, Action::CAM_LEFT);
	keyAlpBind.set(DEFAULT_CAM_RIGHT, Action::CAM_RIGHT);
	keyAlpBind.set(DEFAULT_CAM_FORWARD, Action::CAM_FORWARD);
	keyAlpBind.set(DEFAULT_CAM_BACKWARD, Action::CAM_BACKWARD);
	keyAlpBind.set(DEFAULT_MOUSE_LOOK, Action::MOUSE_LOOK);
	keyAlpBind.set(DEFAULT_CAM_UP, Action::CAM_UP);
	keyAlpBind.set(DEFAULT_BOUNDING_BOX, Action::BOUNDING_BOX);
	keyAlpBind.set(DEFAULT_VERTEX_NORMALS, Action::VERTEX_NORMALS);
	keyAlpBind.set(DEFAULT_WORLD_AXES, Action::WORLD_AXES);
	keyAlpBind.set(DEFAULT_OBJECT_AXES, Action::OBJECT_AXES);
	keyAlpBind.set(DEFAULT_OBJ_SCALE_INC, Action::OBJ_SCALE_INC);
	keyAlpBind.set(DEFAULT_OBJ_SCALE_DEC, Action::OBJ_SCALE_DEC);
	keyAlpBind.set(DEFAULT_PROJECTION_TOGGLE, Action::PROJECTION_TOGGLE);
	keyAlpBind.set(DEFAULT_RESET_SCENE, Action::RESET_SCENE);
	keyAlpBind.set(DEFAULT_FPS_CAMERA, Action::FPS_CAMERA);
	keyAlpBind.set(DEFAULT_VIEW_OBJECT, Action::VIEW_OBJECT);
	keyAlpBind.set(DEFAULT_ESCAPE_ALL, Action::ESCAPE_ALL);

	// Special keys bindings
	keySplBind.set(DEFAULT_OBJ_LEFT, Action::OBJ_LEFT);
	keySplBind.set(DEFAULT_OBJ_RIGHT, Action::OBJ_RIGHT);
	keySplBind.set(DEFAULT_OBJ_FORWARD, Action::OBJ_FORWARD);
	keySplBind.set(DEFAULT_OBJ_BACKWARD, Action::OBJ_BACKWARD);
	keySplBind.set(DEFAULT_OBJ_UP, Action::OBJ_UP);
	keySplBind.set(DEFAULT_OBJ_DOWN, Action::OBJ_DOWN);

	// Modifier keys bindings
	keyModBind.set((ModifierKey)DEFAULT_CAM_DOWN, Action::CAM_DOWN);
	keyModBind.set((ModifierKey)DEFAULT_OBJ_ALT_MOVE, Action::OBJ_ALT_MOVE);
	keyModBind.set((ModifierKey)DEFAULT_OBJ_CONTROL_MODEL, Action::OBJ_CONTROL_MODEL);

	// Mouse button bindings
	mouseBttnBind.set(DEFAULT_OBJ_ROTATE, Action::OBJ_ROTATE);
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

	size_t numPoints = object.meshModel.vertices.size();
	size_t numVertexNormals = numPoints;

	// Reserve the mesh buffers
	mesh_buffers.clear_reserve_resize(numPoints);

	// Reserve the normals buffers
	normals_buffers.clear_reserve_resize(numVertexNormals);

	// Initialize the object
	initObject();
}


//do not change this function unless you really know what you are doing!
void initGraphics(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(START_WIDTH, START_HEIGHT);
	glutCreateWindow("Model renderer using AntTweakBar and freeGlut");
	glutCreateMenu(NULL);

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
		list_pixels,
		camera,
		object,
		lighting,
		shading_mode,
		mesh_buffers,
		bbox_buffers,
		normals_buffers,
		axes_buffers,
		pixels,
		screen,
		actions_done[Action::BOUNDING_BOX],
		actions_done[Action::VERTEX_NORMALS],
		actions_done[Action::WORLD_AXES],
		actions_done[Action::OBJECT_AXES],
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
void Reshape(int width, int height)
{
	glUseScreenCoordinates(width, height);

	//////////////////////////////////////
	///////add your reshape code here/////////////

	// Update the screen dimensions
	screen.x = width;
	screen.y = height;

	// Reserve the number of pixels
	if (list_pixels.capacity() < (size_t)(screen.x * screen.y)) list_pixels.reserve(screen.x * screen.y);

	// Update the screen pixels buffer
	pixels.resize(screen.x, screen.y);

	// Update the mouse rest position
	mouse.update_rest(screen.mid_point());

	// Update the camera projections
	camera.aspect_ratio = screen.aspect_ratio();
	camera.update_projections();

	//////////////////////////////////////

    // Send the new window size to AntTweakBar
    TwWindowSize(width, height);
	//glutPostRedisplay();
}



void MouseButton(int button, int state, int x, int y)
{
	TwEventMouseButtonGLUT(button, state, x, y);

	if (state == GLUT_UP) mouseBttnPress.press(button);
	else if (state == GLUT_DOWN) mouseBttnPress.release(button);

	//// Check if modifiers are pressed (Ctrl, Alt, etc.)
	//int modifiers = glutGetModifiers();
	//switch (getDrawMode()) {
	//case PAIR_HOLD:
	//	pairHold_MouseButton(modifiers, button, state, x, y);
	//	break;
	//case ARRAY_CLICK:
	//	arrayClick_MouseButton(modifiers, button, state, x, y);
	//	break;
	//case ARRAY_HOLD:
	//	arrayHold_MouseButton(modifiers, button, state, x, y);
	//	break;
	//}

	//TwRefreshBar(bar);
	//glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	TwEventMouseMotionGLUT(x, y);

	//// Check if modifiers are pressed (Ctrl, Alt, etc.)
	//int modifiers = glutGetModifiers();
	//switch (getDrawMode()) {
	//case PAIR_HOLD:
	//	pairHold_MouseMotion(modifiers, x, y);
	//	break;
	//case ARRAY_CLICK:
	//	arrayClick_MouseMotion(modifiers, x, y);
	//	break;
	//case ARRAY_HOLD:
	//	arrayHold_MouseMotion(modifiers, x, y);
	//	break;
	//}

	mouse.update_pos(x, y);

	//TwRefreshBar(bar);
	//glutPostRedisplay();
}

void PassiveMouseMotion(int x, int y)
{
	//TwEventMouseMotionGLUT(x, y);
	MouseMotion(x, y);
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
void Terminate(void)
{ 
    TwTerminate();
}

// Perform the given action
void performAction(Action action, bool press) {
	switch (action) {
	default:
		// perror("Unhandled action");
		// exit(1);
		break;
	case Action::CAM_LEFT:
		actions_done[Action::CAM_LEFT] = press;
		break;
	case Action::CAM_RIGHT:
		actions_done[Action::CAM_RIGHT] = press;
		break;
	case Action::CAM_FORWARD:
		actions_done[Action::CAM_FORWARD] = press;
		break;
	case Action::CAM_BACKWARD:
		actions_done[Action::CAM_BACKWARD] = press;
		break;
	case Action::CAM_UP:
		actions_done[Action::CAM_UP] = press;
		break;
	case Action::CAM_DOWN:
		actions_done[Action::CAM_DOWN] = press;
		break;
	case Action::OBJ_LEFT:
		actions_done[Action::OBJ_LEFT] = press;
		break;
	case Action::OBJ_RIGHT:
		actions_done[Action::OBJ_RIGHT] = press;
		break;
	case Action::OBJ_FORWARD:
		actions_done[Action::OBJ_FORWARD] = press;
		break;
	case Action::OBJ_BACKWARD:
		actions_done[Action::OBJ_BACKWARD] = press;
		break;
	case Action::OBJ_UP:
		actions_done[Action::OBJ_UP] = press;
		break;
	case Action::OBJ_DOWN:
		actions_done[Action::OBJ_DOWN] = press;
		break;
	case Action::MOUSE_LOOK:
		if (press) {
			if (!actions_done[Action::MOUSE_LOOK]) { // Switch to mouse look mode
				glutSetCursor(GLUT_CURSOR_NONE); // Make the cursor invisible
				mouse.reset();
			}
			else { // Switch to normal cursor mode
				glutSetCursor(GLUT_CURSOR_INHERIT); // Make the cursor visible
			}
			mouse.update_rest(screen.mid_point());
			actions_done[Action::MOUSE_LOOK].toggle();
			actions_done[Action::OBJ_ROTATE] = false;
			actions_done[Action::FPS_CAMERA] = false;
			//actions_done[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::BOUNDING_BOX:
		if (press) actions_done[Action::BOUNDING_BOX].toggle();
		break;
	case Action::VERTEX_NORMALS:
		if (press) actions_done[Action::VERTEX_NORMALS].toggle();
		break;
	case Action::WORLD_AXES:
		if (press) actions_done[Action::WORLD_AXES].toggle();
		break;
	case Action::OBJECT_AXES:
		if (press) actions_done[Action::OBJECT_AXES].toggle();
		break;
	case Action::OBJ_SCALE_INC:
		actions_done[Action::OBJ_SCALE_INC] = press;
		break;
	case Action::OBJ_SCALE_DEC:
		actions_done[Action::OBJ_SCALE_DEC] = press;
		break;
	case Action::OBJ_ROTATE:
		glutSetCursor(GLUT_CURSOR_INHERIT); // Make the cursor visible
		actions_done[Action::MOUSE_LOOK] = false;
		actions_done[Action::OBJ_ROTATE].toggle();
		actions_done[Action::FPS_CAMERA] = false;
		break;
	case Action::PROJECTION_TOGGLE:
		if (press) camera.toggle();
		break;
	case Action::RESET_SCENE:
		if (press) initVariables();
		break;
	case Action::FPS_CAMERA:
		if (press) {
			if (!actions_done[Action::FPS_CAMERA]) { // Switch to mouse look mode
				glutSetCursor(GLUT_CURSOR_NONE); // Make the cursor invisible
				mouse.reset();
			}
			else { // Switch to normal cursor mode
				glutSetCursor(GLUT_CURSOR_INHERIT); // Make the cursor visible
			}
			mouse.update_rest(screen.mid_point());
			actions_done[Action::MOUSE_LOOK] = false;
			actions_done[Action::OBJ_ROTATE] = false;
			actions_done[Action::FPS_CAMERA].toggle();
			//actions_done[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::VIEW_OBJECT:
		if (press) actions_done[Action::VIEW_OBJECT].toggle();
		break;
	case Action::OBJ_ALT_MOVE:
		actions_done[Action::OBJ_ALT_MOVE] = press;
		break;
	case Action::ESCAPE_ALL:
		if (press) {
			glutSetCursor(GLUT_CURSOR_INHERIT); // Make the cursor visible
			actions_done[Action::MOUSE_LOOK] = false;
			actions_done[Action::FPS_CAMERA] = false;
			actions_done[Action::OBJ_ROTATE] = false;
			actions_done[Action::VIEW_OBJECT] = false;
		}
		break;
	case Action::OBJ_CONTROL_MODEL:
		actions_done[Action::OBJ_CONTROL_MODEL] = press;
	}
}

void updateModifiers(KeyModPress &map) {
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		map.press(ModifierKey::SHIFT);
	}
	else {
		map.release(ModifierKey::SHIFT);
	}
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		map.press(ModifierKey::CTRL);
	}
	else {
		map.release(ModifierKey::CTRL);
	}
	if (GetKeyState(VK_MENU) & 0x8000) {
		map.press(ModifierKey::ALT);
	}
	else {
		map.release(ModifierKey::ALT);
	}
}

void update_motion(Motion &motion,
	Action left, Action right,
	Action forward, Action backward,
	Action up, Action down
) {
	if (actions_done[left]) motion.go_left();
	else motion.stop_left();
	if (actions_done[right]) motion.go_right();
	else motion.stop_right();
	if (actions_done[forward]) motion.go_forward();
	else motion.stop_forward();
	if (actions_done[backward]) motion.go_backward();
	else motion.stop_backward();
	if (actions_done[up]) motion.go_up();
	else motion.stop_up();
	if (actions_done[down]) motion.go_down();
	else motion.stop_down();
}

void Timer(int value) {
	// Measure the time it takes to do the main control loop
	control_timer.start();

	if (frame_number == (unsigned int)value) { // No frame was rendered at that time frame
		glutTimerFunc(time_frame_ms, Timer, value); // Register the timer callback again -- with the same frame number
		return;
	}

	// Update the modifier keys map
	updateModifiers(keyModPress);

	// Check keys and perform actions
	while (keyAlpPress.hasNext()) {
		KeyAlpPress::KeyAndPress next = keyAlpPress.next();
		unsigned char k = next.key;
		bool press = next.press;
		performAction(keyAlpBind.atKey(k), press);
	}
	while (keySplPress.hasNext()) {
		KeySplPress::KeyAndPress next = keySplPress.next();
		int k = next.key;
		bool press = next.press;
		performAction(keySplBind.atKey(k), press);
	}
	while (mouseBttnPress.hasNext()) {
		MouseBttnPress::KeyAndPress next = mouseBttnPress.next();
		int k = next.key;
		bool press = next.press;
		performAction(mouseBttnBind.atKey(k), press);
	}
	if (keyModPress.press_edge(ModifierKey::SHIFT)) {
		performAction(keyModBind.atKey(ModifierKey::SHIFT), true);
	}
	if (keyModPress.release_edge(ModifierKey::SHIFT)) {
		performAction(keyModBind.atKey(ModifierKey::SHIFT), false);
	}
	if (keyModPress.press_edge(ModifierKey::CTRL)) {
		performAction(keyModBind.atKey(ModifierKey::CTRL), true);
	}
	if (keyModPress.release_edge(ModifierKey::CTRL)) {
		performAction(keyModBind.atKey(ModifierKey::CTRL), false);
	}
	if (keyModPress.press_edge(ModifierKey::ALT)) {
		performAction(keyModBind.atKey(ModifierKey::ALT), true);
	}
	if (keyModPress.release_edge(ModifierKey::ALT)) {
		performAction(keyModBind.atKey(ModifierKey::ALT), false);
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
	if (actions_done[Action::OBJ_SCALE_INC]) obj_scale_motion.go_right();
	else obj_scale_motion.stop_right();
	if (actions_done[Action::OBJ_SCALE_DEC]) obj_scale_motion.go_left();
	else obj_scale_motion.stop_left();

	// Calculate the move vectors
	Vector4 cam_move = cam_motion.calcMove();
	Vector4 obj_move = obj_motion.calcMove();
	Vector4 obj_scale_vector = obj_scale_motion.calcMove();

	// Update the object scale
	Matrix4 obj_scale = Matrix4::iso_scaling(1.0 + obj_scale_vector[0]);
	//if (actions_done[Action::OBJ_CONTROL_MODEL]) {
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
	if (actions_done[Action::MOUSE_LOOK] || actions_done[Action::FPS_CAMERA]) {
		camera.w = rotY * camera.w;
		camera.update_u_angle(-u_angle);

		if (actions_done[Action::FPS_CAMERA]) { // FPS view
			camera.update_fps_rot();
		}
		else if (actions_done[Action::MOUSE_LOOK]) { // Free view
			camera.rot = camera.rot * rotY * rotX;
		}

		// Reset the mouse position to the middle of the screen
		glutWarpPointer(screen.mid_x_int(), screen.mid_y_int()); // Set the cursor to the middle of the screen
	}
	else if (actions_done[Action::OBJ_ROTATE]) {
		if (actions_done[Action::OBJ_CONTROL_MODEL]) {
			object.model = object.model * Matrix4::transpose(rot);
		}
		else {
			object.world = object.world * Matrix4::transpose(rot);
		}
		object.rot = rot * object.rot;
	}
	// Update the mouse rest position
	if (!actions_done[Action::MOUSE_LOOK] && !actions_done[Action::FPS_CAMERA]) mouse.update_rest(mouse.curr);
	mouse.reset();

	// Calculate the translation matrix
	Matrix4 cam_translate;
	if (actions_done[Action::FPS_CAMERA]) {
		cam_translate = Matrix4::translation(rotUW * cam_move);
	}
	else {
		cam_translate = Matrix4::translation(camera.rot * cam_move);
	}
	camera.pos = cam_translate * camera.pos;

	// Update the object position
	Matrix4 obj_translate;
	if (!actions_done[Action::OBJ_ALT_MOVE]) {
		obj_translate = Matrix4::translation(rotUW * obj_move);
	}
	else {
		obj_translate = Matrix4::translation(obj_move);
	}
	if (actions_done[Action::OBJ_CONTROL_MODEL]) {
		object.model_pos = obj_translate * object.model_pos;
		object.model = obj_translate * object.model;
	}
	else {
		object.world_pos = obj_translate * object.world_pos;
		object.world = obj_translate * object.world;
	}

	// If "view object" is pressed, look at the object
	if (actions_done[Action::VIEW_OBJECT]) camera.look_at(object.world * object.model_pos);

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

	glutTimerFunc(time_frame_ms, Timer, frame_number); // Register the timer callback again
	TwRefreshBar(bar);
	glutPostRedisplay();
}

void setLightMode(int light_num, LightingEnum &light_mode, const LightingEnum &new_light_mode) {
	if (light_mode == new_light_mode) return;
	switch (light_mode) {
	default:
		// perror("Unhandled lighting mode");
		// exit(1);
		break;
	case LIGHT_POINT:
	{
		light_mode = LIGHT_DIR;
		PointLight *point_light = (light_num == 1) ? &point_light1 : &point_light2;
		DirectionalLight *directional_light = (light_num == 1) ? &directional_light1 : &directional_light2;
		Light **p_lighting_light = (light_num == 1) ? &lighting.light1 : &lighting.light2;
		directional_light->intensity = point_light->intensity;
		directional_light->dir = point_light->pos;
		*p_lighting_light = (Light*)directional_light;
		break;
	}
	case LIGHT_DIR:
	{
		light_mode = LIGHT_POINT;
		PointLight *point_light = (light_num == 1) ? &point_light1 : &point_light2;
		DirectionalLight *directional_light = (light_num == 1) ? &directional_light1 : &directional_light2;
		Light **p_lighting_light = (light_num == 1) ? &lighting.light1 : &lighting.light2;
		point_light->intensity = directional_light->intensity;
		point_light->pos = directional_light->dir;
		*p_lighting_light = (Light*)point_light;
		break;
	}
	}
}
