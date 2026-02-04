#include "texture.h"

#include <immintrin.h>

#include "logging.h"
#include "mtassert.h"
#include "types.h"
#include "utility/mtstring.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

mt_texture *mt_texture_create(uint width, uint height, enum mt_texture_format format)
{
    uint stride = 0;
    switch (format)
    {
    case MT_TEXTURE_RGBA8888:
        stride = 4;
        break;
    case MT_TEXTURE_FLOAT32:
        stride = 4;
        break;
    default:
        LERROR("Unknown texture format '%d'", format);
        return false;
    }

    size_t pitch = width * stride;
    mt_texture *texture = _mm_malloc(sizeof(mt_texture) + pitch * height, 32);
    MT_ASSERT(texture != NULL, "Could not allocate memory for texture");

    texture->pitch = pitch;
    texture->stride = stride;
    texture->width = width;
    texture->height = height;

    return texture;
}

void mt_texture_free(mt_texture *texture)
{
    if (texture)
    {
        _mm_free(texture);
    }
}

mt_texture *mt_texture_load(mt_string_view path)
{
    int width, height, format;
    stbi_uc *data = stbi_load(path.str, &width, &height, &format, 0);
    if (!data)
    {
        LERROR("Failed to load texture '%s'", path.str);
        return false;
    }

    MT_ASSERT(format == STBI_rgb_alpha, "Non-supported texture format");

    uint stride = 0;
    switch (format)
    {
    case STBI_grey:
        stride = 1;
        break;
    case STBI_grey_alpha:
        stride = 2;
        break;
    case STBI_rgb:
        stride = 3;
        break;
    case STBI_rgb_alpha:
        stride = 4;
        break;
    default:
        MT_UNREACHABLE;
    }

    size_t pitch = width * stride;
    size_t byte_count = pitch * height;

    mt_texture *texture = _mm_malloc(sizeof(mt_texture) + byte_count, 32);
    MT_ASSERT(texture != NULL, "Could not allocate memory for texture");

    memcpy(texture->data, data, byte_count);
    texture->width = width;
    texture->height = height;
    texture->pitch = pitch;
    texture->stride = stride;

    return texture;
}

void mt_texture_clear(mt_texture *texture, uint clear)
{
    uint *data = (uint *)texture->data;
    for (size_t i = 0; i < (size_t)texture->width * (size_t)texture->height; ++i)
    {
        data[i] = clear;
    }
}
