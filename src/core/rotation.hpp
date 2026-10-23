#pragma once

namespace dabr::core {

struct Vec3 {
    double x{};
    double y{};
    double z{};

    [[nodiscard]] double length() const noexcept;
    [[nodiscard]] Vec3 normalized() const noexcept;
    [[nodiscard]] double dot(const Vec3& other) const noexcept;
    [[nodiscard]] Vec3 cross(const Vec3& other) const noexcept;
    [[nodiscard]] Vec3 rotatedAxis(double angleRadians, Vec3 axis) const noexcept;
};

struct LookRotation {
    double pitchDegrees{};
    double yawDegrees{};
    double rollDegrees{};
    double rollDeltaDegrees{};
};

[[nodiscard]] Vec3 lookVector(double pitchDegrees, double yawDegrees) noexcept;
[[nodiscard]] LookRotation changeLookInLocalFrame(
    double currentPitchDegrees,
    double currentYawDegrees,
    double currentRollDegrees,
    double pitchInput,
    double yawInput,
    double rollInput
) noexcept;

} // namespace dabr::core
