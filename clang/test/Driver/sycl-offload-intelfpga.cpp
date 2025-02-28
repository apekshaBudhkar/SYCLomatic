///
/// tests specific to -fintelfpga -fsycl
///

/// Check SYCL headers path
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-HEADERS-INTELFPGA %s
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fsycl-targets=spir64_fpga-unknown-unknown %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-HEADERS-INTELFPGA %s
// CHK-HEADERS-INTELFPGA: clang{{.*}} "-internal-isystem" "{{.*}}bin{{[/\\]+}}..{{[/\\]+}}include{{[/\\]+}}sycl"

/// -fintelfpga implies -g and -MMD
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga -Xshardware %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-TOOLS-INTELFPGA %s
// CHK-TOOLS-INTELFPGA: clang{{.*}} "-debug-info-kind=constructor" {{.*}} "-dependency-file"
// CHK-TOOLS-INTELFPGA: aoc{{.*}} "-dep-files={{.*}}"

/// -fintelfpga implies -g but -g0 should override
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -g0 -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-TOOLS-INTELFPGA-G0 %s
// CHK-TOOLS-INTELFPGA-G0-NOT: clang{{.*}} "-debug-info-kind=constructor"

/// -fintelfpga passes it to host and device cc1 compilations
// RUN:   %clangxx -### -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-HOST-DEVICE %s
// CHK-HOST-DEVICE: clang{{.*}} "-cc1"{{.*}} "-fsycl-is-device"{{.*}} "-fintelfpga"
// CHK-HOST-DEVICE: clang{{.*}} "-cc1"{{.*}} "-fintelfpga"{{.*}} "-fsycl-is-host"

/// FPGA target implies -fsycl-disable-range-rounding
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-RANGE-ROUNDING %s
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fsycl-targets=spir64_fpga-unknown-unknown %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-RANGE-ROUNDING %s
// CHK-RANGE-ROUNDING: clang{{.*}} "-fsycl-is-device"{{.*}} "-fsycl-disable-range-rounding"
// CHK-RANGE-ROUNDING: clang{{.*}} "-fsycl-disable-range-rounding"{{.*}} "-fsycl-is-host"

/// FPGA target implies -emit-only-kernels-as-entry-points in sycl-post-link
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-NON-KERNEL-ENTRY-POINTS %s
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fsycl-targets=spir64_fpga-unknown-unknown %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-NON-KERNEL-ENTRY-POINTS %s
// CHK-NON-KERNEL-ENTRY-POINTS: sycl-post-link{{.*}} "-emit-only-kernels-as-entry-points"

/// Non-FPGA targets should not imply -emit-only-kernels-as-entry-points in sycl-post-link
// RUN:   %clang -### -fsycl -fsycl-targets=spir64_fpga,spir64_gen %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=CHK-NON-KERNEL-ENTRY-POINTS-NEG-1
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-1: clang{{.*}} "-triple" "spir64_fpga-unknown-unknown"
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-1: sycl-post-link{{.*}} "-emit-only-kernels-as-entry-points"
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-1: clang{{.*}} "-triple" "spir64_gen-unknown-unknown"
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-1: sycl-post-link
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-1-NOT: "-emit-only-kernels-as-entry-points"
// RUN:   %clang -### -fsycl %s 2>&1 | FileCheck %s --check-prefix=CHK-NON-KERNEL-ENTRY-POINTS-NEG-2
// CHK-NON-KERNEL-ENTRY-POINTS-NEG-2-NOT: "-emit-only-kernels-as-entry-points"

/// -fsycl-disable-range-rounding is applied to all compilations if fpga is used
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fsycl-targets=spir64_fpga-unknown-unknown,spir64_gen-unknown-unknown %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-RANGE-ROUNDING-MULTI %s
// CHK-RANGE-ROUNDING-MULTI: clang{{.*}} "-triple" "spir64_fpga-unknown-unknown"{{.*}} "-fsycl-is-device"{{.*}} "-fsycl-disable-range-rounding"
// CHK-RANGE-ROUNDING-MULTI: clang{{.*}} "-triple" "spir64_gen-unknown-unknown"{{.*}} "-fsycl-is-device"{{.*}} "-fsycl-disable-range-rounding"
// CHK-RANGE-ROUNDING-MULTI: clang{{.*}} "-fsycl-disable-range-rounding"{{.*}} "-fsycl-is-host"

