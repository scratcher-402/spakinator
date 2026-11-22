#include "tree.h"
#include "database.h"

void handle_error(DBError err) {
    if (err) {
        printf("Error in tree, code %d\n", err);
        exit(1);
    }
}

const char* db_file_name = "db.txt";

int main() {
    DBError err = DB_OK;

    Database db = {0};
    err = db_load(db_file_name, &db);
    handle_error(err);

    err = db_save(&db);
    handle_error(err);

    db_close(&db);

    return 0;
}
