/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 16 September 2025
@course CSD1401
@brief Definition file for loading levels from text file into memory
*/

#include "level_manager.h"
#include "asset_manager.h"
#include "end_state.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

Level* get_level(int level) {
    char level_path[100];
    sprintf_s(level_path, 100, LEVEL_FILEPATH, level);

    // read the level text file
    char* level_file = read_content(level_path);

    // make sure this file actually exists
    if (level_file != NULL) {
        //trim_string(level_file);
        // set contexts for string tokenizer ('\n' to split between lines, '=' to split between key and value)
        const char outerdel[] = "\n";
        const char innerdel[] = "=";

        // allocate the root of this level map
        Map* root = malloc(sizeof(Map));
        Map* next = root;
        char* outerptr = NULL;
        char* innerptr = NULL;

        // set initial variable pointers for string tokenization (strtok)
        char* k, * v;
        char* line = strtok_s(level_file, outerdel, &outerptr);
        while (line) {
            // +1 because last character is '\0' to terminate the string
            // set the key (left side)
            k = strtok_s(line, innerdel, &innerptr);
            next->key = malloc(strlen(k) + 1);
            strcpy_s(next->key, strlen(k) + 1, k);

            // set the value (right side)
            v = strtok_s(NULL, innerdel, &innerptr);
            next->value = malloc(strlen(v) + 1);
            strcpy_s(next->value, strlen(v) + 1, v);

            // end of check, go to next line or quit if already at the end
            line = strtok_s(NULL, outerdel, &outerptr);
            if (line) {
                next->next = malloc(sizeof(Map));
                next = next->next;
            }
            else {
                next->next = NULL;
            }
        }
        next = NULL;

        // make sure to free up allocated memory for the text file contents
        free(level_file);
        level_file = NULL;

        // create the level to load and set all the values *** all must have values!!!
        Level* lvl = malloc(sizeof(Level));
        if (lvl) {
            lvl->world_width = (float)atof(get_value(root, "width"));
            lvl->world_height = (float)atof(get_value(root, "height"));

            lvl->walls_size = atoi(get_value(root, "walls_amount"));
            lvl->walls = malloc(sizeof(float) * lvl->walls_size * WALLS_OFFSET);

            lvl->golf_pos_1 = get_vector(get_value(root, "golf_1"));
            lvl->golf_pos_2 = get_vector(get_value(root, "golf_2"));

            lvl->hole_1 = get_vector(get_value(root, "hole_1"));
            lvl->hole_2 = get_vector(get_value(root, "hole_2"));

            // some versions of the level file dont include the par, so make sure to check
            char* line_par = get_value(root, "par");
            if (line_par) {
                lvl->par = atoi(line_par);
            }
            else {
                lvl->par = 3;
            }

            char* line_m_angle = get_value(root, "middle_wall_angle");
            if (line_m_angle) {
                lvl->middle_wall_angle = (float)atof(line_m_angle);
            }
            else {
                lvl->middle_wall_angle = 0.f;
            }

            // set the walls position, scaling, angle and rotation rates
            // check if any of them exists first, if not use default values
            int wall_index = 0;
            for (int i = 0; i < lvl->walls_size * WALLS_OFFSET; i += WALLS_OFFSET, wall_index++) {

                // set wall position if exists
                char wall_pos[32];
                sprintf_s(wall_pos, 32, "wall_%d_pos", wall_index);
                char* line_pos = get_value(root, wall_pos);
                if (line_pos) {
                    CP_Vector pos = get_vector(line_pos);
                    lvl->walls[i + 0] = pos.x;
                    lvl->walls[i + 1] = pos.y;
                }
                else {
                    lvl->walls[i + 0] = 0.f;
                    lvl->walls[i + 1] = 0.f;
                }

                // set wall scale if exists
                char wall_scale[32];
                sprintf_s(wall_scale, 32, "wall_%d_scale", wall_index);
                char* line_scale = get_value(root, wall_scale);
                if (line_scale) {
                    CP_Vector scale = get_vector(line_scale);
                    lvl->walls[i + 2] = scale.x;
                    lvl->walls[i + 3] = scale.y;
                }
                else {
                    lvl->walls[i + 2] = 0.f;
                    lvl->walls[i + 3] = 0.f;
                }

                // set wall angle if exists
                char wall_angle[32];
                sprintf_s(wall_angle, 32, "wall_%d_angle", wall_index);
                char* line_angle = get_value(root, wall_angle);
                if (line_angle) {
                    float angle = (float)atof(line_angle);
                    lvl->walls[i + 4] = angle;
                }
                else {
                    lvl->walls[i + 4] = 0.f;
                }

                // set wall rotation rate if exists
                char wall_rot[32];
                sprintf_s(wall_rot, 32, "wall_%d_rot_rate", wall_index);
                char* line_rot = get_value(root, wall_rot);
                if (line_rot) {
                    float rot = (float)atof(line_rot);
                    lvl->walls[i + 5] = rot;
                }
                else {
                    lvl->walls[i + 5] = 0.f;
                }
            }

            // we are done loading the level, we can now free up the map
            free_map(root);
            root = NULL;
            return lvl;
        } else {
            CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
        }
    }
    
    // no level file detected, so no level will be loaded
    return NULL;
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

// sourced from: https://stackoverflow.com/questions/3381080/reading-all-content-from-a-text-file-c
char* read_content(const char* filename) {
    char* fcontent = NULL;
    int fsize = 0;
    FILE* fp = fopen(filename, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = malloc(sizeof(char) * fsize);
        if (fcontent) {
            fread(fcontent, 1, fsize, fp);
        }
        fclose(fp);
        fp = NULL;
    }
    return fcontent;
}

// tokenize the given string from (131231,14512) to a proper vector
CP_Vector get_vector(char* line) {
    const char innerdel[] = ",";
    char* innerptr = NULL;

    // get the x value (left side)
    char* xstr = strtok_s(line, innerdel, &innerptr);
    float x = (float) atof(xstr);

    // get the y value (right side)
    char* ystr = strtok_s(NULL, innerdel, &innerptr);
    float y = (float) atof(ystr);
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
    if (map->key != NULL) {
        free(map->key);
        map->key = NULL;
    }
    if (map->value != NULL) {
        free(map->value);
        map->value = NULL;
    }
    free(map);
    map = NULL;
}

void free_level(Level* level) {
    free(level->walls);
    free(level);
    level = NULL;
}

// sourced from: https://www.delftstack.com/howto/c/trim-string-in-c
void trim_string(char* str) {
    while (isspace((unsigned char)str[0])) {
        memmove(str, str + 1, strlen(str));
    }

    while (isspace((unsigned char)str[strlen(str) - 1])) {
        str[strlen(str) - 1] = '\0';
    }
}