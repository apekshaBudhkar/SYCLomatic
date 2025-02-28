// DPCT_LABEL_BEGIN|License|
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
//==---- rng_utils.hpp ----------------------------*- C++ -*----------------==//
//
// Copyright (C) Intel Corporation
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// See https://llvm.org/LICENSE.txt for license information.
//
//===----------------------------------------------------------------------===//
// DPCT_LABEL_END

#ifndef __DPCT_RNG_UTILS_HPP__
#define __DPCT_RNG_UTILS_HPP__
// DPCT_COMMENT
// DPCT_COMMENT Example1:
// DPCT_COMMENT // DPCT_LABEL_BEGIN|FeatureNameDef|[Namespace]
// DPCT_COMMENT // DPCT_DEPENDENCY_EMPTY
// DPCT_COMMENT // DPCT_CODE
// DPCT_COMMENT some code
// DPCT_COMMENT // DPCT_LABEL_END
// DPCT_COMMENT
// DPCT_COMMENT Example2:
// DPCT_COMMENT // DPCT_LABEL_BEGIN|FeatureNameDef|[Namespace]
// DPCT_COMMENT // DPCT_DEPENDENCY_BEGIN
// DPCT_COMMENT // FileID|FeatureNameRef
// DPCT_COMMENT [// FileID|FeatureNameRef]
// DPCT_COMMENT ...
// DPCT_COMMENT // DPCT_DEPENDENCY_END
// DPCT_COMMENT // DPCT_CODE
// DPCT_COMMENT some code
// DPCT_COMMENT // DPCT_LABEL_END
// DPCT_COMMENT
// DPCT_COMMENT For header file including dependency, please use predefined feature name:
// DPCT_COMMENT   local_include_dependency: dpct helper files
// DPCT_COMMENT   non_local_include_dependency: other header files

// DPCT_LABEL_BEGIN|non_local_include_dependency|
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
#include <sycl/sycl.hpp>
#include <oneapi/mkl.hpp>
#include <oneapi/mkl/rng/device.hpp>
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|local_include_dependency|
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
// DPCT_LABEL_END
#include "device.hpp"
#include "lib_common_utils.hpp"

