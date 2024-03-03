#include "tracer.h"

// win only
#include <shlwapi.h>

// most of this code from following https://raytracing.github.io/books/RayTracingInOneWeekend.html

inline void writeColor(std::ofstream& out, glm::vec3 vec, int samples) {

	vec *= 1.f / (float)samples;

	out << static_cast<int>(255.999 * vec.x) << ' '
		<< static_cast<int>(255.999 * vec.y) << ' '
		<< static_cast<int>(255.999 * vec.z) << '\n';
}

inline vec3 randVec(float min, float max) {
	return vec3(random(min, max), random(min, max), random(min, max));
}

inline vec3 randNorm() {

	// reject mag > 1 for equal prob 
	while (true) {
		vec3 p = randVec(-1, 1);

		if (length2(p) < 1.0f)
			return normalize(p);
	}
}

inline vec3 randNormHemi(const vec3& norm) {
	vec3 rand = randNorm();

	if (dot(rand, norm) > 0.f)
		return rand;
	return -rand;
}

bool traceScene::hit(const ray& r, interval t, rayhit& hit) const {
	bool didHit = false;
	interval closest = interval(t.min, t.max);

	for (const auto& obj : traceScene::objects) {
		if (obj->hit(r, closest, hit)) {
			didHit = true;
			closest.max = hit.t;
		}
	}

	return didHit;
};

vec3 rayColor(const ray& r, int bounces, traceScene scene) {
	rayhit hit;

	if (bounces <= 0) {
		return vec3(0);
	}

	if (scene.hit(r, interval(0.001f, traceInf), hit)) {

		if (!hit.front)
			return vec3(0);

		ray b = ray(hit.pos, randNormHemi(hit.norm));
		return 0.5f * rayColor(b, bounces - 1, scene);

	} else {
		float a = 0.5f * (r.direction.y + 1.0f);
		return (1.0f - a) * vec3(1) + a * vec3(0.5f, 0.7f, 1.0f);
	}
}

void render(traceCam cam, traceScene scene, int samples, int bounces) {
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

			vec3 sampleColor = vec3(0);

			for (int s = 0; s < samples; s++) {
				vec3 pixelCenter = view0 + viewUDelta * (float)u + viewVDelta * (float)v;

				vec3 randOffset = viewUDelta * (-0.5f + random()) + viewVDelta * (-0.5f + random());

				vec3 rayDir = pixelCenter + randOffset - camCenter;

				ray r = ray(camCenter, rayDir);
				
				sampleColor += rayColor(r, bounces, scene);
			}

			writeColor(output, sampleColor, samples);
		}
	}

	output.close();

	// win only
	// open image after render
	ShellExecute(0, 0, L"image.ppm", 0, 0, SW_SHOW);
}
