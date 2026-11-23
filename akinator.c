#include "database.h"
#include "guess.h"
#include "spdebug.h"

int main(int argc, const char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s path/to/db.txt\n", (argc == 1) ? argv[0] : "akinator");
        return 1;
    }

    const char* db_path = argv[1];
    Database db = {0};
    DBError db_err = db_load(db_path, &db);
    if (db_err) {
        $fatal("Could not load database '%s'", db_path);
        return 1;
    }

    tree_dot_dump(db.root);

    int ret = 0;
    GuessError guess_err = guess_start(&db);
    if (guess_err) {
        $fatal("Error while guessing");
        ret = 1;
        goto cleanup;
    }

    db_save(&db);

cleanup:
    db_close(&db);
    return ret;
}
