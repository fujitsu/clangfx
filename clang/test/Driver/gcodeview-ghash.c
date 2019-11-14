// Note: %s must be preceded by --, otherwise it may be interpreted as a
// command-line option, e.g. on Mac where %s is commonly under /Users.

// GHASH: "-gcodeview-ghash"
// NO_GHASH-NOT: "-gcodeview-ghash"

// default
// RUN: %clang_cl /Z7 -### -- %s 2>&1 | FileCheck -check-prefix=NO_GHASH %s
// enabled
// RUN: %clang_cl /Z7 -gcodeview-ghash -### -- %s 2>&1 | FileCheck -check-prefix=GHASH %s
// disabled
// RUN: %clang_cl /Z7 -gcodeview-ghash -gno-codeview-ghash -### -- %s 2>&1 | FileCheck -check-prefix=NO_GHASH %s

// enabled, no /Z7
// Start Fujitsu Extension: 3-D-003
// RUN: %clang_cl -ffj-no-line -gcodeview-ghash -### -- %s 2>&1 | FileCheck -check-prefix=NO_GHASH %s
// End Fujitsu Extension: 3-D-003

// GCC-style driver
// RUN: %clang -g -gcodeview -gcodeview-ghash -### -- %s 2>&1 | FileCheck -check-prefix=GHASH %s
// RUN: %clang -g -gcodeview -gcodeview-ghash -gno-codeview-ghash -### -- %s 2>&1 | FileCheck -check-prefix=NO_GHASH %s
