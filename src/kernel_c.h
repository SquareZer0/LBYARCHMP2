#ifndef KERNEL_C_H
#define KERNEL_C_H

/* Computes Z[i] = sqrt((X2[i]-X1[i])^2 + (Y2[i]-Y1[i])^2) for i = 0..n-1 */
void distance_c(int n,
                const double* X1, const double* X2,
                const double* Y1, const double* Y2,
                double* Z);

#endif

