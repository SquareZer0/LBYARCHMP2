#include "kernel_c.h"
#include <math.h>

void distance_c(int n,
                const double* X1, const double* X2,
                const double* Y1, const double* Y2,
                double* Z)
{
    for (int i = 0; i < n; i++) {
        double dx = X2[i] - X1[i];
        double dy = Y2[i] - Y1[i];
        Z[i] = sqrt(dx * dx + dy * dy);
    }
}

