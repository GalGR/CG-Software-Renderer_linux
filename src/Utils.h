#pragma once

#include "osstring.h"

void ConvertQuaternionToMatrix(const double quat[4], double mat[16]);
void displayMessage(const std::string& str);
string getOpenFileName();