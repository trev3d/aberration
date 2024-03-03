#include "tracer.h"

int main() {

    traceCam cam = traceCam();
    cam.trans.apply();

    traceScene scene;
    traceSphere sphere;
    sphere.trans.pos = vec3(0, 0, -1);
    sphere.trans.apply();
    sphere.radius = 0.5f;

    scene.objects.push_back(std::make_shared<traceSphere>(sphere));

    traceSphere s2;
    s2.trans.pos = vec3(0.3f, 0.2f, -0.7f);
    s2.trans.apply();
    s2.radius = 0.2f;

    scene.objects.push_back(std::make_shared<traceSphere>(s2));

    render(cam, scene);
}