/// -fintelfpga with -reuse-exe=
// RUN:  touch %t.cpp
// RUN:  %clangxx -### -reuse-exe=testing -target x86_64-unknown-linux-gnu -fsycl -fintelfpga -Xshardware %t.cpp 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-REUSE-EXE %s
// RUN:  %clang_cl -### -reuse-exe=testing -fsycl -fintelfpga -Xshardware %t.cpp 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-REUSE-EXE %s
// CHK-FPGA-REUSE-EXE: aoc{{.*}} "-o" {{.*}} "-sycl" {{.*}} "-reuse-exe=testing"

/// -fintelfpga dependency file generation test
// RUN: touch %t-1.cpp
// RUN: touch %t-2.cpp
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t-1.cpp %t-2.cpp -o %t.out 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t-1.cpp %t-2.cpp -o %t.out 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES %s
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t-1.cpp %t-2.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t-1.cpp %t-2.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES %s
// CHK-FPGA-DEP-FILES: clang{{.*}} "-dependency-file" "[[INPUT1:.+\.d]]" "-MT" "{{.*}}.o"
// CHK-FPGA-DEP-FILES: clang{{.*}} "-dependency-file" "[[INPUT2:.+\.d]]" "-MT" "{{.*}}.o"
// CHK-FPGA-DEP-FILES: aoc{{.*}} "-dep-files={{.*}}[[INPUT1]],{{.*}}[[INPUT2]]"
// CHK-FPGA-DEP-FILES-NOT: clang{{.*}} "-fsycl-is-host"{{.*}} "-dependency-file"

/// -fintelfpga dependency file check with host .d enabled
// RUN: %clangxx -### -MMD -fsycl -fintelfpga -Xshardware %t-1.cpp %t-2.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES-HOST %s
// CHK-FPGA-DEP-FILES-HOST: clang{{.*}} "-triple" "spir64_fpga-unknown-unknown"{{.*}} "-dependency-file" "[[INPUT1:.+\.d]]" "-MT" "{{.*}}.o"
// CHK-FPGA-DEP-FILES-HOST: clang{{.*}} "-triple" "spir64_fpga-unknown-unknown"{{.*}} "-dependency-file" "[[INPUT2:.+\.d]]" "-MT" "{{.*}}.o"
// CHK-FPGA-DEP-FILES-HOST: aoc{{.*}} "-dep-files={{.*}}[[INPUT1]],{{.*}}[[INPUT2]]"
// CHK-FPGA-DEP-FILES-HOST: clang{{.*}} "-fsycl-is-host"{{.*}}
// CHK-FPGA-DEP-FILES-HOST: clang{{.*}} "-fsycl-is-host"{{.*}}

/// -fintelfpga dependency file generation test to object
// RUN: %clangxx -### -fsycl -fintelfpga -target x86_64-unknown-linux-gnu %t-1.cpp %t-2.cpp -c 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-DEP-FILES2,CHK-FPGA-DEP-FILES2-LIN %s
// RUN: %clang_cl -### -fsycl -fintelfpga --target=x86_64-pc-windows-msvc %t-1.cpp %t-2.cpp -c 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-DEP-FILES2,CHK-FPGA-DEP-FILES2-WIN %s
// CHK-FPGA-DEP-FILES2: clang{{.*}} "-dependency-file" "[[INPUT1:.+\.d]]"
// CHK-FPGA-DEP-FILES2-LIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-unknown-linux-gnu,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.o" "-input=[[INPUT1]]"
// CHK-FPGA-DEP-FILES2-WIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-pc-windows-msvc,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.obj" "-input=[[INPUT1]]"
// CHK-FPGA-DEP-FILES2: clang{{.*}} "-dependency-file" "[[INPUT2:.+\.d]]"
// CHK-FPGA-DEP-FILES2-LIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-unknown-linux-gnu,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.o" "-input=[[INPUT2]]"
// CHK-FPGA-DEP-FILES2-WIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-pc-windows-msvc,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.obj" "-input=[[INPUT2]]"

