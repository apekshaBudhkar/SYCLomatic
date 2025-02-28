// DPCT_LABEL_BEGIN|License|
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
//==---- util.hpp ---------------------------------*- C++ -*----------------==//
//
// Copyright (C) Intel Corporation
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// See https://llvm.org/LICENSE.txt for license information.
//
//===----------------------------------------------------------------------===//
// DPCT_LABEL_END

#ifndef __DPCT_UTIL_HPP__
#define __DPCT_UTIL_HPP__
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
#include <complex>
#include <type_traits>
#include <cassert>
#include <cstdint>
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|local_include_dependency|
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
// DPCT_LABEL_END

namespace dpct {

// DPCT_LABEL_BEGIN|err_types|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
namespace detail {

template <typename tag, typename T> class generic_error_type {
public:
  generic_error_type() = default;
  generic_error_type(T value) : value{value} {}
  operator T() const { return value; }

private:
  T value;
};

} // namespace detail

using err0 = detail::generic_error_type<struct err0_tag, int>;
using err1 = detail::generic_error_type<struct err1_tag, int>;
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|make_index_sequence|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
template <int... Ints> struct integer_sequence {};
template <int Size, int... Ints>
struct make_index_sequence
    : public make_index_sequence<Size - 1, Size - 1, Ints...> {};
template <int... Ints>
struct make_index_sequence<0, Ints...> : public integer_sequence<Ints...> {};
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|DataType|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
template <typename T> struct DataType { using T2 = T; };
template <typename T> struct DataType<sycl::vec<T, 2>> {
  using T2 = std::complex<T>;
};
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|matrix_mem_copy|dpct
// DPCT_DEPENDENCY_BEGIN
// Device|get_default_queue
// Memory|dpct_memcpy_detail
// Memory|dpct_memcpy_2d_pitch_detail
// Memory|memcpy_direction
// DPCT_DEPENDENCY_END
// DPCT_CODE
inline void matrix_mem_copy(void *to_ptr, const void *from_ptr, int to_ld,
                            int from_ld, int rows, int cols, int elem_size,
                            memcpy_direction direction = automatic,
                            sycl::queue &queue = dpct::get_default_queue(),
                            bool async = false) {
  if (to_ptr == from_ptr && to_ld == from_ld) {
    return;
  }

  if (to_ld == from_ld) {
    size_t copy_size = elem_size * ((cols - 1) * (size_t)to_ld + rows);
    if (async)
      detail::dpct_memcpy(queue, (void *)to_ptr, (void *)from_ptr,
                          copy_size, direction);
    else
      detail::dpct_memcpy(queue, (void *)to_ptr, (void *)from_ptr,
                          copy_size, direction).wait();
  } else {
    if (async)
      detail::dpct_memcpy(queue, to_ptr, from_ptr, elem_size * to_ld,
                          elem_size * from_ld, elem_size * rows, cols,
                          direction);
    else
      sycl::event::wait(detail::dpct_memcpy(
          queue, to_ptr, from_ptr, elem_size * to_ld, elem_size * from_ld,
          elem_size * rows, cols, direction));
  }
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|matrix_mem_copy_T|dpct
// DPCT_DEPENDENCY_BEGIN
// Device|get_default_queue
// Util|matrix_mem_copy
// Util|DataType
// Memory|memcpy_direction
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// Copy matrix data. The default leading dimension is column.
/// \param [out] to_ptr A pointer points to the destination location.
/// \param [in] from_ptr A pointer points to the source location.
/// \param [in] to_ld The leading dimension the destination matrix.
/// \param [in] from_ld The leading dimension the source matrix.
/// \param [in] rows The number of rows of the source matrix.
/// \param [in] cols The number of columns of the source matrix.
/// \param [in] direction The direction of the data copy.
/// \param [in] queue The queue where the routine should be executed.
/// \param [in] async If this argument is true, the return of the function
/// does NOT guarantee the copy is completed.
template <typename T>
inline void matrix_mem_copy(T *to_ptr, const T *from_ptr, int to_ld,
                            int from_ld, int rows, int cols,
                            memcpy_direction direction = automatic,
                            sycl::queue &queue = dpct::get_default_queue(),
                            bool async = false) {
  using Ty = typename DataType<T>::T2;
  matrix_mem_copy((void *)to_ptr, (void *)from_ptr, to_ld, from_ld, rows, cols,
                  sizeof(Ty), direction, queue, async);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|cast_double_to_int|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Cast the high or low 32 bits of a double to an integer.
/// \param [in] d The double value.
/// \param [in] use_high32 Cast the high 32 bits of the double if true;
/// otherwise cast the low 32 bits.
inline int cast_double_to_int(double d, bool use_high32 = true) {
  sycl::vec<double, 1> v0{d};
  auto v1 = v0.as<sycl::int2>();
  if (use_high32)
    return v1[1];
  return v1[0];
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|cast_ints_to_double|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Combine two integers, the first as the high 32 bits and the second
/// as the low 32 bits, into a double.
/// \param [in] high32 The integer as the high 32 bits
/// \param [in] low32 The integer as the low 32 bits
inline double cast_ints_to_double(int high32, int low32) {
  sycl::int2 v0{low32, high32};
  auto v1 = v0.as<sycl::vec<double, 1>>();
  return v1;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|fast_length|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute fast_length for variable-length array
/// \param [in] a The array
/// \param [in] len Length of the array
/// \returns The computed fast_length
inline float fast_length(const float *a, int len) {
  switch (len) {
  case 1:
    return sycl::fast_length(a[0]);
  case 2:
    return sycl::fast_length(sycl::float2(a[0], a[1]));
  case 3:
    return sycl::fast_length(sycl::float3(a[0], a[1], a[2]));
  case 4:
    return sycl::fast_length(sycl::float4(a[0], a[1], a[2], a[3]));
  case 0:
    return 0;
  default:
    float f = 0;
    for (int i = 0; i < len; ++i)
      f += a[i] * a[i];
    return sycl::sqrt(f);
  }
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|vectorized_max|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute vectorized max for two values, with each value treated as a vector
/// type \p S
/// \param [in] S The type of the vector
/// \param [in] T The type of the original values
/// \param [in] a The first value
/// \param [in] b The second value
/// \returns The vectorized max of the two values
template <typename S, typename T>
inline T vectorized_max(T a, T b) {
  sycl::vec<T, 1> v0{a}, v1{b};
  auto v2 = v0.template as<S>();
  auto v3 = v1.template as<S>();
  v2 = sycl::max(v2, v3);
  v0 = v2.template as<sycl::vec<T, 1>>();
  return v0;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|vectorized_min|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute vectorized min for two values, with each value treated as a vector
/// type \p S
/// \param [in] S The type of the vector
/// \param [in] T The type of the original values
/// \param [in] a The first value
/// \param [in] b The second value
/// \returns The vectorized min of the two values
template <typename S, typename T>
inline T vectorized_min(T a, T b) {
  sycl::vec<T, 1> v0{a}, v1{b};
  auto v2 = v0.template as<S>();
  auto v3 = v1.template as<S>();
  v2 = sycl::min(v2, v3);
  v0 = v2.template as<sycl::vec<T, 1>>();
  return v0;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|vectorized_isgreater_T|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute vectorized isgreater for two values, with each value treated as a
/// vector type \p S
/// \param [in] S The type of the vector
/// \param [in] T The type of the original values
/// \param [in] a The first value
/// \param [in] b The second value
/// \returns The vectorized greater than of the two values
template <typename S, typename T>
inline T vectorized_isgreater(T a, T b) {
  sycl::vec<T, 1> v0{a}, v1{b};
  auto v2 = v0.template as<S>();
  auto v3 = v1.template as<S>();
  auto v4 = sycl::isgreater(v2, v3);
  v0 = v4.template as<sycl::vec<T, 1>>();
  return v0;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|vectorized_isgreater_unsigned|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute vectorized isgreater for two unsigned int values, with each value
/// treated as a vector of two unsigned short
/// \param [in] a The first value
/// \param [in] b The second value
/// \returns The vectorized greater than of the two values
template<>
inline unsigned
vectorized_isgreater<sycl::ushort2, unsigned>(unsigned a, unsigned b) {
  sycl::vec<unsigned, 1> v0{a}, v1{b};
  auto v2 = v0.template as<sycl::ushort2>();
  auto v3 = v1.template as<sycl::ushort2>();
  sycl::ushort2 v4;
  v4[0] = v2[0] > v3[0] ? 0xffff : 0;
  v4[1] = v2[1] > v3[1] ? 0xffff : 0;
  v0 = v4.template as<sycl::vec<unsigned, 1>>();
  return v0;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|vectorized_isgreater_uchar4|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Compute vectorized isgreater for two unsigned int values, with each value
/// treated as a vector of four unsigned char.
/// \param [in] a The first value
/// \param [in] b The second value
/// \returns The vectorized greater than of the two values
template<>
inline unsigned
vectorized_isgreater<sycl::uchar4, unsigned>(unsigned a, unsigned b) {
  sycl::vec<unsigned, 1> v0{a}, v1{b};
  auto v2 = v0.template as<sycl::uchar4>();
  auto v3 = v1.template as<sycl::uchar4>();
  sycl::uchar4 v4;
  v4[0] = v2[0] > v3[0] ? 0xff : 0;
  v4[1] = v2[1] > v3[1] ? 0xff : 0;
  v4[2] = v2[2] > v3[2] ? 0xff : 0;
  v4[3] = v2[3] > v3[3] ? 0xff : 0;
  v0 = v4.template as<sycl::vec<unsigned, 1>>();
  return v0;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|reverse_bits|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Reverse the bit order of an unsigned integer
/// \param [in] a Input unsigned integer value
/// \returns Value of a with the bit order reversed
template <typename T> inline T reverse_bits(T a) {
  static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value,
                "unsigned integer required");
  if (!a)
    return 0;
  T mask = 0;
  size_t count = 4 * sizeof(T);
  mask = ~mask >> count;
  while (count) {
    a = ((a & mask) << count) | ((a & ~mask) >> count);
    count = count >> 1;
    mask = mask ^ (mask << count);
  }
  return a;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|byte_level_permute|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// \param [in] a The first value contains 4 bytes
/// \param [in] b The second value contains 4 bytes
/// \param [in] s The selector value, only lower 16bit used
/// \returns the permutation result of 4 bytes selected in the way
/// specified by \p s from \p a and \p b
inline unsigned int byte_level_permute(unsigned int a, unsigned int b,
                                       unsigned int s) {
  unsigned int ret;
  ret =
      ((((std::uint64_t)b << 32 | a) >> (s & 0x7) * 8) & 0xff) |
      (((((std::uint64_t)b << 32 | a) >> ((s >> 4) & 0x7) * 8) & 0xff) << 8) |
      (((((std::uint64_t)b << 32 | a) >> ((s >> 8) & 0x7) * 8) & 0xff) << 16) |
      (((((std::uint64_t)b << 32 | a) >> ((s >> 12) & 0x7) * 8) & 0xff) << 24);
  return ret;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|ffs|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Find position of first least significant set bit in an integer.
/// ffs(0) returns 0.
///
/// \param [in] a Input integer value
/// \returns The position
template <typename T> inline int ffs(T a) {
  static_assert(std::is_integral<T>::value, "integer required");
  return (sycl::ext::intel::ctz(a) + 1) % (sizeof(T) * 8 + 1);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|select_from_sub_group|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// select_from_sub_group allows work-items to obtain a copy of a value held by
/// any other work-item in the sub_group. The input sub_group will be divided
/// into several logical sub_groups with id range [0, \p logical_sub_group_size
/// - 1]. Each work-item in logical sub_group gets value from another work-item
/// whose id is \p remote_local_id. If \p remote_local_id is outside the
/// logical sub_group id range, \p remote_local_id will modulo with \p
/// logical_sub_group_size. The \p logical_sub_group_size must be a power of 2
/// and not exceed input sub_group size.
/// \tparam T Input value type
/// \param [in] g Input sub_group
/// \param [in] x Input value
/// \param [in] remote_local_id Input source work item id
/// \param [in] logical_sub_group_size Input logical sub_group size
/// \returns The result
template <typename T>
T select_from_sub_group(sycl::sub_group g, T x, int remote_local_id,
                        int logical_sub_group_size = 32) {
  unsigned int start_index =
      g.get_local_linear_id() / logical_sub_group_size * logical_sub_group_size;
  return sycl::select_from_group(
      g, x, start_index + remote_local_id % logical_sub_group_size);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|shift_sub_group_left|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// shift_sub_group_left move values held by the work-items in a sub_group
/// directly to another work-item in the sub_group, by shifting values a fixed
/// number of work-items to the left. The input sub_group will be divided into
/// several logical sub_groups with id range [0, \p logical_sub_group_size - 1].
/// Each work-item in logical sub_group gets value from another work-item whose
/// id is caller's id adds \p delta. If calculated id is outside the logical
/// sub_group id range, the work-item will get value from itself. The \p
/// logical_sub_group_size must be a power of 2 and not exceed input sub_group
/// size.
/// \tparam T Input value type
/// \param [in] g Input sub_group
/// \param [in] x Input value
/// \param [in] delta Input delta
/// \param [in] logical_sub_group_size Input logical sub_group size
/// \returns The result
template <typename T>
T shift_sub_group_left(sycl::sub_group g, T x, unsigned int delta,
                       int logical_sub_group_size = 32) {
  unsigned int id = g.get_local_linear_id();
  unsigned int end_index =
      (id / logical_sub_group_size + 1) * logical_sub_group_size;
  T result = sycl::shift_group_left(g, x, delta);
  if ((id + delta) >= end_index) {
    result = x;
  }
  return result;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|shift_sub_group_right|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// shift_sub_group_right move values held by the work-items in a sub_group
/// directly to another work-item in the sub_group, by shifting values a fixed
/// number of work-items to the right. The input sub_group will be divided into
/// several logical_sub_groups with id range [0, \p logical_sub_group_size - 1].
/// Each work-item in logical_sub_group gets value from another work-item whose
/// id is caller's id subtracts \p delta. If calculated id is outside the
/// logical sub_group id range, the work-item will get value from itself. The \p
/// logical_sub_group_size must be a power of 2 and not exceed input sub_group
/// size.
/// \tparam T Input value type
/// \param [in] g Input sub_group
/// \param [in] x Input value
/// \param [in] delta Input delta
/// \param [in] logical_sub_group_size Input logical sub_group size
/// \returns The result
template <typename T>
T shift_sub_group_right(sycl::sub_group g, T x, unsigned int delta,
                        int logical_sub_group_size = 32) {
  unsigned int id = g.get_local_linear_id();
  unsigned int start_index =
      id / logical_sub_group_size * logical_sub_group_size;
  T result = sycl::shift_group_right(g, x, delta);
  if ((id - start_index) < delta) {
    result = x;
  }
  return result;
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|permute_sub_group_by_xor|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// permute_sub_group_by_xor permutes values by exchanging values held by pairs
/// of work-items identified by computing the bitwise exclusive OR of the
/// work-item id and some fixed mask. The input sub_group will be divided into
/// several logical sub_groups with id range [0, \p logical_sub_group_size - 1].
/// Each work-item in logical sub_group gets value from another work-item whose
/// id is bitwise exclusive OR of the caller's id and \p mask. If calculated id
/// is outside the logical sub_group id range, the work-item will get value from
/// itself. The \p logical_sub_group_size must be a power of 2 and not exceed
/// input sub_group size.
/// \tparam T Input value type
/// \param [in] g Input sub_group
/// \param [in] x Input value
/// \param [in] mask Input mask
/// \param [in] logical_sub_group_size Input logical sub_group size
/// \returns The result
template <typename T>
T permute_sub_group_by_xor(sycl::sub_group g, T x, unsigned int mask,
                           int logical_sub_group_size = 32) {
  unsigned int id = g.get_local_linear_id();
  unsigned int start_index =
      id / logical_sub_group_size * logical_sub_group_size;
  unsigned int target_offset = (id % logical_sub_group_size) ^ mask;
  return sycl::select_from_group(g, x,
                                 target_offset < logical_sub_group_size
                                     ? start_index + target_offset
                                     : id);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|cmul|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Computes the multiplication of two complex numbers.
/// \tparam T Complex element type
/// \param [in] x The first input complex number
/// \param [in] y The second input complex number
/// \returns The result
template <typename T>
sycl::vec<T, 2> cmul(sycl::vec<T, 2> x, sycl::vec<T, 2> y) {
  std::complex<T> t1(x[0], x[1]), t2(y[0], y[1]);
  t1 = t1 * t2;
  return sycl::vec<T, 2>(t1.real(), t1.imag());
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|cdiv|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Computes the division of two complex numbers.
/// \tparam T Complex element type
/// \param [in] x The first input complex number
/// \param [in] y The second input complex number
/// \returns The result
template <typename T>
sycl::vec<T, 2> cdiv(sycl::vec<T, 2> x, sycl::vec<T, 2> y) {
  std::complex<T> t1(x[0], x[1]), t2(y[0], y[1]);
  t1 = t1 / t2;
  return sycl::vec<T, 2>(t1.real(), t1.imag());
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|cabs|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Computes the magnitude of a complex number.
/// \tparam T Complex element type
/// \param [in] x The input complex number
/// \returns The result
template <typename T>
T cabs(sycl::vec<T, 2> x) {
  std::complex<T> t(x[0], x[1]);
  return std::abs(t);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|conj|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Computes the complex conjugate of a complex number.
/// \tparam T Complex element type
/// \param [in] x The input complex number
/// \returns The result
template <typename T>
sycl::vec<T, 2> conj(sycl::vec<T, 2> x) {
  std::complex<T> t(x[0], x[1]);
  t = std::conj(t);
  return sycl::vec<T, 2>(t.real(), t.imag());
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|get_sycl_language_version|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
inline int get_sycl_language_version() {
#ifdef SYCL_LANGUAGE_VERSION
  return SYCL_LANGUAGE_VERSION;
#else
  return 202000;
#endif
}
// DPCT_LABEL_END

namespace experimental {
// DPCT_LABEL_BEGIN|nd_range_barrier|dpct::experimental
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
/// Synchronize work items from all work groups within a SYCL kernel.
/// \param [in] item:  Represents a work group.
/// \param [in] counter: An atomic object defined on a device memory which can
/// be accessed by work items in all work groups. The initial value of the
/// counter should be zero.
/// Note: Please make sure that all the work items of all work groups within
/// a SYCL kernel can be scheduled actively at the same time on a device.
template <int dimensions = 3>
inline void
nd_range_barrier(const sycl::nd_item<dimensions> &item,
                 sycl::atomic_ref<
                     unsigned int, sycl::memory_order::seq_cst,
                     sycl::memory_scope::device,
                     sycl::access::address_space::global_space> &counter) {

  static_assert(dimensions == 3, "dimensions must be 3.");

  unsigned int num_groups = item.get_group_range(2) * item.get_group_range(1) *
                            item.get_group_range(0);

  item.barrier();

  if (item.get_local_linear_id() == 0) {
    unsigned int inc = 1;
    unsigned int old_arrive = 0;
    bool is_group0 =
        (item.get_group(2) + item.get_group(1) + item.get_group(0) == 0);
    if (is_group0) {
      inc = 0x80000000 - (num_groups - 1);
    }

    old_arrive = counter.fetch_add(inc);
    // Synchronize all the work groups
    while (((old_arrive ^ counter.load()) & 0x80000000) == 0)
      ;
  }

  item.barrier();
}

/// Synchronize work items from all work groups within a SYCL kernel.
/// \param [in] item:  Represents a work group.
/// \param [in] counter: An atomic object defined on a device memory which can
/// be accessed by work items in all work groups. The initial value of the
/// counter should be zero.
/// Note: Please make sure that all the work items of all work groups within
/// a SYCL kernel can be scheduled actively at the same time on a device.
template <>
inline void
nd_range_barrier(const sycl::nd_item<1> &item,
                 sycl::atomic_ref<
                     unsigned int, sycl::memory_order::seq_cst,
                     sycl::memory_scope::device,
                     sycl::access::address_space::global_space> &counter) {
  unsigned int num_groups = item.get_group_range(0);

  item.barrier();

  if (item.get_local_linear_id() == 0) {
    unsigned int inc = 1;
    unsigned int old_arrive = 0;
    bool is_group0 = (item.get_group(0) == 0);
    if (is_group0) {
      inc = 0x80000000 - (num_groups - 1);
    }

    old_arrive = counter.fetch_add(inc);
    // Synchronize all the work groups
    while (((old_arrive ^ counter.load()) & 0x80000000) == 0)
      ;
  }

  item.barrier();
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|logical_group|dpct::experimental
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group_1
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// The logical-group is a logical collection of some work-items within a
/// work-group.
/// Note: Please make sure that the logical-group size is a power of 2 in the
/// range [1, current_sub_group_size].
class logical_group {
  sycl::nd_item<3> _item;
  sycl::group<3> _g;
  uint32_t _logical_group_size;
  uint32_t _group_linear_range_in_parent;

public:
  /// Dividing \p parent_group into several logical-groups.
  /// \param [in] item Current work-item.
  /// \param [in] parent_group The group to be divided.
  /// \param [in] size The logical-group size.
  logical_group(sycl::nd_item<3> item, sycl::group<3> parent_group,
                uint32_t size)
      : _item(item), _g(parent_group), _logical_group_size(size) {
    _group_linear_range_in_parent =
        (_g.get_local_linear_range() - 1) / _logical_group_size + 1;
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|logical_group_get_local_linear_id|dpct::experimental
// DPCT_PARENT_FEATURE|logical_group
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Returns the index of the work-item within the logical-group.
  uint32_t get_local_linear_id() const {
    return _item.get_local_linear_id() % _logical_group_size;
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|logical_group_get_group_linear_id|dpct::experimental
// DPCT_PARENT_FEATURE|logical_group
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Returns the index of the logical-group in the parent group.
  uint32_t get_group_linear_id() const {
    return _item.get_local_linear_id() / _logical_group_size;
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|logical_group_get_local_linear_range|dpct::experimental
// DPCT_PARENT_FEATURE|logical_group
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Returns the number of work-items in the logical-group.
  uint32_t get_local_linear_range() const {
    if (_g.get_local_linear_range() % _logical_group_size == 0) {
      return _logical_group_size;
    }
    uint32_t last_item_group_id =
        _g.get_local_linear_range() / _logical_group_size;
    uint32_t first_of_last_group = last_item_group_id * _logical_group_size;
    if (_item.get_local_linear_id() >= first_of_last_group) {
      return _g.get_local_linear_range() - first_of_last_group;
    } else {
      return _logical_group_size;
    }
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|logical_group_get_group_linear_range|dpct::experimental
// DPCT_PARENT_FEATURE|logical_group
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group
// DPCT_DEPENDENCY_END
// DPCT_CODE
  /// Returns the number of logical-group in the parent group.
  uint32_t get_group_linear_range() const {
    return _group_linear_range_in_parent;
  }
// DPCT_LABEL_END
// DPCT_LABEL_BEGIN|logical_group_1|dpct::experimental
// DPCT_PARENT_FEATURE|logical_group
// DPCT_DEPENDENCY_BEGIN
// Util|logical_group
// DPCT_DEPENDENCY_END
// DPCT_CODE
};
// DPCT_LABEL_END
} // namespace experimental

// DPCT_LABEL_BEGIN|int_as_queue_ptr|dpct
// DPCT_DEPENDENCY_BEGIN
// Device|typedef_queue_ptr
// Device|get_default_queue
// DPCT_DEPENDENCY_END
// DPCT_CODE
/// If x <= 2, then return a pointer to the deafult queue;
/// otherwise, return x reinterpreted as a dpct::queue_ptr.
inline queue_ptr int_as_queue_ptr(uintptr_t x) {
  return x <= 2 ?
  &get_default_queue()
  : reinterpret_cast<queue_ptr>(x);
}
// DPCT_LABEL_END

// DPCT_LABEL_BEGIN|kernel_wrapper|dpct
// DPCT_DEPENDENCY_EMPTY
// DPCT_CODE
template <int n_nondefault_params, int n_default_params, typename T>
class args_selector;

/// args_selector is a helper class for extracting arguments from an
/// array of pointers to arguments or buffer of arguments to pass to a
/// kernel function.
///
/// \param R(Ts...) The type of the kernel
/// \param n_nondefault_params The number of nondefault parameters of the kernel
/// (excluding parameters that like sycl::nd_item, etc.)
/// \param n_default_params The number of default parameters of the kernel
///
/// Example usage:
/// With the following kernel:
///   void foo(sycl::float2 *x, int n, sycl::nd_item<3> item_ct1, float f=.1) {}
/// and with the declaration:
///   args_selector<2, 1, decltype(foo)> selector(kernelParams, extra);
/// we have:
///   selector.get<0>() returns a reference to sycl::float*,
///   selector.get<1>() returns a reference to int,
///   selector.get<2>() returns a reference to float
template <int n_nondefault_params, int n_default_params,
   typename R, typename... Ts>
class args_selector<n_nondefault_params, n_default_params, R(Ts...)> {
private:
  void **kernel_params;
  char *args_buffer;

  template <int i>
  static constexpr int account_for_default_params() {
    constexpr int n_total_params = sizeof...(Ts);
    if constexpr (i >= n_nondefault_params) {
      return n_total_params - n_default_params + (i - n_nondefault_params);
    } else {
      return i;
    }
  }    

public:
  /// Get the type of the ith argument of R(Ts...)
  /// \param [in] i Index of parameter to get
  /// \returns Type of ith parameter
  template <int i>
  using arg_type = std::tuple_element_t<account_for_default_params<i>(),
					  std::tuple<Ts...>>;
private:
  template <int i>
  static constexpr int get_offset() {
    if constexpr (i == 0) {
      // we can assume args_buffer is properly aligned to the
      // first argument
      return 0;
    } else {
      constexpr int prev_off = get_offset<i-1>();
      constexpr int prev_past_end = prev_off + sizeof(arg_type<i-1>);
      using T = arg_type<i>;
      // is the past-the-end of the i-1st element properly aligned
      // with the ith element's alignment?
      if constexpr (prev_past_end % alignof(T) == 0) {
	return prev_past_end;
      }
      // otherwise bump prev_past_end to match alignment
      else {
	return prev_past_end + (alignof(T) - (prev_past_end % alignof(T)));
      }
    }
  }

  static char *get_args_buffer(void **extra) {
    if (!extra)
      return nullptr;
    for (; (std::size_t) *extra != 0; ++extra) {
      if ((std::size_t) *extra == 1) {
	return static_cast<char*>(*(extra+1));
      }
    }
    return nullptr;
  }
    
public:
  /// If kernel_params is nonnull, then args_selector will
  /// extract arguments from kernel_params. Otherwise, it
  /// will extract them from extra.
  /// \param [in] kernel_params Array of pointers to arguments
  /// a or null pointer.
  /// \param [in] extra Array containing pointer to argument buffer.
  args_selector(void **kernel_params, void **extra)
    : kernel_params(kernel_params),
      args_buffer(get_args_buffer(extra))
  {}

  /// Get a reference to the ith argument extracted from kernel_params
  /// or extra.
  /// \param [in] i Index of argument to get
  /// \returns Reference to the ith argument
  template <int i>    
  arg_type<i> &get() {
    if (kernel_params) {
      return *static_cast<arg_type<i>*>(kernel_params[i]);
    } else {
      return *reinterpret_cast<arg_type<i>*>(args_buffer + get_offset<i>());
    }
  }
};

#ifdef _WIN32
#define DPCT_EXPORT __declspec(dllexport)
#else
#define DPCT_EXPORT
#endif
// DPCT_LABEL_END

} // namespace dpct



#endif // __DPCT_UTIL_HPP__
