#ifndef SCHEMA_H
#define SCHEMA_H

enum ColumnType {
    TYPE_INT,
    TYPE_DOUBLE,
};

struct Column
{
    int index;
    char *name;
    enum ColumnType type;
};

struct Table
{   
    int rows;
    int column_count;
    struct Column *columns;
};

int run_schema(FILE *table);

int parse_schema(FILE *file, struct Table *table);

void free_table(struct Table *table);

enum ColumnType detect_type(const char *field);

#endif 