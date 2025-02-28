; RUN: %clang -Xclang -opaque-pointers -fdiscard-value-names -S %s -o /dev/null 2>&1 | FileCheck --check-prefix=WARNING %s
; RUN: %clang -Xclang -opaque-pointers -S %s -o /dev/null 2>&1 | FileCheck --check-prefix=NOWARNING %s
; RUN: %clang_cc1 -opaque-pointers -S -emit-llvm %s -discard-value-names -o /dev/null
; PR 44896

; WARNING: ignoring -fdiscard-value-names for LLVM Bitcode
; NOWARNING-NOT: ignoring -fdiscard-value-names for LLVM Bitcode

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64--linux-gnu"

define linkonce ptr @b(ptr %a) {
  ret ptr %a
}

