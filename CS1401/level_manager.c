/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 16 September 2025
@course CSD1401
@brief Definition file for loading levels from text file into memory
*/

#include "level_manager.h"
#include "asset_manager.h"
#include "end_state.h"
#include "data_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

Level* get_level(int level) {
    char level_path[128];
    sprintf_s(level_path, 128, LEVEL_FILEPATH, level);

    Map* root = load_map(level_path);
    // make sure this file actually exists
    if (root != NULL) {
        // create the level to load and set all the values *** all must have values!!!
        Level* lvl = malloc(sizeof(Level));
        if (lvl) {
            lvl->world_width = get_float(root, "width", 48.f);
            lvl->world_height = get_float(root, "height", 27.f);

            lvl->walls_size = get_int(root, "walls_amount", 1);
            lvl->walls = lvl->walls_size ? calloc((size_t)lvl->walls_size * WALLS_OFFSET, sizeof(float)) : NULL;

            char* old_format = get_value(root, "golf_1");
            if (old_format) {
                lvl->golf_balls_size = 2;
                lvl->golf_balls = calloc((size_t)lvl->golf_balls_size * BALL_HOLE_OFFSET, sizeof(float));

                CP_Vector ball_1 = get_vector(root, "golf_1");
                CP_Vector ball_2 = get_vector(root, "golf_2");
                lvl->golf_balls[0] = ball_1.x;
                lvl->golf_balls[1] = ball_1.y;
                lvl->golf_balls[2] = 1.f;

                lvl->golf_balls[3] = ball_2.x;
                lvl->golf_balls[4] = ball_2.y;
                lvl->golf_balls[5] = 1.f;

                CP_Vector hole_1 = get_vector(root, "hole_1");
                CP_Vector hole_2 = get_vector(root, "hole_2");

                lvl->holes_size = 2;
                lvl->holes = calloc(lvl->holes_size * BALL_HOLE_OFFSET, sizeof(float));
                lvl->holes[0] = hole_1.x;
                lvl->holes[1] = hole_1.y;
                lvl->holes[2] = 1.5f;

                lvl->holes[3] = hole_2.x;
                lvl->holes[4] = hole_2.y;
                lvl->holes[5] = 1.5f;
            }
            else {
                lvl->golf_balls_size = get_int(root, "golf_amount", 1);
                lvl->golf_balls = calloc((size_t)lvl->golf_balls_size * BALL_HOLE_OFFSET, sizeof(float));

                int ball_index = 0;
                for (int i = 0; i < lvl->golf_balls_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET, ++ball_index) {
                    char ball_pos[32];
                    sprintf_s(ball_pos, 32, "golf_%d_pos", ball_index);
                    CP_Vector pos = get_vector(root, ball_pos);
                    lvl->golf_balls[i + 0] = pos.x;
                    lvl->golf_balls[i + 1] = pos.y;

                    char ball_diameter[32];
                    sprintf_s(ball_diameter, 32, "golf_%d_diameter", ball_index);
                    lvl->golf_balls[i + 2] = get_float(root, ball_diameter, 1.f);
                }

                lvl->holes_size = get_int(root, "holes_amount", 1);
                lvl->holes = calloc((size_t)lvl->holes_size * BALL_HOLE_OFFSET, sizeof(float));

                int hole_index = 0;
                for (int i = 0; i < lvl->holes_size * BALL_HOLE_OFFSET; i += BALL_HOLE_OFFSET, ++hole_index) {
                    char hole_pos[32];
                    sprintf_s(hole_pos, 32, "hole_%d_pos", hole_index);
                    CP_Vector pos = get_vector(root, hole_pos);
                    lvl->holes[i + 0] = pos.x;
                    lvl->holes[i + 1] = pos.y;

                    char hole_diameter[32];
                    sprintf_s(hole_diameter, 32, "hole_%d_diameter", hole_index);
                    lvl->holes[i + 2] = get_float(root, hole_diameter, 1.5f);
                }
            }

            // some versions of the level file dont include the par, so make sure to check
            lvl->par = get_int(root, "par", 3);

            lvl->middle_wall_angle = get_float(root, "middle_wall_angle", 0.f);

            // set the walls position, scaling, angle and rotation rates
            // check if any of them exists first, if not use default values
            int wall_index = 0;
            for (int i = 0; i < lvl->walls_size * WALLS_OFFSET; i += WALLS_OFFSET, ++wall_index) {

                // set wall position if exists
                char wall_pos[32];
                sprintf_s(wall_pos, 32, "wall_%d_pos", wall_index);
                CP_Vector pos = get_vector(root, wall_pos);
                lvl->walls[i + 0] = pos.x;
                lvl->walls[i + 1] = pos.y;

                // set wall scale if exists
                char wall_scale[32];
                sprintf_s(wall_scale, 32, "wall_%d_scale", wall_index);
                CP_Vector scale = get_vector(root, wall_scale);
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
            lvl->items_size = get_int(root, "items_amount", 0);
            if (lvl->items_size) {
                lvl->items = calloc((size_t)lvl->items_size * ITEMS_OFFSET, sizeof(float));

                int item_index = 0;
                for (int i = 0; i < lvl->items_size * ITEMS_OFFSET; i += ITEMS_OFFSET, ++item_index) {
                    char item_pos[32];
                    sprintf_s(item_pos, 32, "item_%d_pos", item_index);
                    CP_Vector pos = get_vector(root, item_pos);
                    lvl->items[i + 0] = pos.x;
                    lvl->items[i + 1] = pos.y;

                    char item_diameter[32];
                    sprintf_s(item_diameter, 32, "item_%d_diameter", item_index);
                    lvl->items[i + 2] = get_float(root, item_diameter, 0.f);

                    char item_angle[32];
                    sprintf_s(item_angle, 32, "item_%d_angle", item_index);
                    lvl->items[i + 3] = get_float(root, item_angle, 0.f);

                    char item_type[32];
                    sprintf_s(item_type, 32, "item_%d_type", item_index);
                    lvl->items[i + 4] = (float) get_int(root, item_type, 0);
                }
            }
            else {
                lvl->items_size = 0;
                lvl->items = NULL;
            }

            // load terrains in level
            lvl->terrains_size = get_int(root, "terrains_amount", 0);
            if (lvl->terrains_size) {
                lvl->terrains = calloc((size_t)lvl->terrains_size * TERRAINS_OFFSET, sizeof(float));

                int terrain_index = 0;
                for (int i = 0; i < lvl->terrains_size * TERRAINS_OFFSET; i += TERRAINS_OFFSET, ++terrain_index) {
                    char terrain_pos[32];
                    sprintf_s(terrain_pos, 32, "terrain_%d_pos", terrain_index);
                    CP_Vector pos = get_vector(root, terrain_pos);
                    lvl->terrains[i + 0] = pos.x;
                    lvl->terrains[i + 1] = pos.y;

                    char terrain_scale[32];
                    sprintf_s(terrain_scale, 32, "terrain_%d_scale", terrain_index);
                    CP_Vector scale = get_vector(root, terrain_scale);
                    lvl->terrains[i + 2] = scale.x;
                    lvl->terrains[i + 3] = scale.y;

                    char terrain_angle[32];
                    sprintf_s(terrain_angle, 32, "terrain_%d_angle", terrain_index);
                    lvl->terrains[i + 4] = get_float(root, terrain_angle, 0.f);

                    char terrain_type[32];
                    sprintf_s(terrain_type, 32, "terrain_%d_type", terrain_index);
                    lvl->terrains[i + 5] = (float) get_int(root, terrain_type, 0);
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
    // save the world limits, golf balls, holes and middle wall first
    char level_path[128];
    sprintf_s(level_path, 128, LEVEL_FILEPATH, level);
    FILE* fp = fopen(level_path, "w");

    fprintf(fp,
        "width=%f\nheight=%f\nmiddle_wall_angle=%f",
        world_limits.x, world_limits.y,
        ((Wall*)walls->arr)->angle
    );

    int ball_index = 0;
    for (int i = 0; i < golf_balls->length; ++i) {
        GolfBall* ball = (GolfBall*)golf_balls->arr + i;
        if (ball == NULL || ball->destroyed || ball->diameter <= 0.f)
            continue;

        fprintf(fp,
            "\ngolf_%d_pos=%f,%f\ngolf_%d_diameter=%f",
            ball_index, ball->pos.x, ball->pos.y,
            ball_index, ball->diameter
        );
        ++ball_index;
    }

    // save the amount of golf balls saved
    fprintf(fp, "\ngolf_amount=%d", ball_index);

    int hole_index = 0;
    for (int i = 0; i < holes->length; ++i) {
        Hole* hole = (Hole*)holes->arr + i;
        if (hole == NULL || hole->destroyed || hole->diameter <= 0.f)
            continue;

        fprintf(fp,
            "\nhole_%d_pos=%f,%f\nhole_%d_diameter=%f",
            hole_index, hole->pos.x, hole->pos.y,
            hole_index, hole->diameter
        );
        ++hole_index;
    }

    // save the amount of holes saved
    fprintf(fp, "\nholes_amount=%d", hole_index);

    // save the walls now
    int wall_index = 0;
    for (int i = 1; i < walls->length; ++i) {
        Wall* wall = (Wall*)walls->arr + i;
        if (wall == NULL || wall->destroyed || wall->scale.x <= 0.f || wall->scale.y <= 0.f)
            continue;

        fprintf(fp,
            "\nwall_%d_pos=%f,%f\nwall_%d_scale=%f,%f\nwall_%d_angle=%f\nwall_%d_rot_rate=%f",
            wall_index, wall->pos.x, wall->pos.y,
            wall_index, wall->scale.x, wall->scale.y,
            wall_index, wall->angle,
            wall_index, wall->rotation_rate
        );
        ++wall_index;
    }

    // save the amount of walls saved
    fprintf(fp, "\nwalls_amount=%d", wall_index);

    int item_index = 0;
    for (int i = 0; i < items->length; ++i) {
        Item* item = (Item*)items->arr + i;
        if (item == NULL || item->destroyed || item->diameter <= 0.f)
            continue;

        fprintf(fp,
            "\nitem_%d_pos=%f,%f\nitem_%d_diameter=%f\nitem_%d_angle=%f\nitem_%d_type=%d",
            item_index, item->pos.x, item->pos.y,
            item_index, item->diameter,
            item_index, item->angle,
            item_index, item->type
        );
        ++item_index;
    }

    fprintf(fp, "\nitems_amount=%d", item_index);

    int terrain_index = 0;
    for (int i = 0; i < terrains->length; ++i) {
        Terrain* terrain = (Terrain*)terrains->arr + i;
        if (terrain == NULL || terrain->destroyed || terrain->scale.x <= 0.f)
            continue;

        fprintf(fp,
            "\nterrain_%d_pos=%f,%f\nterrain_%d_scale=%f,%f\nterrain_%d_angle=%f\nterrain_%d_type=%d",
            terrain_index, terrain->pos.x, terrain->pos.y,
            terrain_index, terrain->scale.x, terrain->scale.y,
            terrain_index, terrain->angle,
            terrain_index, terrain->type
        );
        ++terrain_index;
    }

    fprintf(fp, "\nterrains_amount=%d", terrain_index);
    fclose(fp);
    fp = NULL;
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
    if (level->golf_balls) {
        free(level->golf_balls);
        level->golf_balls = NULL;
    }
    if (level->holes) {
        free(level->holes);
        level->holes = NULL;
    }
    free(level);
    level = NULL;
}
