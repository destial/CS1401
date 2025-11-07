/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 22 September 2025
@course CSD1401
@brief Definition file for an asset manager that will handle loading and freeing of assets
*/

#include <stdlib.h>
#include <stdio.h>
#include "asset_manager.h"
#include "utils.h"

typedef enum AssetType {
    ASSET_TYPELESS = 0,
    ASSET_IMAGE,
    ASSET_FONT,
    ASSET_AUDIO,
    ASSET_SPRITE,
    ASSET_LAST
} AssetType;

typedef struct AssetStruct { 
    char* key;
    // each pointer holds 8 bytes, so we can just use union "polymorphism"
    // to minimize the size of the struct
    union {
        CP_Image image;
        CP_Font font;
        CP_Sound audio;
        SpriteSheet* sprite;
        void* value; // this is here for funsies, in case we want to store anything else
    };
    Asset* next;
    AssetType type;
} Asset;

Asset* root_assets = NULL;

void Asset_Preload(void) {
    // only the item assets have a shadow image
    // and its very costly to create them, so we preload
    CP_Color shadow = CP_Color_Create(0, 0, 0, 255);
    Asset_GetRecolorImage(BOOST_ITEM_ASSET, shadow, RECOLOR_SET);
    Asset_GetRecolorImage(BIG_ITEM_ASSET, shadow, RECOLOR_SET);
    Asset_GetRecolorImage(SMALL_ITEM_ASSET, shadow, RECOLOR_SET);
    Asset_GetRecolorImage(REVERSE_ITEM_ASSET, shadow, RECOLOR_SET);
}

static CP_Image set_node_image(Asset* node, CP_Image image, const char* filepath) {
    // load the image into memory and check if it exists
    node->image = image;
    if (node->image == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return NULL;
    }

    size_t l = strlen(filepath) + 1; // +1 for '\0' ending char
    // allocate the memory address for the key
    node->key = gc_malloc(l);
    if (node->key) {
        strcpy_s(node->key, l, filepath);
    }
    else {
        node->key = filepath;
    }

    node->type = ASSET_IMAGE;
    node->next = NULL;
    return node->image;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load an image into the asset map
@param node - The map node to load the image into
@param filepath - The filepath of the image file
*/
static CP_Image load_image(Asset* node, const char* filepath) {
    // load the image into memory and check if it exists
    node->image = CP_Image_Load(filepath);
    if (node->image == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return NULL;
    }

    size_t l = strlen(filepath) + 1; // +1 for '\0' ending char
    // allocate the memory address for the key
    node->key = gc_malloc(l);
    if (node->key) {
        strcpy_s(node->key, l, filepath);
    }
    else {
        node->key = filepath;
    }

    node->type = ASSET_IMAGE;
    node->next = NULL;
    return node->image;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load a font into the asset map
@param node - The map node to load the font into
@param filepath - The filepath of the font file
*/
static CP_Font load_font(Asset* node, const char* filepath) {
    // load the font into memory and check if it exists
    node->font = CP_Font_Load(filepath);
    if (node->font == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return NULL;
    }

    size_t l = strlen(filepath) + 1; // +1 for '\0' ending char
    // allocate the memory address for the key
    node->key = gc_malloc(l);
    if (node->key) {
        strcpy_s(node->key, l, filepath);
    }
    else {
        node->key = filepath;
    }
    
    node->type = ASSET_FONT;
    node->next = NULL;
    return node->font;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load an audio into the asset map
@param node - The map node to load the audio into
@param filepath - The filepath of the audio file
*/
static CP_Sound load_audio(Asset* node, const char* filepath) {
    // load the font into memory and check if it exists
    node->audio = CP_Sound_Load(filepath);
    if (node->audio == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return NULL;
    }

    size_t l = strlen(filepath) + 1; // +1 for '\0' ending char
    // allocate the memory address for the key
    node->key = gc_malloc(l);
    if (node->key) {
        strcpy_s(node->key, l, filepath);
    }
    else {
        node->key = filepath;
    }

    node->type = ASSET_AUDIO;
    node->next = NULL;
    return node->audio;
}

static Asset* get_node(const char* key) {
    for (Asset* ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == NULL)
            continue;

        if (strcmp(ptr->key, key) == 0)
            return (Asset*)ptr;
    }
    return NULL;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the assets loaded, also recursively goes through each next pointer
@param map - The root asset map
*/
static void free_asset(Asset* map) {
    if (map == NULL)
        return;

    // traverse to the end of the map
    if (map->next) {
        free_asset(map->next);
        map->next = NULL;
    }

    // free up the loaded assets respectively
    if (map->type == ASSET_IMAGE && map->image != NULL) {
        CP_Image_Free(map->image);
        map->image = NULL;
    }
    else if (map->type == ASSET_FONT && map->font != NULL) {
        CP_Font_Free(map->font);
        map->font = NULL;
    }
    else if (map->type == ASSET_AUDIO && map->audio != NULL) {
        CP_Sound_Free(map->audio);
        map->audio = NULL;
    }
    else if (map->type == ASSET_SPRITE && map->sprite != NULL) {
        CP_Image_Free(map->sprite->root_image);
        map->sprite->root_image = NULL;
        FREE_MEMORY(map->sprite);
    }
    else if (map->type == ASSET_TYPELESS && map->value != NULL) {
        FREE_MEMORY(map->value);
    }

    // free up the key memory address (we used malloc here)
    if (map->key) {
        printf("Freeing up %s\n", map->key);
        FREE_MEMORY(map->key);
    }

    FREE_MEMORY(map);
}

CP_Image Asset_GetImage(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(Asset));
        CP_Image image = load_image(root_assets, filepath);
        if (image == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
    }
    
    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset* ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_IMAGE) {
			return ptr->image;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(Asset));

            // ensure the image exists and can actually be loaded
            CP_Image image = load_image(ptr->next, filepath);
            if (image != NULL) {
                return image;
            }
            else { // filepath is wrong!
                FREE_MEMORY(ptr->next);
                return NULL;
            }
        }
    }
    return NULL;
}

