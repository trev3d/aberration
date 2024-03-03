#pragma once

#include "tracer.h"

struct traceSphere : traceable {
	float radius;
	virtual bool hit(const ray& r, interval t, rayhit& hit) const override;
};