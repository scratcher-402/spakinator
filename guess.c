#include <assert.h>
#include <stdio.h>

#include "guess.h"
#include "database.h"
#include "lib/sptree/sptree.h"
#include "spdebug.h"
#include "tree.h"

static inline void print_tag(size_t depth) {
    for (size_t _ = 0; _ < depth + 1; _++) putchar('>');
    putchar(' ');
}

int guess_handler(NodeItem* item, TreeWalkDirection* direction, TreeNodeType type, size_t depth) {
    if (item == NULL) {
        $err("[guess_handler] Got empty item. Tree structure may be invalid");
        return GUESS_TREE_STRUCT_ERROR;
    }

    print_tag(depth);

    if (type == TNT_LEAF) {
        printf("It is %s.\n", item->text);
        *direction = TWD_QUIT;
        return 0;
    } else {
        printf("Is it %s? [Y/n] ", item->text);

        char choice = 0;
        int count = scanf(" %c", &choice);
        if (count != 1) {
            $err("[guess_handler] Could not read user choice");
            return GUESS_IO_ERROR;
        }

        switch (choice) {
            case 'n':
            case 'N':
                if (type & TNT_LEFT) {
                    *direction = TWD_LEFT;
                    return 0;
                } else {
                    printf("There is no elements matching these criteria.\n");
                    *direction = TWD_QUIT;
                    return 0;
                }

            case 'y':
            case 'Y':
            default:
                if (type & TNT_RIGHT) {
                    *direction = TWD_RIGHT;
                    return 0;
                } else {
                    printf("There is no elements matching these criteria.\n");
                    *direction = TWD_QUIT;
                    return 0;
                }
        }
    }
}

GuessError guess_start(Database* db) {
    assert(db != NULL);

    DBError err = DB_OK;

    TreeNode* root;
    err = db_get_root(db, &root);
    if (err) {
        $err("[guess_start] Could not get root node");
        return GUESS_DB_ERROR;
    }

    return (GuessError)tree_custom_walk(root, guess_handler);
}
