// RUN: %clang -g -S -emit-llvm -o - %s | FileCheck %s
// Start Fujitsu Extension: 3-D-003
// RUN: %clang -ffj-no-line -S -emit-llvm -o - %s | FileCheck %s --check-prefix=NO_DEBUG
// End Fujitsu Extension: 3-D-003
int main (void) {
  return 0;
}

// CHECK:  i32 2, !"Debug Info Version", i32 3}
// NO_DEBUG-NOT: !"Debug Info Version"