namespace dpct {
namespace rng {
namespace device {
// DPCT_LABEL_BEGIN|rng_generator|dpct::rng::device
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator_1
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// The random number generator on device.
/// \tparam engine_t The device random number generator engine. It can only be
/// oneapi::mkl::rng::device::mrg32k3a<1> or
/// oneapi::mkl::rng::device::mrg32k3a<4> or
/// oneapi::mkl::rng::device::philox4x32x10<1> or
/// oneapi::mkl::rng::device::philox4x32x10<4>.
template <typename engine_t> class rng_generator {
  static_assert(
      std::disjunction_v<
          std::is_same<engine_t, oneapi::mkl::rng::device::mrg32k3a<1>>,
          std::is_same<engine_t, oneapi::mkl::rng::device::mrg32k3a<4>>,
          std::is_same<engine_t, oneapi::mkl::rng::device::philox4x32x10<1>>,
          std::is_same<engine_t, oneapi::mkl::rng::device::philox4x32x10<4>>>,
      "engine_t can only be oneapi::mkl::rng::device::mrg32k3a<1> or "
      "oneapi::mkl::rng::device::mrg32k3a<4> or "
      "oneapi::mkl::rng::device::philox4x32x10<1> or "
      "oneapi::mkl::rng::device::philox4x32x10<4>.");
  static constexpr bool _is_engine_vec_size_one = std::disjunction_v<
      std::is_same<engine_t, oneapi::mkl::rng::device::mrg32k3a<1>>,
      std::is_same<engine_t, oneapi::mkl::rng::device::philox4x32x10<1>>>;
  static constexpr std::uint64_t default_seed = 0;
  oneapi::mkl::rng::device::bits<std::uint32_t> _distr_bits;
  oneapi::mkl::rng::device::gaussian<float> _distr_gaussian_float;
  oneapi::mkl::rng::device::gaussian<double> _distr_gaussian_double;
  oneapi::mkl::rng::device::lognormal<float> _distr_lognormal_float;
  oneapi::mkl::rng::device::lognormal<double> _distr_lognormal_double;
  oneapi::mkl::rng::device::poisson<std::uint32_t> _distr_poisson;
  oneapi::mkl::rng::device::uniform<float> _distr_uniform_float;
  oneapi::mkl::rng::device::uniform<double> _distr_uniform_double;
  engine_t _engine;

public:
  /// Default constructor of rng_generator
  rng_generator() { _engine = engine_t(default_seed); }
  /// Constructor of rng_generator
  /// \param [in] seed The seed to initialize the engine state.
  /// \param [in] num_to_skip Set the number of elements need to be skipped.
  /// The number is calculated as: num_to_skip[0] + num_to_skip[1] * 2^64 +
  /// num_to_skip[2] * 2^128 + ... + num_to_skip[n-1] * 2^(64*(n-1))
  rng_generator(std::uint64_t seed,
                std::initializer_list<std::uint64_t> num_to_skip) {
    _engine = engine_t(seed, num_to_skip);
  }
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|rng_generator_generate|dpct::rng::device
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator
// RngUtils|rng_generator_generate_vec
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Generate random number(s) obeys distribution \tparam distr_t.
  /// \tparam T The distribution of the random number. It can only be
  /// oneapi::mkl::rng::device::bits<std::uint32_t>,
  /// oneapi::mkl::rng::device::gaussian<float>,
  /// oneapi::mkl::rng::device::gaussian<double>,
  /// oneapi::mkl::rng::device::lognormal<float>,
  /// oneapi::mkl::rng::device::lognormal<double>,
  /// oneapi::mkl::rng::device::poisson<std::uint32_t>,
  /// oneapi::mkl::rng::device::uniform<float> or
  /// oneapi::mkl::rng::device::uniform<double>
  /// \tparam vec_size The length of the return vector. It can only be 1, 2
  /// or 4.
  /// \param distr_params The parameter(s) for lognormal or poisson
  /// distribution.
  /// \return The vector of the random number(s).
  template <typename distr_t, int vec_size, class... distr_params_t>
  auto generate(distr_params_t... distr_params) {
    static_assert(vec_size == 1 || vec_size == 2 || vec_size == 4,
                  "vec_size is not supported.");
    static_assert(
        std::disjunction_v<
            std::is_same<distr_t,
                         oneapi::mkl::rng::device::bits<std::uint32_t>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::gaussian<float>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::gaussian<double>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::lognormal<float>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::lognormal<double>>,
            std::is_same<distr_t,
                         oneapi::mkl::rng::device::poisson<std::uint32_t>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::uniform<float>>,
            std::is_same<distr_t, oneapi::mkl::rng::device::uniform<double>>>,
        "distribution is not supported.");

    if constexpr (std::is_same_v<
                      distr_t, oneapi::mkl::rng::device::bits<std::uint32_t>>) {
      return generate_vec<vec_size>(_distr_bits);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::gaussian<float>>) {
      return generate_vec<vec_size>(_distr_gaussian_float);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::gaussian<double>>) {
      return generate_vec<vec_size>(_distr_gaussian_double);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::lognormal<float>>) {
      return generate_vec<vec_size>(_distr_lognormal_float, distr_params...,
                                    0.0f, 1.0f);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::lognormal<double>>) {
      return generate_vec<vec_size>(_distr_lognormal_double, distr_params...,
                                    0.0, 1.0);
    }
    if constexpr (std::is_same_v<distr_t, oneapi::mkl::rng::device::poisson<
                                              std::uint32_t>>) {
      return generate_vec<vec_size>(_distr_poisson, distr_params...);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::uniform<float>>) {
      return generate_vec<vec_size>(_distr_uniform_float);
    }
    if constexpr (std::is_same_v<distr_t,
                                 oneapi::mkl::rng::device::uniform<double>>) {
      return generate_vec<vec_size>(_distr_uniform_double);
    }
  }
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|rng_generator_get_engine|dpct::rng::device
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Get the random number generator engine.
  /// \return The reference of the internal random number generator engine.
  engine_t &get_engine() { return _engine; }
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|rng_generator_generate_vec|dpct::rng::device
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator
// DPCT_DEPENDENCY_END
// DPCT_CODE
private:
  template <int vec_size, typename distr_t, class... distr_params_t>
  auto generate_vec(distr_t &distr, distr_params_t... distr_params) {
    if constexpr (sizeof...(distr_params_t)) {
      typename distr_t::param_type pt(distr_params...);
      distr.param(pt);
    }
    if constexpr (vec_size == 4) {
      if constexpr (_is_engine_vec_size_one) {
        sycl::vec<typename distr_t::result_type, 4> res;
        res.x() = oneapi::mkl::rng::device::generate(distr, _engine);
        res.y() = oneapi::mkl::rng::device::generate(distr, _engine);
        res.z() = oneapi::mkl::rng::device::generate(distr, _engine);
        res.w() = oneapi::mkl::rng::device::generate(distr, _engine);
        return res;
      } else {
        return oneapi::mkl::rng::device::generate(distr, _engine);
      }
    } else if constexpr (vec_size == 1) {
      if constexpr (_is_engine_vec_size_one) {
        return oneapi::mkl::rng::device::generate(distr, _engine);
      } else {
        return oneapi::mkl::rng::device::generate_single(distr, _engine);
      }
    } else if constexpr (vec_size == 2) {
      if constexpr (_is_engine_vec_size_one) {
        sycl::vec<typename distr_t::result_type, 2> res;
        res.x() = oneapi::mkl::rng::device::generate(distr, _engine);
        res.y() = oneapi::mkl::rng::device::generate(distr, _engine);
        return res;
      } else {
        sycl::vec<typename distr_t::result_type, 2> res;
        res.x() = oneapi::mkl::rng::device::generate_single(distr, _engine);
        res.y() = oneapi::mkl::rng::device::generate_single(distr, _engine);
        return res;
      }
    }
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|rng_generator_1|dpct::rng::device
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator
// DPCT_DEPENDENCY_END
// DPCT_CODE
};
// DPCT_LABEL_END

} // namespace device

namespace host {
namespace detail {
// DPCT_LABEL_BEGIN|rng_generator_base|dpct::rng::host::detail
// DPCT_DEPENDENCY_BEGIN
// Device|get_default_queue
// DPCT_DEPENDENCY_END
// DPCT_CODE
class rng_generator_base {
public:
  /// Set the seed of host rng_generator.
  /// \param seed The engine seed.
  virtual void set_seed(const std::uint64_t seed) = 0;

