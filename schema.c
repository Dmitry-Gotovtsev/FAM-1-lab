#include <stdio.h>
#include "schema.h"
#include <stdlib.h>
#include <string.h>
#include "ccp.h"

int run_schema(FILE *file){

    struct Table curr_table = {0};

    int res = parse_schema(file, &curr_table);

    if (res == 1){
        return 1;
    }

    printf("rows %d\n", curr_table.rows);
    printf("columns %d\n", curr_table.column_count);
    for (int i = 0; i < curr_table.column_count; ++i)
    {
        printf("%d \"%s\" ", i, curr_table.columns[i].name);
        if (curr_table.columns[i].type == TYPE_INT)
        {
            printf("INT\n");
        }
        else
        {
            printf("DOUBLE\n");
        }
    }

    free_table(&curr_table);
    return 0;
}

int parse_schema(FILE *file, struct Table *table)
{

    struct Table curr_table = *table;

    // Чтение заголовка файла посимвольно

    int heading_size = 0, heading_capacity = 1;
    char *heading = malloc(heading_capacity * sizeof(char));
    if (heading == NULL)
    {
        free(heading);
        error(0);
        return 1;
    }

    int t_char;
    while ((t_char = fgetc(file)) != EOF && t_char != '\n')
    {
        if (heading_size == heading_capacity - 1)
        {
            heading_capacity *= 2;
            char *temp = realloc(heading, heading_capacity * sizeof(char));
            if (temp == NULL)
            {
                free(heading);
                error(0);
                return 1;
            }
            heading = temp;
        }

        heading[heading_size] = t_char;
        heading_size += 1;
    }

    if (heading_size == 0)
    {
        free(heading);
        error(1);
        return 1;
    }
    heading[heading_size] = '\0';

    // Подсчёт кол-ва полей

    if (heading[0] != '\0')
    {
        curr_table.column_count += 1;
    }

    for (int i = 0; i < heading_size; ++i)
    {
        if (heading[i] == ',')
        {
            curr_table.column_count += 1;
        }
    }

    // Разделение на поля и сохранение в Struct Table

    curr_table.columns = malloc(curr_table.column_count * sizeof(struct Column));

    if (curr_table.columns == NULL)
    {
        free(heading);
        free(curr_table.columns);
        error(0);
        return 1;
    }

    int col_ind = 0, field_size = 0, field_capacity = 1;
    char *curr_field = malloc(field_capacity * sizeof(char));
    if (curr_field == NULL)
    {
        free(heading);
        free(curr_field);
        free(curr_table.columns);
        error(0);
        return 1;
    }

    if (curr_table.column_count > 0)
    {
        for (int i = 0; i < heading_size; ++i)
        {
            if (heading[i] == ',')
            {
                curr_field[field_size] = '\0';
                curr_table.columns[col_ind].name = strdup(curr_field);
                curr_table.columns[col_ind].index = col_ind;
                curr_table.columns[col_ind].type = TYPE_INT;
                col_ind += 1;
                field_size = 0;
                continue;
            }

            if (field_capacity - 1 == field_size)
            {
                field_capacity *= 2;
                char *temp = realloc(curr_field, field_capacity * sizeof(char));
                if (temp == NULL)
                {
                    free(heading);
                    free(curr_field);
                    free(curr_table.columns);
                    error(0);
                    return 1;
                }
                curr_field = temp;
            }

            curr_field[field_size] = heading[i];
            field_size += 1;
        }
    }
    curr_field[field_size] = '\0';
    curr_table.columns[col_ind].name = strdup(curr_field);
    curr_table.columns[col_ind].index = col_ind;
    curr_table.columns[col_ind].type = TYPE_INT;

    free(curr_field);
    free(heading);

    // Проверка корректности заголовка

    for (int i = 0; i < curr_table.column_count; ++i)
    {
        if (curr_table.columns[i].name[0] == '\0')
        {
            error(1);
            free_table(&curr_table);
            return 1;
        }
        for (int e = i + 1; e < curr_table.column_count; ++e)
        {
            if (strcmp(curr_table.columns[i].name, curr_table.columns[e].name) == 0)
            {
                error(1);
                free_table(&curr_table);
                return 1;
            }
        }
    }

    int row_capacity = 1, row_size = 0;
    char *curr_row = malloc(row_capacity * sizeof(char));
    if (curr_row == NULL)
    {
        error(0);
        free_table(&curr_table);
        free(curr_row);
        return 1;
    }

    // чтение строк
    int c_char;
    while ((c_char = (fgetc(file))) != EOF)
    {
        row_size = 0;
        while (c_char != '\n' && c_char != EOF)
        {
            if (row_size == row_capacity - 1)
            {
                row_capacity *= 2;
                char *temp = realloc(curr_row, row_capacity * sizeof(char));
                if (temp == NULL)
                {
                    free(curr_row);
                    free_table(&curr_table);
                    error(0);
                    return 1;
                }
                curr_row = temp;
            }
            curr_row[row_size] = c_char;
            row_size += 1;
            c_char = fgetc(file);
        }
        if (row_size == 0)
        {
            continue;
        }
        else
        {
            curr_row[row_size] = '\0';
            if (row_size > 0 && curr_row[row_size - 1] == '\r')
            {
                curr_row[--row_size] = '\0';
            }
            curr_table.rows += 1;
        }

        // определение типа столбцов по строкам
        int f_cap = row_size, f_size = 0, col_ind = 0;
        char *cur_f = malloc((f_cap + 1) * sizeof(char));
        if (cur_f == NULL){
            free(cur_f);
            free(curr_row);
            free_table(&curr_table);
            error(0);
            return 1;
        }

        for (int i = 0; i < row_size; ++i)
        {
            if (curr_row[i] == ',')
            {
                cur_f[f_size] = '\0';
                enum ColumnType t = detect_type(cur_f);
                if (t == TYPE_DOUBLE)
                {
                    curr_table.columns[col_ind].type = TYPE_DOUBLE;
                }
                f_size = 0;
                col_ind += 1;
            }
            else
            {
                cur_f[f_size++] = curr_row[i];
            }
        }
        cur_f[f_size] = '\0';
        enum ColumnType t = detect_type(cur_f);
        if (t == TYPE_DOUBLE)
        {
            curr_table.columns[col_ind].type = TYPE_DOUBLE;
        }
        free(cur_f);
    }

    *table = curr_table;
    free(curr_row);
    return 0;
}

// определение типа поля
enum ColumnType detect_type(const char *field)
{
    int field_len = 0;
    while (field[field_len] != '\0')
    {
        if (field[field_len] == '.')
        {
            return TYPE_DOUBLE;
        }
        ++field_len;
    }
    return TYPE_INT;
}
// Освобождение curr_table

void free_table(struct Table *table)
{
    for (int i = 0; i < table->column_count; ++i)
    {
        free(table->columns[i].name);
    }
    free(table->columns);
    table->columns = NULL;
    table->column_count = 0;
}