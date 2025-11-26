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

typedef struct AssetStruct *Asset;

typedef struct AssetStruct { 
    char* key;
    // each pointer holds 8 bytes, so we can just use union "polymorphism"
    // to minimize the size of the struct
    union {
        CP_Image image;
        CP_Font font;
        CP_Sound audio;
        SpriteSheet sprite;
        void* value; // this is here for funsies, in case we want to store anything else
    };
    Asset next;
    AssetType type;
} *Asset;

static Asset root_assets = NULL;

void Asset_Preload(void) {
    // only the item assets have a shadow image
    // and its very costly to create them, so we preload
    CP_Color shadow = CP_Color_Create(0, 0, 0, 255);
    Asset_GetBlendedImage(BOOST_ITEM_ASSET, shadow, BLEND_SET);
    Asset_GetBlendedImage(BIG_ITEM_ASSET, shadow, BLEND_SET);
    Asset_GetBlendedImage(SMALL_ITEM_ASSET, shadow, BLEND_SET);
    Asset_GetBlendedImage(REVERSE_ITEM_ASSET, shadow, BLEND_SET);
}

static void* set_node(Asset node, AssetType type, void* value, const char* filepath) {
    // load the image into memory and check if it exists
    node->value = value;
    if (node->value == NULL) {
        node->key = NULL;
        node->next = NULL;
        node->type = 0;
        return NULL;
    }

    size_t l = strlen(filepath) + 1; // +1 for '\0' ending char
    // allocate the memory address for the key
    node->key = gc_calloc(l, sizeof(char));
    strcpy_s(node->key, l, filepath);

    node->type = type;
    node->next = NULL;
    return node->image;
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load an image into the asset map
@param node - The map node to load the image into
@param filepath - The filepath of the image file
*/
static CP_Image load_image(Asset node, const char* filepath) {
    return (CP_Image) set_node(node, ASSET_IMAGE, CP_Image_Load(filepath), filepath);
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load a font into the asset map
@param node - The map node to load the font into
@param filepath - The filepath of the font file
*/
static CP_Font load_font(Asset node, const char* filepath) {
    return (CP_Font) set_node(node, ASSET_FONT, CP_Font_Load(filepath), filepath);
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Load an audio into the asset map
@param node - The map node to load the audio into
@param filepath - The filepath of the audio file
*/
static CP_Sound load_audio(Asset node, const char* filepath) {
    return (CP_Sound) set_node(node, ASSET_AUDIO, CP_Sound_Load(filepath), filepath);
}

static Asset get_node(const char* key) {
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == NULL)
            continue;

        if (strcmp(ptr->key, key) == 0)
            return ptr;
    }
    return NULL;
}

void Asset_Free_Node(void* value) {
    if (value == NULL)
        return;

    if (root_assets->value == value) {
        // dont free this up! its a lazy workaround
        return;
    }

    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->next) {
            if (ptr->next->value == value) { // if match, we bridge the previous and the next one
                Asset todel = ptr->next;
                Asset after = todel->next;
                ptr->next = after;

                // ensure to free up the memory allocated for each node
                // free up the loaded assets respectively
                if (todel->type == ASSET_IMAGE && todel->image != NULL) {
                    CP_Image_Free(todel->image);
                    todel->image = NULL;
                }
                else if (todel->type == ASSET_FONT && todel->font != NULL) {
                    CP_Font_Free(todel->font);
                    todel->font = NULL;
                }
                else if (todel->type == ASSET_AUDIO && todel->audio != NULL) {
                    CP_Sound_Free(todel->audio);
                    todel->audio = NULL;
                }
                else if (todel->type == ASSET_SPRITE && todel->sprite != NULL) {
                    Sprite_Free(todel->sprite);
                    todel->sprite = NULL;
                }
                else if (todel->type == ASSET_TYPELESS && todel->value != NULL) {
                    FREE_MEMORY(todel->value);
                }
                printf("Deleting asset %s\n", todel->key);
                FREE_MEMORY(todel->key);
                FREE_MEMORY(todel);
                break;
            }
        }
    }
}

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the memory allocated for this map of the assets loaded, also recursively goes through each next pointer
@param map - The root asset map
*/
static void free_asset(Asset map) {
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
        Sprite_Free(map->sprite);
        map->sprite = NULL;
    }
    else if (map->type == ASSET_TYPELESS && map->value != NULL) {
        FREE_MEMORY(map->value);
    }

    // free up the key memory address (we used malloc here)
    printf("Freeing up asset %s\n", map->key);
    FREE_MEMORY(map->key);
    FREE_MEMORY(map);
}

CP_Image Asset_GetImage(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(struct AssetStruct));
        CP_Image image = load_image(root_assets, filepath);
        if (image == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
        return image;
    }
    
    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_IMAGE) {
			return ptr->image;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(struct AssetStruct));

            // ensure the image exists and can actually be loaded
            CP_Image image = load_image(ptr->next, filepath);
            if (image != NULL) {
                return image;
            }
            // filepath is wrong!
            FREE_MEMORY(ptr->next);
            return NULL;
        }
    }
    return NULL;
}

