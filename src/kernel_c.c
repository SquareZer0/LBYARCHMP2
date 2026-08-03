#include "kernel_c.h"
#include <emmintrin.h>  /* SSE2 intrinsics: __m128d, _mm_*_sd */

void distance_c(int n,
                 const double* X1, const double* X2,
                 const double* Y1, const double* Y2,
                 double* Z)
{
    int i;
    for (i = 0; i < n; i++) {
        /* load scalar doubles into the low lane of an XMM register */
        __m128d x1 = _mm_load_sd(&X1[i]);
        __m128d x2 = _mm_load_sd(&X2[i]);
        __m128d y1 = _mm_load_sd(&Y1[i]);
        __m128d y2 = _mm_load_sd(&Y2[i]);

        __m128d dx = _mm_sub_sd(x2, x1);      /* dx = X2[i] - X1[i]   */
        __m128d dy = _mm_sub_sd(y2, y1);      /* dy = Y2[i] - Y1[i]   */

        __m128d dx2 = _mm_mul_sd(dx, dx);     /* dx*dx                */
        __m128d dy2 = _mm_mul_sd(dy, dy);     /* dy*dy                */

        __m128d sum = _mm_add_sd(dx2, dy2);   /* dx*dx + dy*dy        */
        __m128d res = _mm_sqrt_sd(sum, sum);  /* sqrt(...)            */

        _mm_store_sd(&Z[i], res);
    }
}
