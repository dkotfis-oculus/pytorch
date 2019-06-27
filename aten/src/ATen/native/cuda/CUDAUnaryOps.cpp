#include <ATen/ATen.h>
#include <ATen/LegacyTHFunctionsCUDA.h>
#include <ATen/MemoryOverlap.h>
#include <ATen/native/UnaryOps.h>
#include <ATen/native/TensorIterator.h>
#ifdef NAMEDTENSOR_ENABLED
#include <ATen/NamedTensorUtils.h>
#endif

namespace at { namespace native {

Tensor& _clamp__cuda(Tensor& self, optional<Scalar> min, optional<Scalar> max) {
  return _clamp_out_cuda(self, self, min, max);
}

Tensor& _clamp_out_cuda(
    Tensor& result,
    const Tensor& self,
    optional<Scalar> min,
    optional<Scalar> max) {
  if (min && max) {
    legacy::cuda::_th_clamp_out(result, self, *min, *max);
  } else if (max) {
    legacy::cuda::_th_clamp_max_out(result, self, *max);
  } else if (min) {
    legacy::cuda::_th_clamp_min_out(result, self, *min);
  } else {
    AT_ERROR("At least one of 'min' or 'max' must not be None");
  }
#ifdef NAMEDTENSOR_ENABLED
  at::namedinference::propagate_names(result, self);
#endif
  return result;
}

Tensor& _clamp_max__cuda(Tensor& self, Scalar max) {
  return legacy::cuda::_th_clamp_max_out(self, self, max);
}

Tensor& _clamp_max_out_cuda(Tensor& result, const Tensor& self, Scalar max) {
  legacy::cuda::_th_clamp_max_out(result, self, max);
#ifdef NAMEDTENSOR_ENABLED
  at::namedinference::propagate_names(result, self);
#endif
  return result;
}

Tensor& _clamp_min__cuda(Tensor& self, Scalar min) {
  return legacy::cuda::_th_clamp_min_out(self, self, min);
}

Tensor& _clamp_min_out_cuda(Tensor& result, const Tensor& self, Scalar min) {
  legacy::cuda::_th_clamp_min_out(result, self, min);
#ifdef NAMEDTENSOR_ENABLED
  at::namedinference::propagate_names(result, self);
#endif
  return result;
}

// These are just forwarding stubs

#define IMPLEMENT_UNARY_OP_PREQUEL(op)                           \
  Tensor& _##op##__cuda(Tensor& self) {                          \
    return legacy::cuda::_th_##op##_out(self, self);         \
  }                                                              \
  Tensor& _##op##_out_cuda(Tensor& result, const Tensor& self) { \
    return legacy::cuda::_th_##op##_out(result, self);       \
  }


IMPLEMENT_UNARY_OP_PREQUEL(abs)
IMPLEMENT_UNARY_OP_PREQUEL(acos)
IMPLEMENT_UNARY_OP_PREQUEL(asin)
IMPLEMENT_UNARY_OP_PREQUEL(atan)
IMPLEMENT_UNARY_OP_PREQUEL(ceil)
IMPLEMENT_UNARY_OP_PREQUEL(cos)
IMPLEMENT_UNARY_OP_PREQUEL(cosh)
IMPLEMENT_UNARY_OP_PREQUEL(erf)
IMPLEMENT_UNARY_OP_PREQUEL(erfc)
IMPLEMENT_UNARY_OP_PREQUEL(exp)
IMPLEMENT_UNARY_OP_PREQUEL(expm1)
IMPLEMENT_UNARY_OP_PREQUEL(frac)
IMPLEMENT_UNARY_OP_PREQUEL(floor)
IMPLEMENT_UNARY_OP_PREQUEL(log)
IMPLEMENT_UNARY_OP_PREQUEL(log10)
IMPLEMENT_UNARY_OP_PREQUEL(log1p)
IMPLEMENT_UNARY_OP_PREQUEL(log2)
IMPLEMENT_UNARY_OP_PREQUEL(neg)
IMPLEMENT_UNARY_OP_PREQUEL(reciprocal)
IMPLEMENT_UNARY_OP_PREQUEL(round)
IMPLEMENT_UNARY_OP_PREQUEL(rsqrt)
IMPLEMENT_UNARY_OP_PREQUEL(sigmoid)
IMPLEMENT_UNARY_OP_PREQUEL(sin)
IMPLEMENT_UNARY_OP_PREQUEL(sinh)
IMPLEMENT_UNARY_OP_PREQUEL(sqrt)
IMPLEMENT_UNARY_OP_PREQUEL(tan)
IMPLEMENT_UNARY_OP_PREQUEL(tanh)
IMPLEMENT_UNARY_OP_PREQUEL(trunc)

#define IMPLEMENT_UNARY_OP_VEC_CUDA(op)                          \
  Tensor& _##op##__cuda(Tensor& self) {                          \
    return at::op##_out(self, self);                             \
  }                                                              \
  Tensor& _##op##_out_cuda(Tensor& result, const Tensor& self) { \
    checkBackend(#op, {result}, Backend::CUDA);                  \
    assert_no_internal_overlap(result, #op);                     \
    auto iter = TensorIterator::unary_op(result, self);          \
    op##_stub(iter->device_type(), *iter);                       \
    propagate_names_if_namedtensor_enabled(result, self);        \
    return result;                                               \
  }

IMPLEMENT_UNARY_OP_VEC_CUDA(bitwise_not)

}}
