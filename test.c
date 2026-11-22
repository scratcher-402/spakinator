#include "tree.h"

const char* my_tree = "\"Test root node\" (\"Left \\\"child\\\"\" (nil, nil), \"Right child\" (nil, nil))";

void handle_error(TreeError err) {
    if (err) {
        printf("Error in tree, code %d\n", err);
        exit(1);
    }
}

int main() {
    TreeError err = TREE_OK;

    TreeNode* root = NULL;
    err = tree_parse(&root, my_tree);
    handle_error(err);

    $info("Tree parsed");
    tree_dump(root, stdout);

    err = tree_dot_dump(root);
    handle_error(err);

    tree_free(&root);
    return 0;
}
