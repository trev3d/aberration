#include "tracerShapes.h"

// todo: solve these formulas myself
// i'm blindly following in the interest of time
bool traceSphere::hit(const ray& r, interval t, rayhit& hit) const {
	// t^2 * dot(b) + dot(2tb, (A - C)) + dot(A - C, A - C) - r^2 = 0
	// quad formula

	vec3 oc = r.origin - trans.pos;

	float a = 1; // i make ray directions always normalized
	float bHalf = dot(oc, r.direction);
	float c = length2(oc) - radius * radius;
	float disc = bHalf * bHalf - a * c;

	if (disc < 0) return false;

	float sqrtd = sqrt(disc);

	// find nearest root in range
	// TODO: solve these formulas myself to internalize
	// i am just following this blindly in the interest of time
	float root = (-bHalf - sqrtd) / a;
	if (root <= t.min || t.max <= root) {
		root = (-bHalf + sqrtd) / a;
		if (root <= t.min || t.max <= root)
			return false;
	}

	hit.t = root;
	hit.pos = r.at(root);
	hit.setNormFromRay(r, (hit.pos - trans.pos) / radius);
	//hit.norm= (hit.pos - trans.pos) / radius;

	return true;
}