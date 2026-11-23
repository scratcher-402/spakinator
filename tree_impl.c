#define SPTREE_IMPLEMENTATION
#include "tree.h"
#include "spdebug.h"


void my_tree_element_destroy(NodeItem* item) {
    if (item->text != NULL) free(item->text);
}

const NodeDestructor_t tree_element_destroy = my_tree_element_destroy;

int my_tree_element_serialize(NodeItem* item, FILE* file) {
    assert(item != NULL);
    assert(file != NULL);

    const char* str = item->text;
    if (!str) return 1;

    fputc('"', file);
    while (*str != '\0') {
        if (*str == '"') fprintf(file, "\\\"");
        else if (*str == '\\') fprintf(file, "\\\\");
        else fputc(*str, file);
        str += 1;
    }
    fputc('"', file);

    return 0;
}

int my_tree_element_dot_serialize(NodeItem* item, FILE* file) {
    assert(item != NULL);
    assert(file != NULL);

    const char* str = item->text;
    if (!str) return 1;

    fputc('"', file);
    while (*str != '\0') {
        if (*str == '"') fputs("\\\"", file);
        else if (*str == '\\') fputs("\\\\", file);
        else if (*str == '<') fputs("&lt;", file);
        else if (*str == '>') fputs("&gt;", file);
        else fputc(*str, file);
        str += 1;
    }
    fputc('"', file);

    return 0;
}

const NodeSerializer_t tree_element_serialize = my_tree_element_serialize;
const NodeSerializer_t tree_element_dot_serialize = my_tree_element_dot_serialize;

static inline void my_skip_spaces(const char** strp) {
    while (isspace(**strp) && (**strp != '\0')) *strp += 1;
}

static int get_string_size(const char* str, size_t* size_ptr, size_t* full_size_ptr) {
    size_t size = 0;

    skip_spaces(&str);
    if (*str != '"') {
        $err("[get_string_size] Expected beginning of a string");
        return 1;
    }

    str += 1;
    const char* saved_str = str;

    while (*str != '\0' && *str != '"') {
        // $debug("%p %zu %c %zu", str, str, *str, size);
        size += 1;
        if (*str == '\\') {
            if (str[1] == '\0') {
                $err("Expected a character after \\");
                return 1;
            }
            else if (str[1] == '\\' || str[1] == '"') {
                size += 1;
                str += 1;
            }
            else {
                $err("[get_string_size] Unknown escape sequence: '\\%c'", str[1]);
                return 1;
            }
        }
        str += 1;
    }

    if (*str != '\"') {
        $err("[get_string_size] \" was never closed");
        return 1;
    }

    // $debug("size: %zu, str: %.*s", str - saved_str, str - saved_str, saved_str);

    *size_ptr = size;
    *full_size_ptr = str - saved_str;
    return 0;
}

int my_tree_element_deserialize(const char** strp, NodeItem* item) {
    // $debug("deserialize");
    assert(item != NULL);
    assert(strp != NULL);

    // get size
    size_t string_size = 0;
    size_t string_full_size = 0;
    int err = get_string_size(*strp, &string_size, &string_full_size);
    if (err) {
        $err("[my_tree_element_deserialize] Error while calculating string size");
        return err;
    }

    // $debug("size: %zu, full size: %zu", string_size, string_full_size);

    char* text = (char*)calloc(string_size + 1, sizeof(char));

    // $vs(*strp);
    skip_spaces(strp);
    // $vs(*strp);
    *strp += 1;

    size_t str_index = 0;
    for (size_t i = 0; i < string_full_size; i++) {
        // $debug("%c %zu %zu", (*strp)[i], i, str_index);
        if ((*strp)[i] == '\\') {
            text[str_index] = (*strp)[i+1];
            str_index += 1;
            i += 1;
        } else {
            text[str_index] = (*strp)[i];
            str_index += 1;
        }
    }

    item->text = text;
    *strp += string_full_size + 1;

    // $vs(*strp);

    return 0;
}

const NodeDeserializer_t tree_element_deserialize = my_tree_element_deserialize;


