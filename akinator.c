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

    int ret = 0;

    while (1) {
        char choice = 0;
        printf(
            "Select action: \n"
            " 1) Start guessing\n"
            " 2) Dump database to dot file\n"
            " 0) Quit\n"
            "> "
        );
        int count = scanf(" %c", &choice);
        if (count != 1) {
            $errno;
            $fatal("Input error");
            ret = 1;
            goto cleanup;
        }

        switch (choice) {
            case '1':
                GuessError guess_err = guess_start(&db);
                if (guess_err == GUESS_UPDATE_DB) {
                    db_err = db_save(&db);
                    if (db_err) {
                        $fatal("Error while saving database");
                        ret = 1;
                        goto cleanup;
                    }

                    $info("Database saved");
                } else if (guess_err) {
                    $fatal("Error while guessing");
                    ret = 1;
                    goto cleanup;
                }

                break;

            case '2':
                TreeError tree_err = tree_dot_dump(db.root);
                if (tree_err) {
                    $err("Could not dump tree to dot file");
                } else {
                    $info("Dot dump saved to tree_sump.dot");
                }

                break;

            case '0':
                goto cleanup;

            default:
                $err("Unknown choice: '%c'");
                break;
        }
    }

cleanup:
    db_close(&db);
    return ret;
}
