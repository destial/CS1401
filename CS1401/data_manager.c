/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 28 October 2025
@course CSD1401
@brief Implementation file for loading and saving text files
*/

#include "data_manager.h"

char* get_value(Map* map, const char* key) {
    // loop through each map node and compare the key value with the given key
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            return ptr->value;
        }
    }
    return NULL;
}

Map* load_map(const char* filename) {
    char* contents = read_content(filename);
    if (contents == NULL)
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

// sourced from: https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
char* read_content(const char* filename) {
    char* fcontent = NULL;
    int fsize = 0;
    FILE* fp = fopen(filename, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = calloc(fsize, sizeof(char));
        if (fcontent) {
            fread(fcontent, 1, fsize, fp);
        }
        fclose(fp);
        fp = NULL;
    }
    return fcontent;
}

void write_content(const char* filename, char* content) {
    FILE* fp = fopen(filename, "w");
    if (fp) {
        fputs(content, fp);
        fclose(fp);
        fp = NULL;
    }
}

// tokenize the given string from (131231,14512) to a proper vector
CP_Vector get_vector(char* line) {
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
        free(map->key);
        map->key = NULL;
    }
    if (map->value) {
        free(map->value);
        map->value = NULL;
    }
    free(map);
    map = NULL;
}
