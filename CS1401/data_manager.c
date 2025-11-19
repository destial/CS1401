/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 28 October 2025
@course CSD1401
@brief Implementation file for loading and saving text files
*/

#include "data_manager.h"

// create a node of this map, or if NULL, a new node
static Map add_node(Map map) {
    if (map == NULL) { // if the root map is NULL, we create a new root map
        return gc_malloc(sizeof(struct MapStruct));
    }

    if (map->key == NULL) { // if this node has no value stored, we use this instead
        return map;
    }

    Map last = map;
    while (last->next) { // go to the back of this map
        last = last->next;
    }
    last->next = gc_malloc(sizeof(struct MapStruct)); // create the new node in at the back
    return last->next;
}

// sourced from: https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
char* read_content(const char* filename) {
    FILE* fp; // safe open
    fopen_s(&fp, filename, "r");
    if (!fp) {
        return NULL;
    }

    // check size of file contents, make sure its empty
    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    if (fsize == 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    // read contents
    char* fcontent = gc_calloc(fsize, sizeof(char));
    if (fcontent) {
        fread(fcontent, 1, fsize, fp);
    }
    fclose(fp);
    fp = NULL;
    return fcontent;
}

void write_content(const char* filename, char* content) {
    FILE* fp; // safe open
    fopen_s(&fp, filename, "w");
    if (!fp) {
        return;
    }
    fputs(content, fp);
    fclose(fp);
    fp = NULL;
}

// split the given string from (131231,14512) into a proper vector
// sourced from https://stackoverflow.com/a/54450265
CP_Vector parse_vector(char* line) {
    CP_Vector v = CP_Vector_Set(0.f, 0.f);
    sscanf_s(line, "%f,%f", &v.x, &v.y);
    return v;
}

Map Map_Load(const char* filename) {
    char* contents = read_content(filename);
    if (contents == NULL || strlen(contents) == 0)
        return NULL;

    // define the delimiters for parsing
    const char outerdel[] = "\n"; // each line
    const char innerdel[] = "="; // each key value pair

    // allocate the root of this level map
    Map root = gc_malloc(sizeof(struct MapStruct));
    if (root == NULL)
        return NULL;

    Map next = root;
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
        next->key = gc_malloc(key_len * sizeof(char));
        strcpy_s(next->key, key_len, k);

        // set the value (right side)
        v = strtok_s(NULL, innerdel, &innerptr);
        size_t val_len = strlen(v) + 1;
        next->value = gc_malloc(val_len * sizeof(char));
        strcpy_s(next->value, val_len, v);

        // end of check, go to next line or quit if already at the end
        line = strtok_s(NULL, outerdel, &outerptr);
        if (line) {
            next->next = gc_malloc(sizeof(struct MapStruct));
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
    FREE_MEMORY(contents);

    return root;
}

Map Map_AddString(Map map, const char* key, char* value) {
    // if this key already exists, we skip
    if (Map_GetString(map, key))
        return map;

    Map next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = gc_calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    size_t val_len = strlen(value) + 1;
    next->value = gc_calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, value);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map Map_AddInt(Map map, const char* key, int value) {
    // if this key already exists, we skip
    if (Map_GetString(map, key))
        return map;

    Map next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = gc_calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[64]; sprintf_s(v, 64, "%d", value);
    size_t val_len = strlen(v) + 1;
    next->value = gc_calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map Map_AddFloat(Map map, const char* key, float value) {
    // if this key already exists, we skip
    if (Map_GetString(map, key))
        return map;

    Map next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = gc_calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[64]; sprintf_s(v, 64, "%f", value);
    size_t val_len = strlen(v) + 1;
    next->value = gc_calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map Map_AddBool(Map map, const char* key, bool value) {
    // if this key already exists, we skip
    if (Map_GetString(map, key))
        return map;

    Map next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = gc_calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[6]; sprintf_s(v, 6, "%s", value ? "true" : "false");
    size_t val_len = strlen(v) + 1;
    next->value = gc_calloc(val_len, sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

Map Map_AddVector(Map map, const char* key, CP_Vector value) {
    // if this key already exists, we skip
    if (Map_GetString(map, key))
        return map;

    Map next = add_node(map);

    // +1 because last character is '\0' to terminate the string
    size_t key_len = strlen(key) + 1;
    next->key = gc_calloc(key_len, sizeof(char));
    strcpy_s(next->key, key_len, key);

    // set the value (right side)
    char v[128]; sprintf_s(v, 128, "%f,%f", value.x, value.y);
    size_t val_len = strlen(v) + 1;
    next->value = gc_malloc(val_len * sizeof(char));
    strcpy_s(next->value, val_len, v);

    next->next = NULL;
    return map == NULL ? next : map;
}

void Map_EditString(Map map, const char* key, char* value) {
    // loop through each node to find the key we are looking for
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            // free up the current value
            gc_free(ptr->value);

            // set the new value
            size_t val_len = strlen(value) + 1;
            ptr->value = gc_calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, value);
            return; // after modified, we dont need to look anymore
        }
    }
    Map_AddString(map, key, value);
}

void Map_EditInt(Map map, const char* key, int value) {
    // loop through each node to find the key we are looking for
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            // free up the current value
            gc_free(ptr->value);

            // set the new value
            char v[64]; sprintf_s(v, 64, "%d", value);
            size_t val_len = strlen(v) + 1;
            ptr->value = gc_calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return; // after modified, we dont need to look anymore
        }
    }
    Map_AddInt(map, key, value);
}

void Map_EditFloat(Map map, const char* key, float value) {
    // loop through each node to find the key we are looking for
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            // free up the current value
            gc_free(ptr->value);

            // set the new value
            char v[64]; sprintf_s(v, 64, "%f", value);
            size_t val_len = strlen(v) + 1;
            ptr->value = gc_calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return; // after modified, we dont need to look anymore
        }
    }
    Map_AddFloat(map, key, value);
}

