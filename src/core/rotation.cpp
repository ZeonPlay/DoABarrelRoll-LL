#include "core/rotation.hpp"

#include <algorithm>
#include <cmath>

namespace dabr::core {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kToRad = kPi / 180.0;
constexpr double kToDeg = 180.0 / kPi;

double clamp(double value, double low, double high) noexcept {
    return std::clamp(value, low, high);
}

} // namespace

double Vec3::length() const noexcept {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalized() const noexcept {
    const double magnitude = length();
    if (magnitude <= 1e-12) return {};
    return {x / magnitude, y / magnitude, z / magnitude};
}

double Vec3::dot(const Vec3& other) const noexcept {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3& other) const noexcept {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

Vec3 Vec3::rotatedAxis(double angleRadians, Vec3 axis) const noexcept {
    axis = axis.normalized();
    if (axis.length() <= 1e-12) return *this;

    const double c = std::cos(angleRadians);
    const double s = std::sin(angleRadians);
    return {
        x * c + axis.cross(*this).x * s + axis.x * (axis.dot(*this) * (1.0 - c)),
        y * c + axis.cross(*this).y * s + axis.y * (axis.dot(*this) * (1.0 - c)),
        z * c + axis.cross(*this).z * s + axis.z * (axis.dot(*this) * (1.0 - c))
    };
}

Vec3 lookVector(double pitchDegrees, double yawDegrees) noexcept {
    const double pitch = pitchDegrees * kToRad;
    const double yaw = -yawDegrees * kToRad;
    const double cp = std::cos(pitch);
    return {
        std::sin(yaw) * cp,
        -std::sin(pitch),
        std::cos(yaw) * cp
    };
}

LookRotation changeLookInLocalFrame(
    double currentPitchDegrees,
    double currentYawDegrees,
    double currentRollDegrees,
    double pitchInput,
    double yawInput,
    double rollInput
) noexcept {
    Vec3 facing = lookVector(currentPitchDegrees, currentYawDegrees);
    Vec3 left{1.0, 0.0, 0.0};
    left = left.rotatedAxis(-currentRollDegrees * kToRad, {0.0, 0.0, 1.0});
    left = left.rotatedAxis(-currentPitchDegrees * kToRad, {1.0, 0.0, 0.0});
    left = left.rotatedAxis(-(currentYawDegrees + 180.0) * kToRad, {0.0, 1.0, 0.0});

    facing = facing.rotatedAxis(-0.15 * pitchInput * kToRad, left);

    const Vec3 up = facing.cross(left);
    facing = facing.rotatedAxis(0.15 * yawInput * kToRad, up);
    left = left.rotatedAxis(0.15 * yawInput * kToRad, up);
    left = left.rotatedAxis(0.15 * rollInput * kToRad, facing);

    const double newPitch = -std::asin(clamp(facing.y, -1.0, 1.0)) * kToDeg;
    const double newYaw = -std::atan2(facing.x, facing.z) * kToDeg;
    const Vec3 normalLeft = Vec3{1.0, 0.0, 0.0}.rotatedAxis(
        -(newYaw + 180.0) * kToRad, {0.0, 1.0, 0.0}
    );
    const double newRoll = -std::atan2(
        left.cross(normalLeft).dot(facing), left.dot(normalLeft)
    ) * kToDeg;

    double yawDelta = newYaw - currentYawDegrees;
    yawDelta -= 360.0 * std::floor((yawDelta + 180.0) / 360.0);

    return {
        currentPitchDegrees + (newPitch - currentPitchDegrees),
        currentYawDegrees + yawDelta,
        newRoll,
        newRoll - currentRollDegrees
    };
}

} // namespace dabr::core
