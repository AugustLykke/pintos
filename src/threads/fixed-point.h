#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#define FRACTION (1<<14)

int int_to_fp (int integer);

int fp_to_int_round_zero (int fx_number);
int fp_to_int_round_nearest (int fx_number);

int add_fp_fp (int x, int y);
int add_fp_int (int x, int n);

int sub_fp_from_fp (int y, int x);
int sub_int_from_fp (int n, int x);

int mult_fp_fp (int x, int y);
int mult_int_fp (int n, int x);

int div_fp_by_fp (int x, int y);
int div_fp_by_int (int x, int n);

#endif