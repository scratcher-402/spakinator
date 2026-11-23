#include "database.h"

typedef enum {
    GUESS_OK,
    GUESS_DB_ERROR,
    GUESS_TREE_ERROR,
    GUESS_TREE_STRUCT_ERROR,
    GUESS_IO_ERROR,
    GUESS_UPDATE_DB,
} GuessError;

GuessError guess_start(Database* db);
