#include "database.h"
#include "guess.h"

void handle_error(DBError err) {
    if (err) {
        printf("Error in database, code %d\n", err);
        exit(1);
    }
}

const char* db_file_name = "db.txt";

int main() {
    DBError err = DB_OK;

    Database db = {0};
    err = db_load(db_file_name, &db);
    handle_error(err);

    TreeNode* root = NULL;
    err = db_get_root(&db, &root);
    handle_error(err);

    tree_dot_dump(root);

    GuessError guess_err = guess_start(&db);
    $d(guess_err);

    db_close(&db);

    return 0;
}
