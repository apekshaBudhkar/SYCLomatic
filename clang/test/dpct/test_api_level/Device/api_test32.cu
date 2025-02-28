// RUN: dpct --format-range=none  --usm-level=none  --use-custom-helper=api -out-root %T/Device/api_test32_out %s --cuda-include-path="%cuda-path/include" -- -x cuda --cuda-host-only
// RUN: grep "IsCalled" %T/Device/api_test32_out/MainSourceFiles.yaml | wc -l > %T/Device/api_test32_out/count.txt
// RUN: FileCheck --input-file %T/Device/api_test32_out/count.txt --match-full-lines %s
// RUN: rm -rf %T/Device/api_test32_out

#include <nccl.h>

// CHECK: 4
// TEST_FEATURE: Device_get_device_id
int main() {
  int  device;
  ncclComm_t comm;
}