#include "stats.h"
#include "ccp.h"




int run_stats(FILE *file){
    struct Table curr_table = {0};

    int res = parse_schema(file, &curr_table);

    if (res == 1){
        return 1;
    }

    struct ColumnStats *stats = calloc(curr_table.column_count, sizeof(struct ColumnStats));
    if (stats == NULL){
        free(stats);
        free_table(&curr_table);
        error(0);
        return 1;
    }
    
    rewind(file);
    int c;
    while ((c = fgetc(file)) != '\n' && c != EOF){
    }

    int f_size = 0, f_cap = 1, ind_col = 0;
    char *cur_f = malloc(f_cap * sizeof(char));
    if (cur_f == NULL){
        free(cur_f);
        free(stats);
        free_table(&curr_table);
        error(0);
        return 1;
    }

    while (1){
        c = fgetc(file);

        if (f_size == f_cap - 1){
            f_cap *= 2;
            char *temp = realloc(cur_f, f_cap * sizeof(char));
            if (temp == NULL){
                free(cur_f);
                free(stats);
                free_table(&curr_table);
                error(0);
                return 1;
            }
            cur_f = temp;
        }

        if (c != '\n' && c != ',' && c != EOF){
            cur_f[f_size] = c;
        }
        else{
            if (f_size != 0){
            cur_f[f_size] = '\0';

            if (curr_table.columns[ind_col].type == TYPE_INT){
                long long x = atoll(cur_f);

                if (stats[ind_col].count == 0){
                    stats[ind_col].max_int = x;
                    stats[ind_col].min_int = x;
                }

                if (x < stats[ind_col].min_int){
                    stats[ind_col].min_int = x;
                }

                if (x > stats[ind_col].max_int){
                    stats[ind_col].max_int = x;
                }

                stats[ind_col].sum += x;
                stats[ind_col].count += 1;

                if (x > 0){
                    stats[ind_col].positive += 1;
                }
                else if (x < 0){
                    stats[ind_col].negative += 1;
                }

                if (x % 2 == 0){
                    stats[ind_col].even += 1;
                }
            }
            else {
                double x = atof(cur_f);

                if (stats[ind_col].count == 0){
                    stats[ind_col].max_dub = x;
                    stats[ind_col].min_dub = x;
                }

                if (x < stats[ind_col].min_dub){
                    stats[ind_col].min_dub = x;
                }

                if (x > stats[ind_col].max_dub){
                    stats[ind_col].max_dub = x;
                }

                stats[ind_col].sum += x;
                stats[ind_col].count += 1;

                if (x > 0){
                    stats[ind_col].positive += 1;
                }
                else if (x < 0){
                    stats[ind_col].negative += 1;
                }
                
                if (floor(x) == x){
                    stats[ind_col].integer += 1;
                }
            }

            f_size = -1;
            }
            if (c == ','){
                ind_col += 1;
            }
            else if (c == '\n'){
                ind_col = 0;
            }
            else if (c == EOF){
                break;
            }
        }
        f_size += 1;
    }

    printf("rows %d\n", curr_table.rows);
    printf("columns %d\n", curr_table.column_count);
    for (int i = 0; i < curr_table.column_count; ++i){
        printf("column %d\n", i);
        printf("name \"%s\"\n", curr_table.columns[i].name);

        if (curr_table.columns[i].type == TYPE_INT){
            printf("type INT\nnulls 0\n");
            printf("min %lld\nmax %lld\n",  stats[i].min_int, stats[i].max_int);
            printf("mean %.6f\n", fix_zero(stats[i].sum / stats[i].count));
            printf("positive %d\nnegative %d\n", stats[i].positive, stats[i].negative);
            printf("even %d\n", stats[i].even);
        }
        else{
            printf("type DOUBLE\nnulls 0\n");
            printf("min %.6f\nmax %.6f\n", fix_zero(stats[i].min_dub),  fix_zero(stats[i].max_dub));
            printf("mean %.6f\n", fix_zero(stats[i].sum / stats[i].count));
            printf("positive %d\nnegative %d\n", stats[i].positive, stats[i].negative);
            printf("integer %d\n", stats[i].integer);    
        }
        printf("\n");
    } 


    free(cur_f);
    free(stats);
    free_table(&curr_table);
    return 0;
}

// функция очистки вещественного нуля
double fix_zero(double v) {
    if (v == 0.0) {
        return 0.0;
    }
    return v;
}