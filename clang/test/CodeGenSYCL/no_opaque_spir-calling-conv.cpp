// RUN: %clang_cc1 -fno-sycl-force-inline-kernel-lambda -fsycl-is-device -triple spir64-unknown-unknown -disable-llvm-passes -no-opaque-pointers -emit-llvm %s -o - | FileCheck %s

template <typename name, typename Func>
__attribute__((sycl_kernel)) void kernel_single_task(const Func &kernelFunc) {
  kernelFunc();
}

int main() {

  // CHECK: define {{.*}}spir_kernel void @_ZTSZ4mainE15kernel_function()

  // CHECK: call spir_func void @_ZZ4mainENKUlvE_clEv(%class.anon addrspace(4)* {{[^,]*}} %{{.+}})

  // CHECK: define internal spir_func void @_ZZ4mainENKUlvE_clEv(%class.{{.*}}anon addrspace(4)* {{[^,]*}} %this)

  kernel_single_task<class kernel_function>([]() {});
  return 0;
}
