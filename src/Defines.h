#pragma once

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
#define START_PIXELS_WIDTH 4'000
#define START_PIXELS_HEIGHT 4'000
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

#define FRAME_HERTZ 60

#define FRAME_S (1.0 / FRAME_HERTZ)
#define FRAME_MS (FRAME_S * 1'000.0)
#define FRAME_MS_I ((int64_t)FRAME_MS)
#define FRAME_US (FRAME_S * 1'000'000.0)
#define FRAME_US_I ((int64_t)FRAME_US)
#define FRAME_NS (FRAME_S * 1'000'000'000.0)
#define FRAME_NS_I ((int64_t)FRAME_NS)