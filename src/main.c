// S25I - Miguel Ignacio

#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kernel_c.h"

// x86-64 assembly kernel declaration
extern void distance_asm(int n, const double *X1, const double *X2,
                         const double *Y1, const double *Y2, double *Z);

#define NUM_RUNS 30
#define EPSILON 1e-6

// Helper for timing
static double get_time_ms(LARGE_INTEGER start, LARGE_INTEGER end,
                          LARGE_INTEGER freq) {
  return ((double)(end.QuadPart - start.QuadPart) * 1000.0) /
         (double)freq.QuadPart;
}

// 1. User-input sanity check
void test_example() {
  int n;
  printf("===========================================\n");
  printf("       Sanity Check: User Input            \n");
  printf("===========================================\n");

  printf("Enter n: ");
  scanf("%d", &n);

  if (n <= 0) {
    printf("Invalid n. Skipping sanity check.\n\n");
    return;
  }

  double *X1 = (double *)malloc(n * sizeof(double));
  double *X2 = (double *)malloc(n * sizeof(double));
  double *Y1 = (double *)malloc(n * sizeof(double));
  double *Y2 = (double *)malloc(n * sizeof(double));
  double *Z_c = (double *)malloc(n * sizeof(double));
  double *Z_asm = (double *)malloc(n * sizeof(double));

  if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
    printf("Memory allocation failed!\n");
    free(X1);
    free(X2);
    free(Y1);
    free(Y2);
    free(Z_c);
    free(Z_asm);
    return;
  }

  printf("Enter X1 (%d values): ", n);
  for (int i = 0; i < n; i++)
    scanf("%lf", &X1[i]);

  printf("Enter X2 (%d values): ", n);
  for (int i = 0; i < n; i++)
    scanf("%lf", &X2[i]);

  printf("Enter Y1 (%d values): ", n);
  for (int i = 0; i < n; i++)
    scanf("%lf", &Y1[i]);

  printf("Enter Y2 (%d values): ", n);
  for (int i = 0; i < n; i++)
    scanf("%lf", &Y2[i]);

  distance_c(n, X1, X2, Y1, Y2, Z_c);
  distance_asm(n, X1, X2, Y1, Y2, Z_asm);

  printf("\ni\tC Kernel\tASM Kernel\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t%.5f\t\t%.5f\n", i, Z_c[i], Z_asm[i]);
  }

  // Check if C and ASM outputs match
  int match = 1;
  for (int i = 0; i < n; i++) {
    if (fabs(Z_c[i] - Z_asm[i]) > EPSILON) {
      match = 0;
      break;
    }
  }
  printf("C vs ASM Match: %s\n\n", match ? "PASS" : "FAIL");

  free(X1);
  free(X2);
  free(Y1);
  free(Y2);
  free(Z_c);
  free(Z_asm);
}

// Fills vector with random double values
void fill_random(double *vec, int n) {
  for (int i = 0; i < n; i++) {
    vec[i] = ((double)rand() / RAND_MAX) * 200.0 - 100.0;
  }
}

// 2. Correctness check on larger random dataset
void check_correctness(int n) {
  double *X1 = (double *)malloc(n * sizeof(double));
  double *X2 = (double *)malloc(n * sizeof(double));
  double *Y1 = (double *)malloc(n * sizeof(double));
  double *Y2 = (double *)malloc(n * sizeof(double));
  double *Z_c = (double *)malloc(n * sizeof(double));
  double *Z_asm = (double *)malloc(n * sizeof(double));

  if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
    printf("Memory allocation failed!\n");
    return;
  }

  fill_random(X1, n);
  fill_random(X2, n);
  fill_random(Y1, n);
  fill_random(Y2, n);

  distance_c(n, X1, X2, Y1, Y2, Z_c);
  distance_asm(n, X1, X2, Y1, Y2, Z_asm);

  int correct = 1;
  for (int i = 0; i < n; i++) {
    if (fabs(Z_c[i] - Z_asm[i]) > EPSILON) {
      correct = 0;
      break;
    }
  }

  printf("===========================================\n");
  printf("  Correctness Check: C vs ASM (n = %d)  \n", n);
  printf("===========================================\n");
  printf("Output Match: %s\n\n", correct ? "PASS" : "FAIL");

  free(X1);
  free(X2);
  free(Y1);
  free(Y2);
  free(Z_c);
  free(Z_asm);
}

// 3. Performance benchmark
void run_benchmark(int exponent, LARGE_INTEGER freq) {
  int n = 1 << exponent;
  printf("===========================================\n");
  printf(" Performance Benchmark: n = 2^%d (%d)\n", exponent, n);
  printf("===========================================\n");

  double *X1 = (double *)malloc(n * sizeof(double));
  double *X2 = (double *)malloc(n * sizeof(double));
  double *Y1 = (double *)malloc(n * sizeof(double));
  double *Y2 = (double *)malloc(n * sizeof(double));
  double *Z_c = (double *)malloc(n * sizeof(double));
  double *Z_asm = (double *)malloc(n * sizeof(double));

  if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
    printf("Memory allocation failed for n = 2^%d! Skipping...\n\n", exponent);
    free(X1);
    free(X2);
    free(Y1);
    free(Y2);
    free(Z_c);
    free(Z_asm);
    return;
  }

  fill_random(X1, n);
  fill_random(X2, n);
  fill_random(Y1, n);
  fill_random(Y2, n);

  LARGE_INTEGER start, end;
  double total_time_c = 0.0;
  double total_time_asm = 0.0;

  // Benchmark C Kernel
  for (int r = 0; r < NUM_RUNS; r++) {
    QueryPerformanceCounter(&start);
    distance_c(n, X1, X2, Y1, Y2, Z_c);
    QueryPerformanceCounter(&end);
    total_time_c += get_time_ms(start, end, freq);
  }

  // Benchmark ASM Kernel
  for (int r = 0; r < NUM_RUNS; r++) {
    QueryPerformanceCounter(&start);
    distance_asm(n, X1, X2, Y1, Y2, Z_asm);
    QueryPerformanceCounter(&end);
    total_time_asm += get_time_ms(start, end, freq);
  }

  double avg_c = total_time_c / NUM_RUNS;
  double avg_asm = total_time_asm / NUM_RUNS;

  printf("Average C Kernel Execution Time:   %.4f ms\n", avg_c);
  printf("Average ASM Kernel Execution Time: %.4f ms\n", avg_asm);
  printf("Speedup (C / ASM): %.2fx\n\n", avg_c / avg_asm);

  // Display first 10 elements of vector Z
  printf("First 10 elements of vector Z:\n");
  printf("i\tZ (C)\t\tZ (ASM)\n");
  for (int i = 0; i < 10 && i < n; i++) {
    printf("%d\t%.8f\t%.8f\n", i, Z_c[i], Z_asm[i]);
  }
  printf("\n");

  free(X1);
  free(X2);
  free(Y1);
  free(Y2);
  free(Z_c);
  free(Z_asm);
}

int main() {
  srand((unsigned int)time(NULL));

  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);

  // Run worked example sanity check
  test_example();

  // Run correctness check
  check_correctness(65536);

  // Run timing benchmark for n = 2^20, 2^24, 2^28
  int exponents[] = {20, 24, 28};
  int num_tests = sizeof(exponents) / sizeof(exponents[0]);

  for (int i = 0; i < num_tests; i++) {
    run_benchmark(exponents[i], freq);
  }

  return 0;
}
