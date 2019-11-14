// Start Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -ffj-no-line -emit-llvm -S -o %t1.ll -x c - < %s
// End Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -emit-ast -o %t.ast %s
// Start Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -ffj-no-line -emit-llvm -S -o %t2.ll -x ast - < %t.ast
// End Fujitsu Extension: 3-D-003
// RUN: diff %t1.ll %t2.ll

int main() {
  return 0;
}