  /// Set the dimensions of host rng_generator.
  /// \param dimensions The engine dimensions.
  virtual void set_dimensions(const std::uint32_t dimensions) = 0;

  /// Set the queue of host rng_generator.
  /// \param queue The engine queue.
  virtual void set_queue(sycl::queue *queue) = 0;

  /// Generate unsigned int random number(s) with 'uniform_bits' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  virtual inline void generate_uniform_bits(unsigned int *output,
                                            std::int64_t n) = 0;

  /// Generate unsigned long long random number(s) with 'uniform_bits'
  /// distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  virtual inline void generate_uniform_bits(unsigned long long *output,
                                            std::int64_t n) = 0;

  /// Generate float random number(s) with 'lognormal' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param m Mean of associated normal distribution
  /// \param s Standard deviation of associated normal distribution.
  virtual inline void generate_lognormal(float *output, std::int64_t n, float m,
                                         float s) = 0;

  /// Generate double random number(s) with 'lognormal' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param m Mean of associated normal distribution
  /// \param s Standard deviation of associated normal distribution.
  virtual inline void generate_lognormal(double *output, std::int64_t n,
                                         double m, double s) = 0;

  /// Generate float random number(s) with 'gaussian' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param mean Mean of normal distribution
  /// \param stddev Standard deviation of normal distribution.
  virtual inline void generate_gaussian(float *output, std::int64_t n,
                                        float mean, float stddev) = 0;

