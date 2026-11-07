#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include "schema.h"
#include <math.h>
#include <stdlib.h>

struct ColumnStats
{
    long long min_int;
    long long max_int;

    double min_dub;
    double max_dub;

    double sum;
    int count;
    double mean;
    int positive;
    int negative;

    int even;
    int integer;
};

double fix_zero(double v);

int run_stats(FILE *file);

#endif