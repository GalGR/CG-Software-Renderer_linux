#include "Utils.h"
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include "osdialog/osdialog.h"
#endif
#include <iostream>
#include <fstream>


//helper function to convert a quaternion to a 4x4 rotation matrix
void ConvertQuaternionToMatrix(const double quat[4], double mat[16])
{
	double yy2 = 2.0f * quat[1] * quat[1];
	double xy2 = 2.0f * quat[0] * quat[1];
	double xz2 = 2.0f * quat[0] * quat[2];
	double yz2 = 2.0f * quat[1] * quat[2];
	double zz2 = 2.0f * quat[2] * quat[2];
	double wz2 = 2.0f * quat[3] * quat[2];
	double wy2 = 2.0f * quat[3] * quat[1];
	double wx2 = 2.0f * quat[3] * quat[0];
	double xx2 = 2.0f * quat[0] * quat[0];
	mat[0*4+0] = - yy2 - zz2 + 1.0f;
	mat[0*4+1] = xy2 + wz2;
	mat[0*4+2] = xz2 - wy2;
	mat[0*4+3] = 0;
	mat[1*4+0] = xy2 - wz2;
	mat[1*4+1] = - xx2 - zz2 + 1.0f;
	mat[1*4+2] = yz2 + wx2;
	mat[1*4+3] = 0;
	mat[2*4+0] = xz2 + wy2;
	mat[2*4+1] = yz2 - wx2;
	mat[2*4+2] = - xx2 - yy2 + 1.0f;
	mat[2*4+3] = 0;
	mat[3*4+0] = mat[3*4+1] = mat[3*4+2] = 0;
	mat[3*4+3] = 1;
}


#if defined(_WIN32)
void displayMessage(const std::string& str)
{
	std::wstring wStr(str.begin(), str.end());
	MessageBox(NULL, wStr.c_str(), TEXT("Renderer"), MB_OK);
}
#elif defined(__linux__)
void displayMessage(const std::string& str) {
	osdialog_message(OSDIALOG_INFO, OSDIALOG_OK, str.c_str());
}
#endif

#if defined(_WIN32)
string getOpenFileName()
{
	const int strMaxLen = 10000;
	OPENFILENAME ofn = {0};
	TCHAR fileStr[strMaxLen] = {0};

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = fileStr;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(fileStr)/sizeof(TCHAR) - 1;

	GetOpenFileName(&ofn);
	return fileStr;
}
#elif defined(__linux__)
string getOpenFileName() {
	char *filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
	if (filename) {
		string fileStr(filename);
		free(filename); // Avoid memory leakage!
		return fileStr;
	}
	return string(); // Return empty string
}
#endif