CP_Image Asset_GetRecolorImage(const char* filepath, CP_Color color, AssetRecolorMethod method) {
    char colored_path[128];
    sprintf_s(colored_path, 128, "%s//%02x%02x%02x%02x", filepath, (int)color.r, (int)color.g, (int)color.b, (int)color.a);
    CP_Image root = Asset_GetImage(colored_path); // get the already created recolored image if exists
    if (root != NULL)
        return root;

    // get the original image to recolor
    root = Asset_GetImage(filepath);
    if (root == NULL)
        return NULL;

    // get dimensions of the image
    int w = CP_Image_GetWidth(root);
    int h = CP_Image_GetHeight(root);
    int arraySize = w * h;

    // create the array using the image size and the size of a CP_Color
    CP_Color* colorArray = gc_calloc(arraySize, sizeof(CP_Color));

    // load the pixel data into the color array
    CP_Image_GetPixelData(root, colorArray);
    for (int i = 0; i < arraySize; ++i) {
        switch (method) {
            case RECOLOR_MULTIPLY:
                if (colorArray[i].a > 0) { // multiply any non-transparent pixel color
                    colorArray[i].r = (unsigned char) (((colorArray[i].r / 255.f) * (color.r / 255.f)) * 255.f);
                    colorArray[i].g = (unsigned char) (((colorArray[i].g / 255.f) * (color.g / 255.f)) * 255.f);
                    colorArray[i].b = (unsigned char) (((colorArray[i].b / 255.f) * (color.b / 255.f)) * 255.f);
                    colorArray[i].a = (unsigned char) (((colorArray[i].a / 255.f) * (color.a / 255.f)) * 255.f);
                }
                break;
            case RECOLOR_ADD: // add any non-transparent pixel colors together
                if (colorArray[i].a > 0) {
                    colorArray[i].r += color.r;
                    colorArray[i].g += color.g;
                    colorArray[i].b += color.b;
                    colorArray[i].a += color.a;
                }
                break;
            case RECOLOR_SET:
                if (colorArray[i].a > 0) { // set any non-transparent pixel color
                    colorArray[i] = color;
                }
                break;
            default: break;
        }
    }

    // create a shadow image from the color array
    CP_Image recolored_image = CP_Image_CreateFromData(w, h, (unsigned char*)colorArray);
    gc_free(colorArray); // after creating this image, we can free up the color array buffer
    for (Asset* ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(Asset));
            // load this image into this new node
            set_node_image(ptr->next, recolored_image, colored_path);
            break;
        }
    }
    return recolored_image;
}

CP_Font Asset_GetFont(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(Asset));
        CP_Font font = load_font(root_assets, filepath);
        if (font == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
        return font;
    }

    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset* ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_FONT) {
            return ptr->font;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(Asset));

            // ensure the font exists and can actually be loaded
            CP_Font font = load_font(ptr->next, filepath);
            if (font != NULL) {
                return font;
            }
            else { // filepath is wrong!
                FREE_MEMORY(ptr->next);
                return NULL;
            }
        }
    }
    return NULL;
}

CP_Sound Asset_GetAudio(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(Asset));
        CP_Sound audio = load_audio(root_assets, filepath);
        if (audio == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
        return audio;
    }

    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset* ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_AUDIO) {
            return ptr->audio;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(Asset));

            // ensure the audio file exists and can actually be loaded
            CP_Sound audio = load_audio(ptr->next, filepath);
            if (audio != NULL) {
                return audio;
            }
            else { // filepath is wrong!
                FREE_MEMORY(ptr->next);
                return NULL;
            }
        }
    }
    return NULL;
}

SpriteSheet* Asset_GetSprite(const char* filepath, int rows, int columns, float duration) {
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(Asset));
        if (root_assets == NULL) 
            return NULL;
        
        root_assets->key = NULL;
        root_assets->sprite = NULL;
        root_assets->next = NULL;
    }

	// check if this sprite sheet already exists
    Asset* node = get_node(filepath);
    if (node != NULL && node->type == ASSET_SPRITE) {
        return node->sprite;
    }

	// the current filepath is not a sprite yet, so check if it's an image first
	// then we convert it to a sprite sheet
    if (node != NULL && node->type == ASSET_IMAGE) {
        SpriteSheet* sheet = gc_malloc(sizeof(SpriteSheet));
        if (sheet == NULL) 
            return NULL;
        
        // initialize the sprite sheet values
        sheet->root_image = node->image;
        sheet->current_row = 0;
        sheet->current_column = 0;
        sheet->rows = rows;
        sheet->columns = columns;
        sheet->duration = duration;
        sheet->elapsedDuration = 0.f;

		sheet->img_h = CP_Image_GetHeight(sheet->root_image);
		sheet->img_w = CP_Image_GetWidth(sheet->root_image);
		sheet->sub_h = sheet->img_h / rows;
		sheet->sub_w = sheet->img_w / columns;

        node->sprite = sheet;
        node->type = ASSET_SPRITE;
        return sheet;
    }

    // its not loaded yet, so we load it as an image if it exists
	// then the next call will convert it to a sprite sheet
    CP_Image image = Asset_GetImage(filepath);
    if (image)
        return Asset_GetSprite(filepath, rows, columns, duration);

    return NULL;
}

void Assets_Free(void) {
    if (root_assets == NULL) 
        return;

    // recursive freeing
    free_asset(root_assets);
    root_assets = NULL;
    printf("Freeing up all assets\n");
}