CP_Image Asset_GetBlendedImage(const char* filepath, CP_Color color, AssetBlendMode mode) {
    char colored_path[128];
    sprintf_s(colored_path, 128, "%s//%02x%02x%02x%02x%d", filepath, (int)color.r, (int)color.g, (int)color.b, (int)color.a, mode);
    CP_Image root = Asset_GetImage(colored_path); // get the already created blended image if exists
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
        switch (mode) {
            case BLEND_MULTIPLY:
                if (colorArray[i].a > 0) { // multiply any non-transparent pixel color
                    colorArray[i].r = (unsigned char) (((colorArray[i].r / 255.f) * (color.r / 255.f)) * 255.f);
                    colorArray[i].g = (unsigned char) (((colorArray[i].g / 255.f) * (color.g / 255.f)) * 255.f);
                    colorArray[i].b = (unsigned char) (((colorArray[i].b / 255.f) * (color.b / 255.f)) * 255.f);
                    colorArray[i].a = (unsigned char) (((colorArray[i].a / 255.f) * (color.a / 255.f)) * 255.f);
                }
                break;
            case BLEND_ADD: // add any non-transparent pixel colors together, not including alpha
                if (colorArray[i].a > 0) {
                    colorArray[i].r = min(255, (int)colorArray[i].r + (int)color.r);
                    colorArray[i].g = min(255, (int)colorArray[i].g + (int)color.g);
                    colorArray[i].b = min(255, (int)colorArray[i].b + (int)color.b);

                }
                break;
            case BLEND_ADD_ALPHA: // add any non-transparent pixel colors together
                if (colorArray[i].a > 0) {
                    colorArray[i].r = min(255, (int)colorArray[i].r + (int)color.r);
                    colorArray[i].g = min(255, (int)colorArray[i].g + (int)color.g);
                    colorArray[i].b = min(255, (int)colorArray[i].b + (int)color.b);
                    colorArray[i].a = min(255, (int)colorArray[i].a + (int)color.a);
                }
                break;
            case BLEND_SET:
                if (colorArray[i].a > 0) { // set any non-transparent pixel color
                    colorArray[i] = color;
                }
                break;
            default: break;
        }
    }

    // create a shadow image from the color array
    CP_Image recolored_image = CP_Image_CreateFromData(w, h, (unsigned char*)colorArray);
    FREE_MEMORY(colorArray); // after creating this image, we can free up the color array buffer
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(struct AssetStruct));
            // load this image into this new node
            set_node(ptr->next, ASSET_IMAGE, recolored_image, colored_path);
            break;
        }
    }
    return recolored_image;
}

CP_Font Asset_GetFont(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(struct AssetStruct));
        CP_Font font = load_font(root_assets, filepath);
        if (font == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
        return font;
    }

    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_FONT) {
            return ptr->font;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(struct AssetStruct));

            // ensure the font exists and can actually be loaded
            CP_Font font = load_font(ptr->next, filepath);
            if (font != NULL) {
                return font;
            }
            // filepath is wrong!
            FREE_MEMORY(ptr->next);
            return NULL;
        }
    }
    return NULL;
}

CP_Sound Asset_GetAudio(const char* filepath) {
    // if the root is empty, we initialize it
    if (root_assets == NULL) {
        root_assets = gc_malloc(sizeof(struct AssetStruct));
        CP_Sound audio = load_audio(root_assets, filepath);
        if (audio == NULL) {
            FREE_MEMORY(root_assets);
            return NULL;
        }
        return audio;
    }

    // traverse through the map nodes and find the key that matches this filepath (and asset type)
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, filepath) == 0 && ptr->type == ASSET_AUDIO) {
            return ptr->audio;
        }
        if (ptr->next == NULL) {
            ptr->next = gc_malloc(sizeof(struct AssetStruct));

            // ensure the audio file exists and can actually be loaded
            CP_Sound audio = load_audio(ptr->next, filepath);
            if (audio != NULL) {
                return audio;
            }
            // filepath is wrong!
            FREE_MEMORY(ptr->next);
            return NULL;
        }
    }
    return NULL;
}

SpriteSheet Asset_GetSprite(const char* filepath, int rows, int columns, float duration) {
    if (root_assets == NULL) {
        if (Asset_GetImage(filepath))
            return Asset_GetSprite(filepath, rows, columns, duration);

        return NULL;
    }

	// check if this sprite sheet already exists
    Asset node = get_node(filepath);
    if (node != NULL && node->type == ASSET_SPRITE) {
        return node->sprite;
    }

	// the current filepath is not a sprite yet, so check if it's an image first
	// then we convert it to a sprite sheet
    if (node != NULL && node->type == ASSET_IMAGE) {
        node->sprite = Sprite_Create(node->image, rows, columns, duration);
        node->type = ASSET_SPRITE;
        return node->sprite;
    }

    // its not loaded yet, so we load it as an image if it exists
	// then the next call will convert it to a sprite sheet
    CP_Image image = Asset_GetImage(filepath);
    if (image)
        return Asset_GetSprite(filepath, rows, columns, duration);

    return NULL;
}

void Asset_SpriteStartFrame(void) {
    for (Asset ptr = root_assets; ptr != NULL; ptr = ptr->next) {
        if (ptr->type == ASSET_SPRITE) {
            Sprite_ResetFrame(ptr->sprite);
        }
    }
}

void Assets_Free(void) {
    if (root_assets == NULL) 
        return;

    // recursive freeing
    free_asset(root_assets);
    root_assets = NULL;
    printf("Freed up all assets\n");
}
