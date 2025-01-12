// RUN: %clang %s -dM -E -target x86_64-windows | FileCheck %s --check-prefix=NO
// Start Fujitsu Extension: 3-D-003
// RUN: %clang -ffj-no-line %s -dM -E -target x86_64 -fno-asynchronous-unwind-tables | FileCheck %s --check-prefix=NO
// End Fujitsu Extension: 3-D-003

// RUN: %clang %s -dM -E -target x86_64 | FileCheck %s
// RUN: %clang %s -dM -E -target aarch64-apple-darwin | FileCheck %s
// RUN: %clang %s -dM -E -target x86_64 -fno-asynchronous-unwind-tables -g | FileCheck %s
// RUN: %clang %s -dM -E -target x86_64 -fno-asynchronous-unwind-tables -fexceptions | FileCheck %s

// NO-NOT: #define __GCC_HAVE_DWARF2_CFI_ASM
// CHECK: #define __GCC_HAVE_DWARF2_CFI_ASM 1