/// -fintelfpga dependency file test to object with output designator
// RUN: touch %t-1.cpp
// RUN: %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %t-1.cpp -c -o dummy.o 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-DEP-FILES3,CHK-FPGA-DEP-FILES3-LIN %s
// RUN: %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %t-1.cpp -c -MMD -MF"dummy.d" 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-DEP-FILES3,CHK-FPGA-DEP-FILES3-LIN %s
// RUN: %clang_cl -### --target=x86_64-pc-windows-msvc -fsycl -fintelfpga %t-1.cpp -c -Fodummy.obj 2>&1 \
// RUN:  | FileCheck -check-prefixes=CHK-FPGA-DEP-FILES3,CHK-FPGA-DEP-FILES3-WIN %s
// CHK-FPGA-DEP-FILES3: clang{{.*}} "-triple" "spir64_fpga-unknown-unknown"{{.*}} "-dependency-file" "[[OUTPUT:.+\.d]]"
// CHK-FPGA-DEP-FILES3-LIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-unknown-linux-gnu,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.o" "-input=[[OUTPUT]]"
// CHK-FPGA-DEP-FILES3-WIN: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-spir64_fpga-unknown-unknown,host-x86_64-pc-windows-msvc,sycl-fpga_dep" {{.*}} "-input={{.*}}.bc" "-input={{.*}}.obj" "-input=[[OUTPUT]]"

/// -fintelfpga dependency obj use test
// RUN: touch %t-1.o
// RUN: touch %t-2.o
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware -target x86_64-unknown-linux-gnu %t-1.o %t-2.o 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES-OBJ %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t-1.o %t-2.o 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES-OBJ %s
// CHK-FPGA-DEP-FILES-OBJ: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-fpga_dep" "-input={{.*}}-1.o" "-output=[[DEPFILE1:.+\.d]]" "-unbundle"
// CHK-FPGA-DEP-FILES-OBJ: clang-offload-bundler{{.*}} "-type=o" "-targets=sycl-fpga_dep" "-input={{.*}}-2.o" "-output=[[DEPFILE2:.+\.d]]" "-unbundle"
// CHK-FPGA-DEP-FILES-OBJ: aoc{{.*}} "-dep-files=[[DEPFILE1]],[[DEPFILE2]]

