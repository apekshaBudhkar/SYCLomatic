; TODO: switch to opaque pointers once llvm-spirv tool is able to handle them.

; Check for passthrough abilities
; RUN: llvm-as -opaque-pointers=0 %s -o %t.bc
; RUN: spirv-to-ir-wrapper %t.bc -o %t_1.bc -skip-unknown-input
; RUN: llvm-dis -opaque-pointers=0 %t_1.bc -o %t_1.ll
; RUN: FileCheck %s --input-file %t_1.ll

; Check for SPIR-V conversion
; RUN: llvm-spirv -opaque-pointers=0 %t.bc -o %t.spv
; RUN: spirv-to-ir-wrapper -llvm-spirv-opts "-emit-opaque-pointers=false" %t.spv -o %t_2.bc
; RUN: llvm-dis -opaque-pointers=0 %t_2.bc -o %t_2.ll
; RUNx: FileCheck %s --input-file %t_2.ll

; CHECK: target datalayout
; CHECK-NEXT: target triple = "spir-unknown-unknown"
; CHECK: Function Attrs: nounwind
; CHECK-NEXT: define spir_kernel void @foo(i32 addrspace(1)* %a)
; CHECK-NEXT: entry:
; CHECK-NEXT: %a.addr = alloca i32 addrspace(1)*, align 4
; CHECK-NEXT: store i32 addrspace(1)* %a, i32 addrspace(1)** %a.addr, align 4
; CHECK-NEXT: ret void

target datalayout = "e-p:32:32-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024"
target triple = "spir-unknown-unknown"

; Function Attrs: nounwind
define spir_kernel void @foo(i32 addrspace(1)* %a) #0 !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !4 !kernel_arg_type_qual !5 {
entry:
  %a.addr = alloca i32 addrspace(1)*, align 4
  store i32 addrspace(1)* %a, i32 addrspace(1)** %a.addr, align 4
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!opencl.enable.FP_CONTRACT = !{}
!opencl.spir.version = !{!6}
!opencl.ocl.version = !{!6}
!opencl.used.extensions = !{!7}
!opencl.used.optional.core.features = !{!7}
!opencl.compiler.options = !{!7}

!1 = !{i32 1}
!2 = !{!"none"}
!3 = !{!"int*"}
!4 = !{!"int*"}
!5 = !{!""}
!6 = !{i32 1, i32 2}
!7 = !{}

