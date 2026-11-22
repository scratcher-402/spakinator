#include <stdio.h>
#include <sys/stat.h>
#include "database.h"
#include "lib/sptree/sptree.h"
#include "spdebug.h"

static DBError get_file_size(FILE* file, size_t* size_ptr) {
    struct stat st;
    int err = fstat(fileno(file), &st);
    if (err) {
        $errno;
        $err("[get_file_size] Could not stat file (error code: %d)", err);
        return DB_FILE_STAT_ERROR;
    }

    *size_ptr = st.st_size;
    return DB_OK;
}

// TODO: correct cleanup
DBError db_load(const char* file_name, Database* db) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        $errno;
        $err("[db_load] Could not open file: %s", file_name);
        return DB_FILE_OPEN_ERROR;
    }

    size_t file_size = 0;
    DBError err = get_file_size(file, &file_size);
    if (err) {
        $err("[db_load] Could not get file size");
        return err;
    }

    char* file_data = (char*)calloc(file_size + 1, sizeof(char));
    if (!file_data) {
        $errno;
        $err("[db_load] Could not allocate memory for file data");
        return DB_ALLOC_ERROR;
    }

    size_t bytes_read = fread(file_data, sizeof(char), file_size, file);
    if (bytes_read != file_size) {
        $errno;
        $err("[db_load] Could not read data from file (could read %zu / %zu symbols)", bytes_read, file_size);
        return DB_READ_ERROR;
    }
    file_data[file_size] = '\0';

    TreeNode* root = NULL;
    TreeError tree_err = tree_parse(&root, file_data);
    if (tree_err) {
        $err("[db_load] Could not parse tree");
        return DB_PARSE_ERROR;
    }

    free(file_data);
    fclose(file);

    db->root = root;
    db->file_name = file_name;
    return DB_OK;
}

DBError db_get_root(Database* db, TreeNode** root) {
    *root = db->root;
    return DB_OK;
}

// TODO: cleanup
DBError db_save(Database* db) {
    FILE* file = fopen(db->file_name, "w");
    if (!file) {
        $errno;
        $err("[db_save] Could not open file: %s", db->file_name);
        return DB_FILE_OPEN_ERROR;
    }

    TreeError tree_err = tree_dump(db->root, file);
    if (tree_err) {
        $err("[db_save] Could not dump tree to a file");
        return DB_DUMP_ERROR;
    }

    fclose(file);
    return DB_OK;
}

void db_close(Database* db) {
    db->file_name = NULL;
    tree_free(&db->root);
}

