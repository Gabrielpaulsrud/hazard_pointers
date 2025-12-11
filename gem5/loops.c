#include <stdio.h>
#include <stdint.h>

int main() {
    volatile uint64_t sum = 0;

    for (uint64_t i = 0; i < 100ULL; i++) {
        sum += (i & 1);
    }

    printf("sum = %llu\n", (unsigned long long)sum);
    return 0;
}
