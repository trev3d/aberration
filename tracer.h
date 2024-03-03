#pragma once

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <memory>

#include "transform.h"

using namespace glm;

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
	virtual bool hit(const ray& r, float tMin, float tMax, rayhit& hit) const = 0;
};

struct traceSphere : traceable {
	float radius;
	virtual bool hit(const ray& r, float tMin, float tMax, rayhit& hit) const override;
};

struct traceScene : traceable {
	std::vector<std::shared_ptr<traceable>> objects;
	virtual bool hit(const ray& r, float tMin, float tMax, rayhit& hit) const override;
};

void render(traceCam cam, traceScene scene);