/// -fintelfpga dependency file use from object phases test
// RUN: touch %t-1.o
// RUN: %clangxx -fsycl -fno-sycl-instrument-device-code -fno-sycl-device-lib=all -fintelfpga -ccc-print-phases %t-1.o 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES-OBJ-PHASES %s
// RUN: %clang_cl -fsycl -fno-sycl-instrument-device-code -fno-sycl-device-lib=all -fintelfpga -ccc-print-phases %t-1.o 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-DEP-FILES-OBJ-PHASES %s
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 0: input, "{{.*}}-1.o", object, (host-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 1: clang-offload-unbundler, {0}, object, (host-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 2: linker, {1}, image, (host-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 3: spirv-to-ir-wrapper, {1}, ir, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 4: linker, {3}, ir, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 5: sycl-post-link, {4}, tempfiletable, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 6: file-table-tform, {5}, tempfilelist, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 7: llvm-spirv, {6}, tempfilelist, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 8: clang-offload-unbundler, {0}, fpga_dep
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 9: backend-compiler, {7, 8}, fpga_aocx, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 10: file-table-tform, {5, 9}, tempfiletable, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 11: clang-offload-wrapper, {10}, object, (device-sycl)
// CHK-FPGA-DEP-FILES-OBJ-PHASES: 12: offload, "host-sycl ({{.*}})" {2}, "device-sycl (spir64_fpga-unknown-unknown)" {11}, image

/// -fintelfpga output report file test
// RUN: mkdir -p %t_dir
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.out 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.out 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.out 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.o 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.o 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.o 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.a 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.a 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.a 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.lib 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.lib 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.lib 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.obj 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.obj 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.obj 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.exe 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.exe 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.exe 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT %s

// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.xxx 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT-KEEP-EXT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -o %t_dir/file.xxx 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT-KEEP-EXT %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %s -Fe%t_dir/file.xxx 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT-KEEP-EXT %s
// CHK-FPGA-REPORT-OPT: aoc{{.*}} "-sycl" {{.*}} "-output-report-folder={{.*}}{{(/|\\\\)}}file.prj"
// CHK-FPGA-REPORT-OPT-KEEP-EXT: aoc{{.*}} "-sycl" {{.*}} "-output-report-folder={{.*}}{{(/|\\\\)}}file.xxx.prj"

/// -fintelfpga output report file from dir/source
/// check dependency file from dir/source
// RUN: mkdir -p %t_dir
// RUN: touch %t_dir/dummy.cpp
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t_dir/dummy.cpp  2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT2 %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t_dir/dummy.cpp 2>&1 \
// RUN:  | FileCheck -DOUTDIR=%t_dir -check-prefix=CHK-FPGA-REPORT-OPT2 %s
// CHK-FPGA-REPORT-OPT2: aoc{{.*}} "-sycl"{{.*}} "-dep-files={{.+}}dummy-{{.+}}.d" "-output-report-folder={{.*}}dummy.prj"
// CHK-FPGA-REPORT-OPT2-NOT: aoc{{.*}} "-sycl" {{.*}}_dir{{.*}}

/// -fintelfpga dependency files from multiple source
// RUN: touch dummy2.cpp
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t_dir/dummy.cpp dummy2.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-MULTI-DEPS %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t_dir/dummy.cpp dummy2.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-MULTI-DEPS %s
// CHK-FPGA-MULTI-DEPS: aoc{{.*}} "-sycl"{{.*}} "-dep-files={{.+}}dummy-{{.+}}.d,{{.+}}dummy2-{{.+}}.d" "-output-report-folder={{.*}}dummy.prj"

/// -fintelfpga output report file should be based on first input (src/obj)
// RUN: mkdir -p %t_dir
// RUN: touch %t_dir/dummy1.cpp
// RUN: touch %t_dir/dummy2.cpp
// RUN: touch %t_dir/dummy1.o
// RUN: touch %t_dir/dummy2.o
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t_dir/dummy2.o %t_dir/dummy1.cpp  2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-REPORT-NAME %s
// RUN: %clangxx -### -fsycl -fintelfpga -Xshardware %t_dir/dummy2.cpp %t_dir/dummy1.o  2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-REPORT-NAME %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t_dir/dummy2.o %t_dir/dummy1.cpp 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-REPORT-NAME %s
// RUN: %clang_cl -### -fsycl -fintelfpga -Xshardware %t_dir/dummy2.cpp %t_dir/dummy1.o 2>&1 \
// RUN:  | FileCheck -check-prefix=CHK-FPGA-REPORT-NAME %s
// CHK-FPGA-REPORT-NAME: aoc{{.*}} "-sycl"{{.*}} "-output-report-folder={{.*}}dummy2.prj"

/// Check for implied options with -Xshardware (-g -O0)
/// Expectation is for -O0 to not be used with -Xshardware
// RUN:   %clang -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga -g -O0 -Xs "-DFOO1 -DFOO2" -Xshardware %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-TOOLS-IMPLIED-OPTS %s
// RUN:   %clang_cl -### -fsycl -fintelfpga -Zi -Od -Xs "-DFOO1 -DFOO2" -Xshardware %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-TOOLS-IMPLIED-OPTS %s
// CHK-TOOLS-IMPLIED-OPTS-NOT: clang{{.*}} "-fsycl-is-device"{{.*}} "-O0"
// CHK-TOOLS-IMPLIED-OPTS: sycl-post-link{{.*}} "-O2"
// CHK-TOOLS-IMPLIED-OPTS: aoc{{.*}} "-g" "-DFOO1" "-DFOO2"

/// shared objects should not be checked for FPGA contents
// RUN: touch %t.so
// RUN: %clangxx -fsycl -fintelfpga %t.so -### 2>&1 \
// RUN:   | FileCheck -check-prefix=ERROR_BUNDLE_CHECK %s
// ERROR_BUNDLE_CHECK-NOT: clang-offload-bundler{{.*}} "-targets=sycl-fpga_aoc{{(x|r|r_emu|o)}}-intel-unknown"{{.*}} "-check-section"
// ERROR_BUNDLE_CHECK-NOT: error: file too small to be an archive

/// Implied default device should not prevent hardware/simulation
// RUN:  %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga -Xshardware %S/Inputs/SYCL/liblin64.a %s 2>&1 \
// RUN:    | FileCheck -check-prefix IMPLIED_DEVICE_HARDWARE -DBEOPT=hardware %s
// RUN:  %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga -Xssimulation %S/Inputs/SYCL/liblin64.a %s 2>&1 \
// RUN:    | FileCheck -check-prefix IMPLIED_DEVICE_HARDWARE -DBEOPT=simulation %s
// IMPLIED_DEVICE_HARDWARE: aoc{{.*}} "-[[BEOPT]]"

/// -fno-discard-value-names is default for FPGA
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fintelfpga %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-DISCARD-VALUE-NAMES %s
// RUN:   %clangxx -### -target x86_64-unknown-linux-gnu -fsycl -fsycl-targets=spir64_fpga-unknown-unknown %s 2>&1 \
// RUN:   | FileCheck -check-prefix=CHK-DISCARD-VALUE-NAMES %s
// CHK-DISCARD-VALUE-NAMES-NOT: clang{{.*}} "-fsycl-is-device"{{.*}} "-discard-value-names"
