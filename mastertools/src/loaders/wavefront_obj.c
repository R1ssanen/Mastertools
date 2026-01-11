#include "loaders/wavefront_obj.h"

#include <stdlib.h>

#include "scene/entity.h"
#include "utility/array.h"
#include "utility/file.h"
#include "utility/mstring.h"

static inline bool matches(char c, const char *str)
{
    for (size_t i = 0; i < strlen(str); ++i)
    {
        if (str[i] == c)
        {
            return true;
        }
    }

    return false;
}

static char *trim_lead(char *str, const char *whitespace)
{
    if (!str)
    {
        return NULL;
    }

    for (size_t i = 0; i < strlen(str); ++i)
    {
        if (!matches(str[i], whitespace))
        {
            return str + i;
        }
    }

    return NULL;
}

static char *trim_tail(char *str, const char *whitespace)
{
    if (!str)
    {
        return NULL;
    }

    if (!matches(str[0], whitespace))
    {
        return str;
    }

    for (size_t i = strlen(str) - 1; i > 0; --i)
    {
        if (!matches(str[i], whitespace))
        {
            str[i + 1] = '\0';
            return str;
        }
    }

    return NULL;
}

inline static char *trim(char *str, const char *whitespace)
{
    str = trim_lead(str, whitespace);
    return trim_tail(str, whitespace);
}

static const size_t END_TOK = (size_t)-1;

typedef struct tokenizer tokenizer;
struct tokenizer
{
    char *src;
    size_t offset;
    char delimiter;
};

static inline char *next(tokenizer *tokenizer)
{
    size_t start = tokenizer->offset;
    if (start == END_TOK)
    {
        return NULL;
    }

    for (size_t i = start; tokenizer->src[i] != '\0'; ++i)
    {
        if (tokenizer->src[i] == tokenizer->delimiter)
        {
            tokenizer->src[i] = '\0';
            tokenizer->offset = i + 1;
            return tokenizer->src + start;
        }
    }

    tokenizer->offset = END_TOK;
    if (tokenizer->src[start] != '\0')
    {
        return tokenizer->src + start;
    }
    else
    {
        return NULL;
    }
}

mt_entity *mt_load_wavefront_obj(mt_string_view path)
{
    char *buffer;
    size_t len;
    if (!mt_file_read(path, (byte **)(&buffer), &len))
    {
        return NULL;
    }

    mt_array vertices = mt_array_create(sizeof(float));
    mt_array indices = mt_array_create(sizeof(int));

    tokenizer line_tok = {.src = buffer, .offset = 0, .delimiter = '\n'};
    char *line;
    while ((line = next(&line_tok)))
    {
        line = trim_lead(line, " \t\v\f\r");

        if ((line[0] == 'v') && (line[1] == ' '))
        {
            tokenizer vertex_tok = {.src = trim_lead(line, " \t\v\f\rv"), .offset = 0, .delimiter = ' '};
            char *vertex;
            while ((vertex = next(&vertex_tok)))
            {
                float coord = strtof(vertex, NULL);
                mt_array_push(&vertices, &coord);
            }

            // vertices[vertex_count++] = 1.f; // w
            // vertices[vertex_count++] = 0.f; // u
            // vertices[vertex_count++] = 0.f; // v
            // vertices[vertex_count++] = 0.f; // nx
            // vertices[vertex_count++] = 0.f; // ny
            // vertices[vertex_count++] = 0.f; // nz
        }

        else if (line[0] == 'f')
        {
            tokenizer vertex_tok = {.src = trim_lead(line, " \t\v\f\rf"), .offset = 0, .delimiter = ' '};
            char *vertex;
            while ((vertex = next(&vertex_tok)))
            {
                tokenizer index_tok = {.src = trim_lead(vertex, " \t\v\f\r"), .offset = 0, .delimiter = '/'};
                char *index;
                // while ((index = next(&index_tok)))
                // {
                //     puts(index);
                // }
                int id = strtoul(next(&index_tok), NULL, 10);
                mt_array_push(&indices, &id);
            }
        }
    }

    free(buffer);

    printf("vertex count: %zu\n", vertices.size / 3);
    printf("triangle count: %zu\n", indices.size / 3);

    mt_entity *entity = malloc(sizeof(mt_entity));
    entity->meshes = mt_array_create(sizeof(mt_mesh));

    mt_mesh mesh = {
        .vertices = vertices,
        .indices = indices,
        .attribute_count = 1,
    };
    mt_array_push(&entity->meshes, &mesh);

    return entity;
}
