#include "Face.h"

Face::Face(std::array<size_t, FACE_VERTICES> indices, const std::vector<Vector4> &vertices, bool counter_clock_wise) :
	indices{ indices }, counter_clock_wise{ counter_clock_wise } {

	this->face_normal = this->calc_normal(vertices);
}