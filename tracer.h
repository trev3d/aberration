#pragma once

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <memory>

#include "transform.h"

using namespace glm;

const float traceInf = std::numeric_limits<float>::infinity();
const float tracePi = 3.1415926535897932385f;

inline double traceDegToRad(double degrees) {
	return degrees * tracePi / 180.0f;
}

#pragma once
struct interval
{
	float min, max;

	interval() : min(traceInf), max(-traceInf) {}

	interval(float min, float max) {
		this->min = min;
		this->max = max;
	}

	static const interval empty, universe;
};

const static interval empty(+traceInf, -traceInf);
const static interval universe(-traceInf, +traceInf);

struct traceCam {
	jtgTransform trans;

	int imgWidth = 256;
	int imgHeight = 256;

	float focalLength = 1;
};


struct ray {
	ray(vec3 orig, vec3 dir) {
		origin = orig;
		direction = normalize(dir);
	}

	vec3 origin;
	vec3 direction;

	vec3 at(float t) const {
		return origin + t * direction;
	}
};

struct rayhit {
	vec3 pos = vec3(0, 0, 0);
	vec3 norm = vec3(0, 1, 0);
	float t = 0;
	bool front = true;

	void setNormFromRay(const ray& r, const vec3& outNorm) {
		front = dot(r.direction, outNorm) < 0;
		norm = front ? outNorm : -outNorm;
	}
};

struct traceable {
	jtgTransform trans;
	virtual bool hit(const ray& r, interval t, rayhit& hit) const = 0;
};

struct traceScene : traceable {
	std::vector<std::shared_ptr<traceable>> objects;
	virtual bool hit(const ray& r, interval t, rayhit& hit) const override;
};

void render(traceCam cam, traceScene scene);