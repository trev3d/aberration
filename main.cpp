#include "tracer.h"
#include "tracerShapes.h"

int main() {

    traceCam cam = traceCam();
    cam.trans.apply();
    cam.focalLength = 0.8f;

    traceScene scene;
    traceSphere s1;
    s1.trans.pos = vec3(-0.4f, 0, -1);
    s1.trans.apply();
    s1.radius = 0.5f;

    scene.objects.push_back(std::make_shared<traceSphere>(s1));

    traceSphere s2;
    s2.trans.pos = vec3(0.3f, 0.2f, -0.7f);
    s2.trans.apply();
    s2.radius = 0.2f;

    scene.objects.push_back(std::make_shared<traceSphere>(s2));

    render(cam, scene, 100, 10);
}