  /// Generate double random number(s) with 'gaussian' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param mean Mean of normal distribution
  /// \param stddev Standard deviation of normal distribution.
  virtual inline void generate_gaussian(double *output, std::int64_t n,
                                        double mean, double stddev) = 0;

  /// Generate unsigned int random number(s) with 'poisson' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param lambda Lambda for the Poisson distribution.
  virtual inline void generate_poisson(unsigned int *output, std::int64_t n,
                                       double lambda) = 0;

  /// Generate float random number(s) with 'uniform' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  virtual inline void generate_uniform(float *output, std::int64_t n) = 0;

  /// Generate double random number(s) with 'uniform' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  virtual inline void generate_uniform(double *output, std::int64_t n) = 0;

  /// Skip ahead several random number(s).
  /// \param num_to_skip The number of random numbers to be skipped.
  virtual void skip_ahead(const std::uint64_t num_to_skip) = 0;

protected:
  sycl::queue *_queue{&dpct::get_default_queue()};
  std::uint64_t _seed{0};
  std::uint32_t _dimensions{1};
};
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|rng_generator_host|dpct::rng::host::detail
// DPCT_DEPENDENCY_BEGIN
// RngUtils|rng_generator_base
// LibCommonUtils|get_memory
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// The random number generator on host.
template <typename engine_t = oneapi::mkl::rng::philox4x32x10>
class rng_generator : public rng_generator_base {
public:
  /// Constructor of rng_generator.
  rng_generator() : _engine(creat_engine(_queue, _seed, _dimensions)) {}

  /// Set the seed of host rng_generator.
  /// \param seed The engine seed.
  void set_seed(const std::uint64_t seed) {
    if (seed == _seed) {
      return;
    }
    _seed = seed;
    _engine = creat_engine(_queue, _seed, _dimensions);
  }

  /// Set the dimensions of host rng_generator.
  /// \param dimensions The engine dimensions.
  void set_dimensions(const std::uint32_t dimensions) {
    if (dimensions == _dimensions) {
      return;
    }
    _dimensions = dimensions;
    _engine = creat_engine(_queue, _seed, _dimensions);
  }

  /// Set the queue of host rng_generator.
  /// \param queue The engine queue.
  void set_queue(sycl::queue *queue) {
    if (queue == _queue) {
      return;
    }
    _queue = queue;
    _engine = creat_engine(_queue, _seed, _dimensions);
  }

  /// Generate unsigned int random number(s) with 'uniform_bits' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  inline void generate_uniform_bits(unsigned int *output, std::int64_t n) {
    static_assert(sizeof(unsigned int) == sizeof(std::uint32_t));
    generate<oneapi::mkl::rng::uniform_bits<std::uint32_t>>(
        (std::uint32_t *)output, n);
  }

  /// Generate unsigned long long random number(s) with 'uniform_bits'
  /// distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  inline void generate_uniform_bits(unsigned long long *output,
                                    std::int64_t n) {
    static_assert(sizeof(unsigned long long) == sizeof(std::uint64_t));
    generate<oneapi::mkl::rng::uniform_bits<std::uint64_t>>(
        (std::uint64_t *)output, n);
  }

  /// Generate float random number(s) with 'lognormal' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param m Mean of associated normal distribution
  /// \param s Standard deviation of associated normal distribution.
  inline void generate_lognormal(float *output, std::int64_t n, float m,
                                 float s) {
    generate<oneapi::mkl::rng::lognormal<float>>(output, n, m, s);
  }

  /// Generate double random number(s) with 'lognormal' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param m Mean of associated normal distribution
  /// \param s Standard deviation of associated normal distribution.
  inline void generate_lognormal(double *output, std::int64_t n, double m,
                                 double s) {
    generate<oneapi::mkl::rng::lognormal<double>>(output, n, m, s);
  }

  /// Generate float random number(s) with 'gaussian' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param mean Mean of normal distribution
  /// \param stddev Standard deviation of normal distribution.
  inline void generate_gaussian(float *output, std::int64_t n, float mean,
                                float stddev) {
    generate<oneapi::mkl::rng::gaussian<float>>(output, n, mean, stddev);
  }

