#include <immintrin.h>
#include <iostream>

int main() {
    std::cout << "AVX supported: "
              << (__builtin_cpu_supports("avx") ? "yes" : "no")
              << "\n";

    std::cout << "AVX2 supported: "
              << (__builtin_cpu_supports("avx2") ? "yes" : "no")
              << "\n";

    std::cout << "AVX-512 supported: "
              << (__builtin_cpu_supports("avx512f") ? "yes" : "no")
              << "\n";
}