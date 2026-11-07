#include <stdio.h>
#include <string.h>
#include "ccp.h"
#include "schema.h"
#include "stats.h"

int main(int argc, char *argv[]){
    int command;
    
    // Обработка некорректных команд
    if (parse_args(argc, argv, &command) == 0){
        error(3);
        return 2;
    }
    

    if (command == 0){
        printf("csvstat schema <csv-file> - outputs data about the table columns\n csvstat stats <csv-file> - outputs general data about the table and column statistics\n csvstat report <csv-file> <output-file> - analyzes the table and generates a report in the specified output file");
    }
    
    // команда schema
    else if (command == 1){
        FILE *table = fopen(argv[2], "r");
        if (table == NULL){
            error(2);
            return 1;
        }
    
        int result = run_schema(table);
        fclose(table);
        return result;
    }

    // команда stats
    else if (command == 2){
        FILE *table = fopen(argv[2], "r");
        if (table == NULL){
            fprintf(stderr, "Error: file open error\n");
            return 1;
        }

        int result = run_stats(table);
        fclose(table);
        return result;
    }

    return 0;
}

// Функция обработки команд
int parse_args(int argc, char *argv[], int *command){  
    if (argc == 1){
        return 0;
    }
    if (strcmp(argv[1], "--help")==0){
        *command = 0;
        return (argc == 2);
    }
    else if (strcmp(argv[1], "schema")==0){
        *command = 1;
        return (argc == 3);
    }
    else if (strcmp(argv[1], "stats")==0){
        *command = 2;
        return (argc == 3);
    }
    else{
        return 0;
    }
}

// Функция вывода ошибки по коду
void error(int code){
    if (code == 0){
        fprintf(stderr, "Error: memory allocation error\n");
    }
    else if (code == 1){
        fprintf(stderr, "Error: reading table header error\n");
    }
    else if (code == 2){
        fprintf(stderr, "Error: file open error\n");
    }
    else if (code == 3){
        fprintf(stderr, "Error: unsupported command\n Try --help");
    }
}