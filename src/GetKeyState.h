#pragma once

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