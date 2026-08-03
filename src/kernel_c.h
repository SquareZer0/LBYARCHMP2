#ifndef KERNEL_C_H
#define KERNEL_C_H

/*
 * distance_c
 * ----------
 * Computes Z[i] = sqrt( (X2[i]-X1[i])^2 + (Y2[i]-Y1[i])^2 ) for i = 0..n-1.
 *
 * Uses SSE2 scalar double-precision intrinsics explicitly (_mm_..._sd)
 * so the "functional scalar SIMD registers / instructions" requirement
 * is satisfied in an auditable way, rather than just relying on the
 * compiler's default x64 codegen (which also happens to use scalar
 * SSE2 for doubles, but that's implicit).
 */
void distance_c(int n,
                 const double* X1, const double* X2,
                 const double* Y1, const double* Y2,
                 double* Z);

#endif /* KERNEL_C_H */
