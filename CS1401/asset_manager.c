/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 22 September 2025
@course CSD1401
@brief Definition file for an asset manager that will handle loading and freeing of assets
*/

#include "asset_manager.h"
#include <stdlib.h>

typedef enum AssetType {
    IMAGE = 1,
    FONT
} AssetType;

typedef struct AssetMap {
    const char* key;
    void* value;
    AssetType type;
    struct AssetMap* next;
} AssetMap;

AssetMap* root_assets;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load an image into the asset map
@param
node - The map node to load the image into
filepath - The filepath of the image file
*/
static CP_BOOL load_image(AssetMap* node, const char* filepath) {

    // load the image into memory and check if it exists
    node->value = CP_Image_Load(filepath);
    if (node->value == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return 0;
    }

    // allocate the memory address for the key
    node->key = malloc(strlen(filepath) + 1);
    if (node->key) {
        strcpy_s(node->key, strlen(filepath) + 1, filepath);
    }
    else {
        node->key = filepath;
    }

    node->type = IMAGE;
    node->next = NULL;
    return 1;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load a font into the asset map
@param
node - The map node to load the font into
filepath - The filepath of the font file
*/
static CP_BOOL load_font(AssetMap* node, const char* filepath) {

    // load the font into memory and check if it exists
    node->value = CP_Font_Load(filepath);
    if (node->value == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return 0;
    }

    // allocate the memory address for the key
    node->key = malloc(strlen(filepath) + 1);
    if (node->key) {
        strcpy_s(node->key, strlen(filepath) + 1, filepath);
    }
    else {
        node->key = filepath;
    }
    
    node->type = FONT;
    node->next = NULL;
    return 1;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the assets loaded, also recursively goes through each next pointer
@param
map - The root asset map
*/
static void free_assetmap(AssetMap* map) {
    if (map == NULL)
        return;

    // traverse to the end of the map
    if (map->next) {
        free_assetmap(map->next);
        map->next = NULL;
    }

    // free up the loaded assets respectively
    if (map->value != NULL) {
        if (map->type == IMAGE) {
            CP_Image_Free(map->value);
        }
        else if (map->type == FONT) {
            CP_Font_Free(map->value);
        }
        else { // some random use case if we have other assets to load (technically we can have any key/value pair now lol)
            free(map->value);
        }
        map->value = NULL;
    }

    // free up the key memory address (we used malloc here)
    if (map->key) {
        free(map->key);
        map->key = NULL;
    }

    free(map);
    map = NULL;
}

CP_Image* get_image(const char* filepath) {
    // if the root is empty, we initialize it
    if (!root_assets) {
        root_assets = malloc(sizeof(AssetMap));
        load_image(root_assets, filepath);
    }
    
    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    AssetMap* ptr;
    for (ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == IMAGE) {
            return (CP_Image*) ptr->value;
        }
        if (ptr->next == NULL) {
            ptr->next = malloc(sizeof(AssetMap));

            // ensure the image exists and can actually be loaded
            if (load_image(ptr->next, filepath)) {
                return get_image(filepath);
            }
            else { // filepath is wrong!
                free(ptr->next);
                ptr->next = NULL;
                return NULL;
            }
        }
    }
    return NULL;
}

CP_Font* get_font(const char* filepath) {
    // if the root is empty, we initialize it
    if (!root_assets) {
        root_assets = malloc(sizeof(AssetMap));
    }

    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    AssetMap* ptr;
    for (ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == FONT) {
            return (CP_Font*) ptr->value;
        }
        if (ptr->next == NULL) {
            ptr->next = malloc(sizeof(AssetMap));

            // ensure the font exists and can actually be loaded
            if (load_font(ptr->next, filepath)) {
                return get_font(filepath);
            }
            else { // filepath is wrong!
                free(ptr->next);
                ptr->next = NULL;
                return NULL;
            }
        }
    }
    return NULL;
}

void free_assets(void) {
    if (root_assets) {
        free_assetmap(root_assets);
        root_assets = NULL;
    }
}