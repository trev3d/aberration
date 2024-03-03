#include "tracer.h"

// win only
#include <shlwapi.h>

// most of this code from following https://raytracing.github.io/books/RayTracingInOneWeekend.html

void writeColor(std::ofstream& out, glm::vec3 vec) {
	out << static_cast<int>(255.999 * vec.x) << ' '
		<< static_cast<int>(255.999 * vec.y) << ' '
		<< static_cast<int>(255.999 * vec.z) << '\n';
}

// todo: solve these formulas myself
// i'm blindly following in the interest of time
bool traceSphere::hit(const ray& r, float tMin, float tMax, rayhit& hit) const {
	// t^2 * dot(b) + dot(2tb, (A - C)) + dot(A - C, A - C) - r^2 = 0
	// quad formula

	vec3 oc = r.origin - trans.pos;

	float a = 1; // i make ray directions always normalized
	float bHalf = dot(oc, r.direction);
	float c = length2(oc) - radius * radius;
	float disc = bHalf * bHalf - a*c;

	if (disc < 0) return false;

	float sqrtd = sqrt(disc);

	// find nearest root in range
	// TODO: solve these formulas myself to internalize
	// i am just following this blindly in the interest of time
	float root = (-bHalf - sqrtd) / a;
	if (root <= tMin || tMax <= root) {
		root = (-bHalf + sqrtd) / a;
		if (root <= tMin || tMax <= root)
			return false;
	}

	hit.t = root;
	hit.pos = r.at(root);
	hit.setNormFromRay(r, (hit.pos - trans.pos) / radius);
	//hit.norm= (hit.pos - trans.pos) / radius;

	return true;
}

bool traceScene::hit(const ray& r, float tMin, float tMax, rayhit& hit) const {
	rayhit temp;
	bool didHit = false;
	float closestTMax = tMax;

	for (const auto& obj : traceScene::objects) {
		if (obj->hit(r, tMin, closestTMax, temp)) {
			didHit = true;
			closestTMax = temp.t;
			hit = temp;
		}
	}

	return didHit;
};

void render(traceCam cam, traceScene scene) {
	// Image

	vec3 camCenter = cam.trans.pos;

	int imgWidth = cam.imgWidth;
	int imgHeight = cam.imgHeight;

	vec2 viewportSize = vec2(2, 2);

	vec3 viewU = vec3(viewportSize.x, 0, 0);
	vec3 viewV = vec3(0, -viewportSize.y, 0);

	vec3 viewUDelta = viewU / (float)imgWidth;
	vec3 viewVDelta = viewV / (float)imgHeight;

	// upperleft pixel location

	vec3 viewUpperLeft = camCenter - vec3(0, 0, cam.focalLength) - viewU / 2.0f - viewV / 2.0f;

	vec3 view0 = viewUpperLeft + 0.5f * (viewUDelta + viewVDelta);

	// Render

	std::ofstream output;
	output.open("image.ppm");

	output << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int v = 0; v < imgHeight; ++v) {

		std::clog << "\rScanlines remaining: " << (imgHeight - v) << ' ' << std::flush;

		for (int u = 0; u < imgWidth; ++u) {

			vec3 pixelCenter = view0 + viewUDelta * (float)u + viewVDelta * (float)v;

			vec3 rayDir = pixelCenter - camCenter;

			ray r = ray(camCenter, rayDir);

			rayhit hit;

			vec3 color = vec3(0);

			if (scene.hit(r, 0, 9999, hit)) {
				color = 0.5f * (hit.norm + vec3(1));
			}

			writeColor(output, color);
		}
	}

	output.close();

	// win only
	// open image after render
	ShellExecute(0, 0, L"image.ppm", 0, 0, SW_SHOW);
}