  /// Generate double random number(s) with 'gaussian' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param mean Mean of normal distribution
  /// \param stddev Standard deviation of normal distribution.
  inline void generate_gaussian(double *output, std::int64_t n, double mean,
                                double stddev) {
    generate<oneapi::mkl::rng::gaussian<double>>(output, n, mean, stddev);
  }

  /// Generate unsigned int random number(s) with 'poisson' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  /// \param lambda Lambda for the Poisson distribution.
  inline void generate_poisson(unsigned int *output, std::int64_t n,
                               double lambda) {
    generate<oneapi::mkl::rng::poisson<unsigned int>>(output, n, lambda);
  }

  /// Generate float random number(s) with 'uniform' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  inline void generate_uniform(float *output, std::int64_t n) {
    generate<oneapi::mkl::rng::uniform<float>>(output, n);
  }

  /// Generate double random number(s) with 'uniform' distribution.
  /// \param output The pointer of the first random number.
  /// \param n The number of random numbers.
  inline void generate_uniform(double *output, std::int64_t n) {
    generate<oneapi::mkl::rng::uniform<double>>(output, n);
  }

  /// Skip ahead several random number(s).
  /// \param num_to_skip The number of random numbers to be skipped.
  void skip_ahead(const std::uint64_t num_to_skip) {
    if constexpr (std::is_same_v<engine_t, oneapi::mkl::rng::mt2203>)
      throw std::runtime_error("no skip_ahead method of mt2203 engine.");
    else
      oneapi::mkl::rng::skip_ahead(_engine, num_to_skip);
  }

private:
  static inline engine_t creat_engine(sycl::queue *queue,
                                      const std::uint64_t seed,
                                      const std::uint32_t dimensions) {
    return std::is_same_v<engine_t, oneapi::mkl::rng::sobol>
               ? engine_t(*queue, dimensions)
               : engine_t(*queue, seed);
  }

  template <typename distr_t, typename buffer_t, class... distr_params_t>
  void generate(buffer_t *output, const std::int64_t n,
                const distr_params_t... distr_params) {
    auto output_buf = dpct::detail::get_memory(output);
    oneapi::mkl::rng::generate(distr_t(distr_params...), _engine, n,
                               output_buf);
  }
  engine_t _engine{};
};
// DPCT_LABEL_END
} // namespace detail
} // namespace host

// DPCT_LABEL_BEGIN|random_engine_type|dpct::rng
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
enum class random_engine_type {
  philox4x32x10,
  mrg32k3a,
  mt2203,
  mt19937,
  sobol
};
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|typedef_host_rng_ptr|dpct::rng
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
typedef std::shared_ptr<rng::host::detail::rng_generator_base> host_rng_ptr;
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|create_host_rng|dpct::rng
// DPCT_DEPENDENCY_BEGIN
// RngUtils|typedef_host_rng_ptr
// RngUtils|random_engine_type
// RngUtils|rng_generator_host
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// Create a host random number generator.
/// \param type The random engine type.
/// \return The pointer of random number generator.
inline host_rng_ptr create_host_rng(const random_engine_type type) {
  switch (type) {
  case random_engine_type::philox4x32x10:
    return std::make_shared<
        rng::host::detail::rng_generator<oneapi::mkl::rng::philox4x32x10>>();
  case random_engine_type::mrg32k3a:
    return std::make_shared<
        rng::host::detail::rng_generator<oneapi::mkl::rng::mrg32k3a>>();
  case random_engine_type::mt2203:
    return std::make_shared<
        rng::host::detail::rng_generator<oneapi::mkl::rng::mt2203>>();
  case random_engine_type::mt19937:
    return std::make_shared<
        rng::host::detail::rng_generator<oneapi::mkl::rng::mt19937>>();
  case random_engine_type::sobol:
    return std::make_shared<
        rng::host::detail::rng_generator<oneapi::mkl::rng::sobol>>();
  }
}
// DPCT_LABEL_END
} // namespace rng
} // namespace dpct

#endif // __DPCT_RNG_UTILS_HPP__
