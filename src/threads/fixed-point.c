#include "threads/fixed-point.h"
#include <stdint.h>

int 
int_to_fp (int integer)
{
    return integer * (FRACTION);
}

int 
fp_to_int_round_zero (int fx_number) 
{
    return fx_number/(FRACTION);
}

int 
fp_to_int_round_nearest (int fx_number) 
{
    return fx_number >= 0 ? 
        (fx_number + (FRACTION) / 2) / (FRACTION)
        : (fx_number - (FRACTION)/2) / (FRACTION);
}

int 
add_fp_fp (int x, int y) 
{
    return x + y;
}

int 
add_fp_int (int x, int n) 
{
    return x + n * (FRACTION);
}

int 
sub_fp_from_fp (int y, int x) 
{
    return x - y;
}

int 
sub_int_from_fp (int n, int x) 
{
    return x - n* (FRACTION);
}

int
mult_fp_fp (int x, int y)
{
    return ((int64_t)x) * y / (FRACTION);
}

int
mult_int_fp (int n, int x)
{
    return x*n;
}

int
div_fp_by_fp (int x, int y)
{
    return ((int64_t)x) * (FRACTION) / y;
}

int
div_fp_by_int (int x, int n)
{
    return x/n;
}