#pragma once
#include "tree.h"

typedef struct {
    TreeNode* root;
    const char* file_name;
} Database;

typedef enum {
    DB_OK = 0,
    DB_FILE_OPEN_ERROR,
    DB_FILE_STAT_ERROR,
    DB_ALLOC_ERROR,
    DB_READ_ERROR,
    DB_PARSE_ERROR,
    DB_DUMP_ERROR
} DBError;

DBError db_load(const char* file_name, Database* db);
DBError db_get_root(Database* db, TreeNode** root);
DBError db_save(Database* db);
void db_close(Database* db);
