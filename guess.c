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

static int create_new_item_from_input(TreeNode** node) {
    char* text = NULL;
    size_t n = 0;
    ssize_t result = getline(&text, &n, stdin);
    if (result == -1) {
        $errno;
        $err("[add_item] Error while reading user input");
        free(text);
        return GUESS_IO_ERROR;
    }

    char* pos = strchr(text, '\n');
    if (pos != NULL) *pos = '\0';

    NodeItem item = {
        .text = text,
    };

    TreeError tree_err = tree_new_node(node, &item);
    if (tree_err) {
        $err("[add_item] Could not create new node");
        return GUESS_TREE_ERROR;
    }

    return 0;
}

static int nonexistent_item(TreeNode* node, TreeAttachMode mode) {
    printf("There is no elements matching these criteria. Would you like to add one? [y/N] ");

    char choice = 0;
    int count = scanf(" %c", &choice);
    if (count != 1) {
        $errno;
        $err("[nonexistent_item] Error while reading user input");
        return GUESS_IO_ERROR;
    }

    if (choice != '\n') getchar();

    int err = 0;
    TreeNode* new_node = NULL;
    TreeError tree_err = TREE_OK;

    switch (choice) {
        case 'y':
        case 'Y':
            printf("What is it? > ");
            err = create_new_item_from_input(&new_node);
            if (err) return err;

            tree_err = tree_attach(node, new_node, mode);
            if (tree_err) {
                $err("[nonexistent_item] Error while attaching new node");
                tree_free(&new_node);
                return GUESS_TREE_ERROR;
            }

            return GUESS_UPDATE_DB;

        case 'n':
        case 'N':
        default:
            return 0;
    }
}

static int handle_leaf(TreeNode* node) {
    printf("Is it %s? [Y/n] ", node->el.text);

    char choice = 0;
    int count = scanf(" %c", &choice);
    if (count != 1) {
        $errno;
        $err("[handle_leaf] Error while reading user input");
        return GUESS_IO_ERROR;
    }

    if (choice != '\n') getchar();

    if (choice == 'n' || choice == 'N') {
        TreeNode* old_variant = NULL;
        TreeNode* new_variant = NULL;

        printf("What is it? > ");
        int err = create_new_item_from_input(&new_variant);
        if (err) return err;

        printf("What makes %s different from %s? > ", new_variant->el.text, node->el.text);
        err = create_new_item_from_input(&old_variant);
        if (err) return err;

        char* question = old_variant->el.text;
        old_variant->el.text = node->el.text;
        node->el.text = question;

        printf("Is this statement true for %s? (otherwise it is true for %s) [Y/n] ", new_variant->el.text, old_variant->el.text);
        choice = 0;
        int count = scanf(" %c", &choice);
        if (count != 1) {
            $errno;
            $err("[handle_leaf] Error while reading user input");
            return GUESS_IO_ERROR;
        }

        if (choice != '\n') getchar();

        switch (choice) {
            case 'n':
            case 'N':
                tree_attach(node, new_variant, TREE_ATTACH_LEFT);
                tree_attach(node, old_variant, TREE_ATTACH_RIGHT);

            case 'y':
            case 'Y':
            default:
                tree_attach(node, new_variant, TREE_ATTACH_RIGHT);
                tree_attach(node, old_variant, TREE_ATTACH_LEFT);
        }

        return GUESS_UPDATE_DB;
    }

    return GUESS_OK;
}

int guess_handler(TreeNode* node, TreeWalkDirection* direction, TreeNodeType type, size_t depth) {
    if (node == NULL) {
        $err("[guess_handler] Got empty item. Tree structure may be invalid");
        return GUESS_TREE_STRUCT_ERROR;
    }

    print_tag(depth);

    if (type == TNT_LEAF) {
        int err = handle_leaf(node);
        *direction = TWD_QUIT;
        return err;
    } else {
        printf("%s [Y/n] ", node->el.text);

        char choice = 0;
        int count = scanf(" %c", &choice);
        if (count != 1) {
            $err("[guess_handler] Could not read user choice");
            return GUESS_IO_ERROR;
        }

        if (choice != '\n') getchar();
        int err = 0;

        switch (choice) {
            case 'n':
            case 'N':
                if (type & TNT_LEFT) {
                    *direction = TWD_LEFT;
                    return 0;
                } else {
                    err = nonexistent_item(node, TREE_ATTACH_LEFT);
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
                    err = nonexistent_item(node, TREE_ATTACH_RIGHT);
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
