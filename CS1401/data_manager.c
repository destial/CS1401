/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 28 October 2025
@course CSD1401
@brief Implementation file for loading and saving text files
*/

#include "data_manager.h"
#include "utils.h"

// sourced from: https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
char* read_content(const char* filename) {
    int fsize = 0;
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        return NULL;
    }

    char* fcontent = NULL;
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    fcontent = calloc(fsize, sizeof(char));
    if (fcontent) {
        fread(fcontent, 1, fsize, fp);
    }
    fclose(fp);
    fp = NULL;
    return fcontent;
}

void write_content(const char* filename, char* content) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        return;
    }
    fputs(content, fp);
    fclose(fp);
    fp = NULL;
}

Map* load_map(const char* filename) {
    char* contents = read_content(filename);
    if (contents == NULL || strlen(contents) == 0)
        return NULL;

    const char outerdel[] = "\n";
    const char innerdel[] = "=";

    // allocate the root of this level map
    Map* root = malloc(sizeof(Map));
    if (root == NULL)
        return NULL;

    Map* next = root;
    char* outerptr = NULL;
    char* innerptr = NULL;

    // set initial variable pointers for string tokenization (strtok)
    char* k, * v;
    char* line = strtok_s(contents, outerdel, &outerptr);
    while (line) {
        // set the key (left side)
        k = strtok_s(line, innerdel, &innerptr);

        // +1 because last character is '\0' to terminate the string
        size_t key_len = strlen(k) + 1;
        next->key = calloc(key_len, sizeof(char));
        strcpy_s(next->key, key_len, k);

        // set the value (right side)
        v = strtok_s(NULL, innerdel, &innerptr);
        size_t val_len = strlen(v) + 1;
        next->value = calloc(val_len, sizeof(char));
        strcpy_s(next->value, val_len, v);

        // end of check, go to next line or quit if already at the end
        line = strtok_s(NULL, outerdel, &outerptr);
        if (line) {
            next->next = malloc(sizeof(Map));
            if (next->next) {
                next = next->next;
            }
        }
        else {
            next->next = NULL;
        }
    }
    next = NULL;

    // make sure to free up allocated memory for the text file contents
    free(contents);
    contents = NULL;
    return root;
}

// create a node of this map, or if NULL, a new node
static Map* add_node(Map* map) {
    Map* next;
    if (map == NULL) {
        next = malloc(sizeof(Map));
    }
    else {
        if (map->key == NULL) {
            next = map;
        }
        else {
            Map* last = map;
            while (last->next) {
                last = last->next;
            }
            last->next = malloc(sizeof(Map));
            next = last->next;
        }
    }
    return next;
}

Map* add_entry(Map* map, const char* key, char* value) {
    Map* next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    size_t val_len = strlen(value) + 1;
    next->value = calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, value);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map* add_entry_int(Map* map, const char* key, int value) {
    Map* next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[64];
    sprintf_s(v, 64, "%d", value);
    size_t val_len = strlen(v) + 1;
    next->value = calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map* add_entry_float(Map* map, const char* key, float value) {
    Map* next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[64];
    sprintf_s(v, 64, "%f", value);
    size_t val_len = strlen(v) + 1;
    next->value = calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map* add_entry_vector(Map* map, const char* key, CP_Vector value) {
    Map* next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[128];
    sprintf_s(v, 128, "%f,%f", value.x, value.y);
    size_t val_len = strlen(v) + 1;
    next->value = calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

void edit_entry(Map* map, const char* key, char* value) {
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            free(ptr->value);
            size_t val_len = strlen(value) + 1;
            ptr->value = calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, value);
            return;
        }
    }
    add_entry(map, key, value);
}

void edit_entry_int(Map* map, const char* key, int value) {
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            free(ptr->value);
            char v[64];
            sprintf_s(v, 64, "%d", value);
            size_t val_len = strlen(v) + 1;
            ptr->value = calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return;
        }
    }
    add_entry_int(map, key, value);
}

void edit_entry_float(Map* map, const char* key, float value) {
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            free(ptr->value);
            char v[64];
            sprintf_s(v, 64, "%f", value);
            size_t val_len = strlen(v) + 1;
            ptr->value = calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return;
        }
    }
    add_entry_float(map, key, value);
}

void edit_entry_vector(Map* map, const char* key, CP_Vector value) {
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            free(ptr->value);
            char v[128];
            sprintf_s(v, 128, "%f,%f", value.x, value.y);
            size_t val_len = strlen(v) + 1;
            ptr->value = calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return;
        }
    }
    add_entry_vector(map, key, value);
}

Map* remove_entry(Map* map, const char* key) {
    // first one in the map
    if (strcmp(map->key, key) == 0) {
        Map* next = map->next;
        FREE_MEMORY(map->key);
        FREE_MEMORY(map->value);
        FREE_MEMORY(map);
        return next;
    }
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        // inside the map
        if (ptr->next) {
            if (strcmp(ptr->next->key, key) == 0) {
                Map* todel = ptr->next;
                Map* after = todel->next;
                ptr->next = after;

                FREE_MEMORY(todel->key);
                FREE_MEMORY(todel->value);
                FREE_MEMORY(todel);
                break;
            }
        }
    }
    return map;
}

CP_BOOL save_map(const char* filename, Map* map) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        return FALSE;
    }

    Map* next = map;
    while (next) {
        fputs(next->key, fp);
        fputc('=', fp);
        fputs(next->value, fp);
        fputc('\n', fp);
        next = next->next;
    }

    fclose(fp);
    fp = NULL;
    return TRUE;
}

char* get_value(Map* map, const char* key) {
    // loop through each map node and compare the key value with the given key
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            return ptr->value;
        }
    }
    return NULL;
}

int get_int(Map* map, const char* key, int def) {
    char* value = get_value(map, key);
    return value ? atoi(value) : def;
}

float get_float(Map* map, const char* key, float def) {
    char* value = get_value(map, key);
    return value ? (float)atof(value) : def;
}

CP_Vector get_vector(Map* map, const char* key) {
    char* value = get_value(map, key);
    return value ? parse_vector(value) : CP_Vector_Set(0, 0);
}

// tokenize the given string from (131231,14512) to a proper vector
CP_Vector parse_vector(char* line) {
    const char innerdel[] = ",";
    char* innerptr = NULL;

    // get the x value (left side)
    char* xstr = strtok_s(line, innerdel, &innerptr);
    float x = (float)atof(xstr);

    // get the y value (right side)
    char* ystr = strtok_s(NULL, innerdel, &innerptr);
    float y = (float)atof(ystr);
    CP_Vector vector = CP_Vector_Set(x, y);

    // clean up
    xstr = NULL;
    ystr = NULL;
    return vector;
}

// recursively loops through the map to free up the allocations, starting from the last node
void free_map(Map* map) {
    if (map == NULL)
        return;

    if (map->next) {
        free_map(map->next);
        map->next = NULL;
    }
    if (map->key) {
        FREE_MEMORY(map->key);
    }
    if (map->value) {
        FREE_MEMORY(map->value);
    }
    FREE_MEMORY(map);
}
