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
            lvl->walls = lvl->walls_size ? malloc(sizeof(float) * lvl->walls_size * WALLS_OFFSET) : NULL;

            char* old_format = get_value(root, "golf_1");
            if (old_format) {
                lvl->golf_balls_size = 2;
                lvl->golf_balls = malloc(sizeof(float) * lvl->golf_balls_size * BALL_HOLE_OFFSET);

                CP_Vector ball_1 = get_vector(old_format);
                CP_Vector ball_2 = get_vector(get_value(root, "golf_2"));
                lvl->golf_balls[0] = ball_1.x;
                lvl->golf_balls[1] = ball_1.y;
                lvl->golf_balls[2] = 1.f;

                lvl->golf_balls[3] = ball_2.x;
                lvl->golf_balls[4] = ball_2.y;
                lvl->golf_balls[5] = 1.f;

                CP_Vector hole_1 = get_vector(get_value(root, "hole_1"));
                CP_Vector hole_2 = get_vector(get_value(root, "hole_2"));

                lvl->holes_size = 2;
                lvl->holes = malloc(sizeof(float) * lvl->holes_size * BALL_HOLE_OFFSET);
                lvl->holes[0] = hole_1.x;
                lvl->holes[1] = hole_1.y;
                lvl->holes[2] = 1.5f;

                lvl->holes[3] = hole_2.x;
                lvl->holes[4] = hole_2.y;
                lvl->holes[5] = 1.5f;
            }
            else {
                lvl->golf_balls_size = atoi(get_value(root, "golf_amount"));
                lvl->golf_balls = malloc(sizeof(float) * lvl->golf_balls_size * BALL_HOLE_OFFSET);

                int ball_index = 0;
                for (int i = 0; i < lvl->golf_balls_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET, ++ball_index) {
                    char ball_pos[32];
                    sprintf_s(ball_pos, 32, "golf_%d_pos", ball_index);
                    char* line_pos = get_value(root, ball_pos);
                    CP_Vector pos = line_pos ? get_vector(line_pos) : CP_Vector_Set(0, 0);
                    lvl->golf_balls[i + 0] = pos.x;
                    lvl->golf_balls[i + 1] = pos.y;

                    char ball_diameter[32];
                    sprintf_s(ball_diameter, 32, "golf_%d_diameter", ball_index);
                    char* line_diameter = get_value(root, ball_diameter);
                    lvl->golf_balls[i + 2] = line_diameter ? (float) atof(line_diameter) : 1.f;
                }

                lvl->holes_size = atoi(get_value(root, "holes_amount"));
                lvl->holes = malloc(sizeof(float) * lvl->holes_size * BALL_HOLE_OFFSET);

                int hole_index = 0;
                for (int i = 0; i < lvl->holes_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET, ++hole_index) {
                    char hole_pos[32];
                    sprintf_s(hole_pos, 32, "hole_%d_pos", hole_index);
                    char* line_pos = get_value(root, hole_pos);
                    CP_Vector pos = line_pos ? get_vector(line_pos) : CP_Vector_Set(0, 0);
                    lvl->holes[i + 0] = pos.x;
                    lvl->holes[i + 1] = pos.y;

                    char hole_diameter[32];
                    sprintf_s(hole_diameter, 32, "hole_%d_diameter", hole_index);
                    char* line_diameter = get_value(root, hole_diameter);
                    lvl->holes[i + 2] = line_diameter ? (float) atof(line_diameter) : 1.5f;
                }
            }

            // some versions of the level file dont include the par, so make sure to check
            char* line_par = get_value(root, "par");
            lvl->par = line_par ? atoi(line_par) : 3;

            char* line_m_angle = get_value(root, "middle_wall_angle");
            lvl->middle_wall_angle = line_m_angle ? (float)atof(line_m_angle) : 0.f;

            // set the walls position, scaling, angle and rotation rates
            // check if any of them exists first, if not use default values
            int wall_index = 0;
            for (int i = 0; i < lvl->walls_size * WALLS_OFFSET; i += WALLS_OFFSET, ++wall_index) {

                // set wall position if exists
                char wall_pos[32];
                sprintf_s(wall_pos, 32, "wall_%d_pos", wall_index);
                char* line_pos = get_value(root, wall_pos);
                CP_Vector pos = line_pos ? get_vector(line_pos) : CP_Vector_Set(0, 0);
                lvl->walls[i + 0] = pos.x;
                lvl->walls[i + 1] = pos.y;

                // set wall scale if exists
                char wall_scale[32];
                sprintf_s(wall_scale, 32, "wall_%d_scale", wall_index);
                char* line_scale = get_value(root, wall_scale);
                CP_Vector scale = line_scale ? get_vector(line_scale) : CP_Vector_Set(0, 0);
                lvl->walls[i + 2] = scale.x;
                lvl->walls[i + 3] = scale.y;

                // set wall angle if exists
                char wall_angle[32];
                sprintf_s(wall_angle, 32, "wall_%d_angle", wall_index);
                char* line_angle = get_value(root, wall_angle);
                lvl->walls[i + 4] = line_angle ? (float)atof(line_angle) : 0.f;

                // set wall rotation rate if exists
                char wall_rot[32];
                sprintf_s(wall_rot, 32, "wall_%d_rot_rate", wall_index);
                char* line_rot = get_value(root, wall_rot);
                lvl->walls[i + 5] = line_rot ? (float)atof(line_rot) : 0.f;
            }

            // load items in level
            char* item_size_line = get_value(root, "items_amount");
            if (item_size_line) {
                lvl->items_size = atoi(item_size_line);
                lvl->items = malloc(sizeof(float) * lvl->items_size * ITEMS_OFFSET);

                int item_index = 0;
                for (int i = 0; i < lvl->items_size * ITEMS_OFFSET; i += ITEMS_OFFSET, ++item_index) {
                    char item_pos[32];
                    sprintf_s(item_pos, 32, "item_%d_pos", item_index);
                    char* line_pos = get_value(root, item_pos);
                    CP_Vector pos = line_pos ? get_vector(line_pos) : CP_Vector_Set(0, 0);
                    lvl->items[i + 0] = pos.x;
                    lvl->items[i + 1] = pos.y;

                    char item_diameter[32];
                    sprintf_s(item_diameter, 32, "item_%d_diameter", item_index);
                    char* line_diameter = get_value(root, item_diameter);
                    lvl->items[i + 2] = line_diameter ? (float)atof(line_diameter) : 0.f;

                    char item_angle[32];
                    sprintf_s(item_angle, 32, "item_%d_angle", item_index);
                    char* line_angle = get_value(root, item_angle);
                    lvl->items[i + 3] = line_angle ? (float)atof(line_angle) : 0.f;

                    char item_type[32];
                    sprintf_s(item_type, 32, "item_%d_type", item_index);
                    char* line_type = get_value(root, item_type);
                    lvl->items[i + 4] = line_type ? (float)atoi(line_type) : 0.f;
                }
            }
            else {
                lvl->items_size = 0;
                lvl->items = NULL;
            }

            // load terrains in level
            char* terrain_size_line = get_value(root, "terrains_amount");
            if (terrain_size_line) {
                lvl->terrains_size = atoi(terrain_size_line);
                lvl->terrains = malloc(sizeof(float) * lvl->terrains_size * TERRAINS_OFFSET);

                int terrain_index = 0;
                for (int i = 0; i < lvl->terrains_size * TERRAINS_OFFSET; i += TERRAINS_OFFSET, ++terrain_index) {
                    char terrain_pos[32];
                    sprintf_s(terrain_pos, 32, "terrain_%d_pos", terrain_index);
                    char* line_pos = get_value(root, terrain_pos);
                    CP_Vector pos = line_pos ? get_vector(line_pos) : CP_Vector_Set(0, 0);
                    lvl->terrains[i + 0] = pos.x;
                    lvl->terrains[i + 1] = pos.y;

                    char terrain_scale[32];
                    sprintf_s(terrain_scale, 32, "terrain_%d_scale", terrain_index);
                    char* line_scale = get_value(root, terrain_scale);
                    CP_Vector scale = line_pos ? get_vector(line_scale) : CP_Vector_Set(0, 0);
                    lvl->terrains[i + 2] = scale.x;
                    lvl->terrains[i + 3] = scale.y;

                    char terrain_angle[32];
                    sprintf_s(terrain_angle, 32, "terrain_%d_angle", terrain_index);
                    char* line_angle = get_value(root, terrain_angle);
                    lvl->terrains[i + 4] = line_angle ? (float)atof(line_angle) : 0.f;

                    char terrain_type[32];
                    sprintf_s(terrain_type, 32, "terrain_%d_type", terrain_index);
                    char* line_type = get_value(root, terrain_type);
                    lvl->terrains[i + 5] = line_type ? (float)atoi(line_type) : 0.f;
                }
            }
            else {
                lvl->terrains_size = 0;
                lvl->terrains = NULL;
            }

            // we are done loading the level, we can now free up the map
            free_map(root);
            root = NULL;
            return lvl;
        }
        else {
            CP_Engine_SetNextGameStateForced(end_state_init, end_state_update, end_state_end);
        }
    }
    
    // no level file detected, so no level will be loaded
    return NULL;
}

