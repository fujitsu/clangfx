// Start Fujitsu Extension: 3-D-003
// RUN: %clang -ffj-no-line -O1 -fvectorize -target x86_64-unknown-unknown -emit-llvm -Rpass-analysis -S %s -o - 2>&1 | FileCheck %s --check-prefix=RPASS
// End Fujitsu Extension: 3-D-003
// RUN: %clang -O1 -fvectorize -target x86_64-unknown-unknown -emit-llvm -S %s -o - 2>&1 | FileCheck %s

// Start Fujitsu Extension: 3-D-003
// RPASS: {{.*}}:11:8: remark: loop not vectorized: loop contains a switch statement
// CHECK-NOT: {{.*}}:11:8: remark: loop not vectorized: loop contains a switch statement
// End Fujitsu Extension: 3-D-003

double foo(int N, int *Array) {
  double v = 0.0;

  #pragma clang loop vectorize(enable)
  for (int i = 0; i < N; i++) {
    switch(Array[i]) {
    case 0: v += 1.0f; break;
    case 1: v -= 0.5f; break;
    case 2: v *= 2.0f; break;
    default: v = 0.0f;
    }
  }

  return v;
}
