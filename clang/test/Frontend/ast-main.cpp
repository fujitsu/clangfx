// Start Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -ffj-no-line -emit-llvm -S -o %t1.ll -x c++ - < %s
// End Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -fno-delayed-template-parsing -emit-ast -o %t.ast %s
// Start Fujitsu Extension: 3-D-003
// RUN: env SDKROOT="/" %clang -ffj-no-line -emit-llvm -S -o %t2.ll -x ast - < %t.ast
// End Fujitsu Extension: 3-D-003
// RUN: diff %t1.ll %t2.ll

// http://llvm.org/bugs/show_bug.cgi?id=15377
template<typename T>
struct S {
    T *mf();
};
template<typename T>
T *S<T>::mf() {
    // warning: non-void function does not return a value [-Wreturn-type]
}

void f() {
    S<int>().mf();
}

int main() {
  return 0;
}
