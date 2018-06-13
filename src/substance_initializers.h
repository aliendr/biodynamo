#ifndef SUBSTANCE_INITIALIZERS_H_
#define SUBSTANCE_INITIALIZERS_H_

#include <stdexcept>
#include <vector>

#include "Math/DistFunc.h"

#include "diffusion_grid.h"

namespace bdm {

// -----------------------------------------------------------------------------
// A substance initializer is a function that can be used to initialize the
// concentration values of a particular substance in space.
// -----------------------------------------------------------------------------

// Use this enum to express the axis you are interested in
enum Axis { kXAxis, kYAxis, kZAxis };

/// An initializer that uniformly initializes the concentration of a diffusion
/// grid based on the input value and the range (along the specified axis).
struct Uniform {
  float min_;
  float max_;
  float value_;
  uint8_t axis_;

  Uniform(float min, float max, float value, uint8_t axis) {
    min_ = min;
    max_ = max;
    value_ = value;
    axis_ = axis;
  }

  float operator()(float x, float y, float z) {
    switch (axis_) {
      case Axis::kXAxis: {
        if (x >= min_ && x <= max_) {
          return value_;
        }
        break;
      }
      case Axis::kYAxis: {
        if (y >= min_ && y <= max_) {
          return value_;
        }
        break;
      }
      case Axis::kZAxis: {
        if (z >= min_ && z <= max_) {
          return value_;
        }
        break;
      }
      default:
        throw std::logic_error("You have chosen an non-existing axis!");
    }
    return 0;
  }
};

/// An initializer that follows a Gaussian (normal) distribution along one axis
/// We use ROOT's built-in statistics function `normal_pdf(X, sigma, mean)`,
/// that follows the normal probability density function:
/// ( 1/( sigma * sqrt(2*pi) ))*e^( (-(x - mean )^2) / (2*sigma^2))
struct GaussianBand {
  float mean_;
  float sigma_;
  uint8_t axis_;

  /// @brief      The constructor
  ///
  /// @param[in]  mean   The mean of the Gaussian distribution (should be a
  ///                    value within the range of the chosen axis)
  /// @param[in]  sigma  The sigma of the Gaussian distribution
  /// @param[in]  axis   The axis along which you want the Gaussian distribution
  ///                    to be oriented to
  ///
  GaussianBand(float mean, float sigma, uint8_t axis) {
    mean_ = mean;
    sigma_ = sigma;
    axis_ = axis;
  }

  /// @brief      The model that we want to apply for substance initialization.
  ///             The operator is called for the entire space
  ///
  /// @param[in]  x     The x coordinate
  /// @param[in]  y     The y coordinate
  /// @param[in]  z     The z coordinate
  ///
  float operator()(float x, float y, float z) {
    switch (axis_) {
      case Axis::kXAxis:
        return ROOT::Math::normal_pdf(x, sigma_, mean_);
      case Axis::kYAxis:
        return ROOT::Math::normal_pdf(y, sigma_, mean_);
      case Axis::kZAxis:
        return ROOT::Math::normal_pdf(z, sigma_, mean_);
      default:
        throw std::logic_error("You have chosen an non-existing axis!");
    }
  }
};

/// An initializer that follows a Poisson (normal) distribution along one axis
/// The function ROOT::Math::poisson_pdfd(X, lambda) follows the normal
/// probability density function:
/// {e^( - lambda ) * lambda ^x )} / x!
struct PoissonBand {
  float lambda_;
  uint8_t axis_;

  /// @brief      The constructor
  ///
  /// @param[in]  lambda The lambda of the Poisson distribution
  /// @param[in]  axis   The axis along which you want the Poisson distribution
  ///                    to be oriented to
  ///
  PoissonBand(float lambda, uint8_t axis) {
    lambda_ = lambda;
    axis_ = axis;
  }

  /// @brief      The model that we want to apply for substance initialization.
  ///             The operator is called for the entire space
  ///
  /// @param[in]  x     The x coordinate
  /// @param[in]  y     The y coordinate
  /// @param[in]  z     The z coordinate
  ///
  float operator()(float x, float y, float z) {
    switch (axis_) {
      case Axis::kXAxis:
        return ROOT::Math::poisson_pdf(x, lambda_);
      case Axis::kYAxis:
        return ROOT::Math::poisson_pdf(y, lambda_);
      case Axis::kZAxis:
        return ROOT::Math::poisson_pdf(y, lambda_);
      default:
        throw std::logic_error("You have chosen an non-existing axis!");
    }
  }
};

}  // namespace bdm

#endif  // SUBSTANCE_INITIALIZERS_H_