void save_level(int level, ObjectArray* walls, ObjectArray* golf_balls, ObjectArray* holes, ObjectArray* items, ObjectArray* terrains, CP_Vector world_limits, int par) {
    char level_path[128];
    sprintf_s(level_path, 128, LEVEL_FILEPATH, level);
    FILE* fptr = fopen(level_path, "w");

    // save the world limits, golf balls, holes and middle wall first
    char level_file[FILE_SIZE];
    sprintf_s(level_file, FILE_SIZE,
        "width=%f\nheight=%f\nmiddle_wall_angle=%f",
        world_limits.x, world_limits.y,
        ((Wall*)walls->arr)->angle
    );

    int ball_index = 0;
    for (int i = 0; i < golf_balls->length; ++i) {
        GolfBall* ball = (GolfBall*)golf_balls->arr + i;
        if (ball == NULL || ball->destroyed || ball->diameter <= 0.f)
            continue;

        char ball_data[128];
        sprintf_s(ball_data, 128,
            "\ngolf_%d_pos=%f,%f\ngolf_%d_diameter=%f",
            ball_index, ball->pos.x, ball->pos.y,
            ball_index, ball->diameter
        );
        strcat_s(level_file, FILE_SIZE, ball_data);
        ++ball_index;
    }

    // save the amount of golf balls saved
    char last_balls[64];
    sprintf_s(last_balls, 64, "\ngolf_amount=%d", ball_index);
    strcat_s(level_file, FILE_SIZE, last_balls);

    int hole_index = 0;
    for (int i = 0; i < holes->length; ++i) {
        Hole* hole = (Hole*)holes->arr + i;
        if (hole == NULL || hole->destroyed || hole->diameter <= 0.f)
            continue;

        char hole_data[128];
        sprintf_s(hole_data, 128,
            "\nhole_%d_pos=%f,%f\nhole_%d_diameter=%f",
            hole_index, hole->pos.x, hole->pos.y,
            hole_index, hole->diameter
        );
        strcat_s(level_file, FILE_SIZE, hole_data);
        ++hole_index;
    }

    // save the amount of holes saved
    char last_holes[64];
    sprintf_s(last_holes, 64, "\nholes_amount=%d", hole_index);
    strcat_s(level_file, FILE_SIZE, last_holes);

    // save the walls now
    int wall_index = 0;
    for (int i = 1; i < walls->length; ++i) {
        Wall* wall = (Wall*)walls->arr + i;
        if (wall == NULL || wall->destroyed || wall->scale.x <= 0.f || wall->scale.y <= 0.f)
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
        ++wall_index;
    }

    // save the amount of walls saved
    char last_walls[64];
    sprintf_s(last_walls, 64, "\nwalls_amount=%d", wall_index);
    strcat_s(level_file, FILE_SIZE, last_walls);

    int item_index = 0;
    for (int i = 0; i < items->length; ++i) {
        Item* item = (Item*)items->arr + i;
        if (item == NULL || item->destroyed || item->diameter <= 0.f)
            continue;

        char item_data[1024];
        sprintf_s(item_data, 1024,
            "\nitem_%d_pos=%f,%f\nitem_%d_diameter=%f\nitem_%d_angle=%f\nitem_%d_type=%d",
            item_index, item->pos.x, item->pos.y,
            item_index, item->diameter,
            item_index, item->angle,
            item_index, item->type
        );
        strcat_s(level_file, FILE_SIZE, item_data);
        ++item_index;
    }

    char last_items[64];
    sprintf_s(last_items, 64, "\nitems_amount=%d", item_index);
    strcat_s(level_file, FILE_SIZE, last_items);

    int terrain_index = 0;
    for (int i = 0; i < terrains->length; ++i) {
        Terrain* terrain = (Terrain*)terrains->arr + i;
        if (terrain == NULL || terrain->destroyed || terrain->scale.x <= 0.f)
            continue;

        char terrain_data[1024];
        sprintf_s(terrain_data, 1024,
            "\nterrain_%d_pos=%f,%f\nterrain_%d_scale=%f,%f\nterrain_%d_angle=%f\nterrain_%d_type=%d",
            terrain_index, terrain->pos.x, terrain->pos.y,
            terrain_index, terrain->scale.x, terrain->scale.y,
            terrain_index, terrain->angle,
            terrain_index, terrain->type
        );
        strcat_s(level_file, FILE_SIZE, terrain_data);
        ++terrain_index;
    }

    char last_terrains[64];
    sprintf_s(last_terrains, 64, "\nterrains_amount=%d", terrain_index);
    strcat_s(level_file, FILE_SIZE, last_terrains);

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
        level->walls = NULL;
    }
    if (level->items) {
        free(level->items);
        level->items = NULL;
    }
    if (level->terrains) {
        free(level->terrains);
        level->terrains = NULL;
    }
    free(level);
    level = NULL;
}
