#include "core/rotation.hpp"

#include <cassert>
#include <cmath>

namespace {
bool near(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) <= epsilon;
}
}

int main() {
    using namespace dabr::core;

    const Vec3 forward = lookVector(0.0, 0.0);
    assert(near(forward.x, 0.0));
    assert(near(forward.y, 0.0));
    assert(near(forward.z, 1.0));

    const Vec3 right = Vec3{1.0, 0.0, 0.0}.rotatedAxis(
        3.14159265358979323846 / 2.0, {0.0, 0.0, 1.0});
    assert(near(right.x, 0.0));
    assert(near(right.y, 1.0));
    assert(near(right.z, 0.0));

    const LookRotation unchanged = changeLookInLocalFrame(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    assert(near(unchanged.pitchDegrees, 0.0));
    assert(near(unchanged.yawDegrees, 0.0));
    assert(near(unchanged.rollDegrees, 0.0));

    // Yaw should remain continuous across the -180/180 degree boundary.
    const LookRotation boundary = changeLookInLocalFrame(0.0, 179.9, 0.0, 0.0, 1.0, 0.0);
    assert(boundary.yawDegrees > 179.0 && boundary.yawDegrees < 181.0);

    return 0;
}
