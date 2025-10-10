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

#ifndef FILE_SIZE
#define FILE_SIZE 8192
#endif

Level* get_level(int level) {
    char level_path[128];
    sprintf_s(level_path, 128, LEVEL_FILEPATH, level);

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
            // set the key (left side)
            k = strtok_s(line, innerdel, &innerptr);

            // +1 because last character is '\0' to terminate the string
            size_t key_len = strlen(k) + 1;
            next->key = malloc(key_len);
            strcpy_s(next->key, key_len, k);

            // set the value (right side)
            v = strtok_s(NULL, innerdel, &innerptr);
            size_t val_len = strlen(v) + 1;
            next->value = malloc(val_len);
            strcpy_s(next->value, val_len, v);

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
            if (lvl->walls_size) {
                lvl->walls = malloc(sizeof(float) * lvl->walls_size * WALLS_OFFSET);
            }
            else {
                lvl->walls = NULL;
            }

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

            char* item_size_line = get_value(root, "items_amount");
            if (item_size_line) {
                lvl->items_size = atoi(item_size_line);
                lvl->items = malloc(sizeof(float) * lvl->items_size * ITEMS_OFFSET);

                int item_index = 0;
                for (int i = 0; i < lvl->items_size * ITEMS_OFFSET; i += ITEMS_OFFSET, item_index++) {
                    char item_pos[32];
                    sprintf_s(item_pos, 32, "item_%d_pos", item_index);
                    char* line_pos = get_value(root, item_pos);
                    if (line_pos) {
                        CP_Vector pos = get_vector(line_pos);
                        lvl->items[i + 0] = pos.x;
                        lvl->items[i + 1] = pos.y;
                    }
                    else {
                        lvl->items[i + 0] = 0.f;
                        lvl->items[i + 1] = 0.f;
                    }

                    char item_diameter[32];
                    sprintf_s(item_diameter, 32, "item_%d_diameter", item_index);
                    char* line_diameter = get_value(root, item_diameter);
                    if (line_diameter) {
                        float diameter = (float)atof(line_diameter);
                        lvl->items[i + 2] = diameter;
                    }
                    else {
                        lvl->items[i + 2] = 0.f;
                    }

                    char item_angle[32];
                    sprintf_s(item_angle, 32, "item_%d_angle", item_index);
                    char* line_angle = get_value(root, item_angle);
                    if (line_angle) {
                        float angle = (float)atof(line_angle);
                        lvl->items[i + 3] = angle;
                    }
                    else {
                        lvl->items[i + 3] = 0.f;
                    }

                    char item_rot_rate[32];
                    sprintf_s(item_rot_rate, 32, "item_%d_rot_rate", item_index);
                    char* line_rot_rate = get_value(root, item_rot_rate);
                    if (line_rot_rate) {
                        float rot_rate = (float)atof(line_rot_rate);
                        lvl->items[i + 4] = rot_rate;
                    }
                    else {
                        lvl->items[i + 4] = 0.f;
                    }

                    char item_type[32];
                    sprintf_s(item_type, 32, "item_%d_type", item_index);
                    char* line_type = get_value(root, item_type);
                    if (line_type) {
                        int type = atoi(line_type);
                        lvl->items[i + 5] = (float) type;
                    }
                    else {
                        lvl->items[i + 5] = 0;
                    }
                }
            }
            else {
                lvl->items_size = 0;
                lvl->items = NULL;
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

void save_level(int level, Wall* walls, int walls_amount, GolfBall* golf_balls, Hole* holes, Item* items, int items_amount, CP_Vector world_limits, int par) {
    char level_path[128];
    sprintf_s(level_path, 128, LEVEL_FILEPATH, level);
    FILE* fptr = fopen(level_path, "w");

    // save the world limits, golf balls, holes and middle wall first
    char level_file[FILE_SIZE];
    sprintf_s(level_file, FILE_SIZE,
        "width=%f\nheight=%f\ngolf_1=%f,%f\ngolf_2=%f,%f\nhole_1=%f,%f\nhole_2=%f,%f\nmiddle_wall_angle=%f",
        world_limits.x, world_limits.y,
        golf_balls[0].pos.x, golf_balls[0].pos.y,
        golf_balls[1].pos.x, golf_balls[1].pos.y,
        holes[0].pos.x, holes[0].pos.y,
        holes[1].pos.x, holes[1].pos.y,
        walls->angle
    );

    // save the walls now
    int wall_index = 0;
    for (int i = 1; i < walls_amount; i++) {
        Wall* wall = walls + i;
        if (wall == NULL || wall->destroyed)
            continue;

        if (wall->scale.x == 0.f || wall->scale.y == 0.f)
            continue;

        char wall_data[1024];
        sprintf_s(wall_data, 1024,
            "\nwall_%d_pos=%f,%f\nwall_%d_scale=%f,%f\nwall_%d_angle=%f\nwall_%d_rot_rate=%f",
            wall_index, wall->pos.x, wall->pos.y,
            wall_index, wall->scale.x, wall->scale.y,
            wall_index, wall->angle,
            wall_index, wall->rotation_rate
        );
        strcat_s(level_file, FILE_SIZE, wall_data);
        wall_index++;
    }

    // save the amount of walls saved
    char last[64];
    sprintf_s(last, 64, "\nwalls_amount=%d", wall_index);
    strcat_s(level_file, FILE_SIZE, last);

    int item_index = 0;
    for (int i = 0; i < items_amount; i++) {
        Item* item = items + i;
        if (item == NULL)
            continue;

        if (item->destroyed || item->diameter <= 0.f)
            continue;

        char item_data[1024];
        sprintf_s(item_data, 1024,
            "\nitem_%d_pos=%f,%f\nitem_%d_diameter=%f\nitem_%d_angle=%f\nitem_%d_rot_rate=%f\nitem_%d_type=%d",
            item_index, item->pos.x, item->pos.y,
            item_index, item->diameter,
            item_index, item->angle,
            item_index, item->rotation_rate,
            item_index, item->type
        );
        strcat_s(level_file, FILE_SIZE, item_data);
        item_index++;
    }

    char ilast[64];
    sprintf_s(ilast, 64, "\nitems_amount=%d", item_index);
    strcat_s(level_file, FILE_SIZE, ilast);

    fputs(level_file, fptr);
    fclose(fptr);
    fptr = NULL;
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
    if (level->walls) {
        free(level->walls);
    }
    if (level->items) {
        free(level->items);
    }
    free(level);
    level = NULL;
}