void Map_EditVector(Map map, const char* key, CP_Vector value) {
    // loop through each node to find the key we are looking for
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            // free up the current value
            gc_free(ptr->value);

            // set the new value
            char v[128]; sprintf_s(v, 128, "%f,%f", value.x, value.y);
            size_t val_len = strlen(v) + 1;
            ptr->value = gc_calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return; // after modified, we dont need to look anymore
        }
    }
    Map_AddVector(map, key, value);
}

void Map_EditBool(Map map, const char* key, bool value) {
    // loop through each node to find the key we are looking for
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            // free up the current value
            gc_free(ptr->value);

            // set the new value
            char v[6]; sprintf_s(v, 6, "%s", value ? "true" : "false");
            size_t val_len = strlen(v) + 1;
            ptr->value = gc_calloc(val_len, sizeof(char));
            strcpy_s(ptr->value, val_len, v);
            return; // after modified, we dont need to look anymore
        }
    }
    Map_AddBool(map, key, value);
}

Map Map_Remove(Map map, const char* key) {
    // first one in the map
    if (strcmp(map->key, key) == 0) {
        Map next = map->next;
        FREE_MEMORY(map->key);
        FREE_MEMORY(map->value);
        FREE_MEMORY(map);
        return next;
    }
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        // inside the map
        if (ptr->next) {
            if (strcmp(ptr->next->key, key) == 0) { // if match, we bridge the previous and the next one
                Map todel = ptr->next;
                ptr->next = todel->next;

                // ensure to free up the memory allocated for each node
                FREE_MEMORY(todel->key);
                FREE_MEMORY(todel->value);
                FREE_MEMORY(todel);
                break;
            }
        }
    }
    return map;
}

bool Map_Save(const char* filename, Map map) {
    FILE* fp; // safe open
    fopen_s(&fp, filename, "w");
    if (!fp) {
        return false;
    }

    // add each node to the map
    Map next = map;
    while (next) {
        fputs(next->key, fp); // key value pair (e.g. key=value)
        fputc('=', fp);
        fputs(next->value, fp);
        if (next = next->next) {
            fputc('\n', fp);
        }
    }

    fclose(fp);
    fp = NULL;
    return true;
}

char* Map_GetString(Map map, const char* key) {
    // loop through each map node and compare the key value with the given key
    for (Map ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            return ptr->value;
        }
    }
    return NULL;
}

int Map_GetInt(Map map, const char* key, int def) {
    char* value = Map_GetString(map, key);
    return value ? atoi(value) : def;
}

float Map_GetFloat(Map map, const char* key, float def) {
    char* value = Map_GetString(map, key);
    return value ? (float)atof(value) : def;
}

bool Map_GetBool(Map map, const char* key, bool def) {
    char* value = Map_GetString(map, key);
    return value ? strcmp(value, "true") == 0 : def;
}

CP_Vector Map_GetVector(Map map, const char* key) {
    char* value = Map_GetString(map, key);
    return value ? parse_vector(value) : CP_Vector_Set(0, 0);
}

// recursively loops through the map to free up the allocations, starting from the last node
void Map_Free(Map map) {
    if (map == NULL)
        return;

    if (map->next) {
        // if it has a next, recursive call the next node
        Map_Free(map->next);
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
