#include "tracer.h"

// win only
#include <shlwapi.h>

// most of this code from following https://raytracing.github.io/books/RayTracingInOneWeekend.html

inline void writeColor(std::ofstream& out, glm::vec3 vec) {
	out << static_cast<int>(255.999 * vec.x) << ' '
		<< static_cast<int>(255.999 * vec.y) << ' '
		<< static_cast<int>(255.999 * vec.z) << '\n';
}

bool traceScene::hit(const ray& r, interval t, rayhit& hit) const {
	rayhit temp;
	bool didHit = false;
	interval closest = interval(t.min, t.max);

	for (const auto& obj : traceScene::objects) {
		if (obj->hit(r, closest, temp)) {
			didHit = true;
			closest.max = temp.t;
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

			if (scene.hit(r, interval(0, traceInf), hit)) {
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
