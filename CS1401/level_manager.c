#include "level_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

char* get_value(Map* map, const char* key) {
    for (Map* ptr = map; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            return ptr->value;
        }
    }
}

Level* get_level(int level) {
    char level_path[100];
    sprintf_s(level_path, 100, "Assets/Levels/level_%d.lvl", level);
    char* level_file = read_content(level_path);
    const char outerdel[] = "\n";
    const char innerdel[] = "=";

    Map* root = malloc(sizeof(Map));
    Map* next = root;
    if (level_file != NULL) {
        char* outerptr = NULL;
        char* innerptr = NULL;

        char* k, * v;
        char* line = strtok_s(level_file, outerdel, &outerptr);
        while (line) {
            // key
            k = strtok_s(line, innerdel, &innerptr);
            next->key = malloc(strlen(k) + 1);
            strcpy_s(next->key, strlen(k) + 1, k);

            // value
            v = strtok_s(NULL, innerdel, &innerptr);
            next->value = malloc(strlen(v) + 1);
            strcpy_s(next->value, strlen(v) + 1, v);

            // end of check
            line = strtok_s(NULL, outerdel, &outerptr);
            if (line) {
                next->next = malloc(sizeof(Map));
                next = next->next;
            }
            else {
                next->next = NULL;
            }
        }
        free(level_file);

        Level* lvl = malloc(sizeof(Level));
        lvl->world_width = atof(get_value(root, "width"));
        lvl->world_height = atof(get_value(root, "height"));

        lvl->walls_size = atoi(get_value(root, "walls_amount"));
        lvl->walls = malloc(sizeof(float) * lvl->walls_size * 5);

        lvl->golf_pos_1 = get_vector(get_value(root, "golf_1"));
        lvl->golf_pos_2 = get_vector(get_value(root, "golf_2"));

        lvl->hole_1 = get_vector(get_value(root, "hole_1"));
        lvl->hole_2 = get_vector(get_value(root, "hole_2"));

        int wall_index = 0;
        for (int i = 0; i < lvl->walls_size * 5; i += 5, wall_index++) {
            char wall_pos[32];
            sprintf_s(wall_pos, 32, "wall_%d_pos", wall_index);
            CP_Vector pos = get_vector(get_value(root, wall_pos));

            char wall_scale[32];
            sprintf_s(wall_scale, 32, "wall_%d_scale", wall_index);
            CP_Vector scale = get_vector(get_value(root, wall_scale));

            char wall_angle[32];
            sprintf_s(wall_angle, 32, "wall_%d_angle", wall_index);
            float angle = atof(get_value(root, wall_angle));

            lvl->walls[i + 0] = pos.x;
            lvl->walls[i + 1] = pos.y;
            lvl->walls[i + 2] = scale.x;
            lvl->walls[i + 3] = scale.y;
            lvl->walls[i + 4] = angle;              
        }
        free_map(root);

        return lvl;
    }

    return NULL;
}

char* read_content(const char* filename) {
    char* fcontent = NULL;
    int fsize = 0;
    FILE* fp = fopen(filename, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = (char*)malloc(sizeof(char) * fsize);
        fread(fcontent, 1, fsize, fp);

        fclose(fp);
    }
    return fcontent;
}

CP_Vector get_vector(char* line) {
    const char innerdel[] = ",";
    char* innerptr = NULL;
    char* xstr = strtok_s(line, innerdel, &innerptr);
    float x = atof(xstr);
    char* ystr = strtok_s(NULL, innerdel, &innerptr);
    float y = atof(ystr);
    return CP_Vector_Set(x, y);
}

void free_map(Map* map) {
    if (map->next) {
        free_map(map->next);
    }
    free(map->key);
    free(map->value);
    free(map);
}
