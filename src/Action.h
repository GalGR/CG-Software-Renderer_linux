#pragma once

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