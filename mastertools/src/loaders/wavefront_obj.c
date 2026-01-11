#include "loaders/wavefront_obj.h"

#include <stdlib.h>

#include "scene/entity.h"
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
    // char test[] = "ab/b/c d/eg/f g/h/ia";
    // size_t offs = 0;
    // char *test_indices = split_by_delim(test, &offs, ' ', '\0');
    // for (; test_indices != NULL; test_indices = split_by_delim(test, &offs, ' ', '\0'))
    // {
    //     size_t offs_2 = 0;
    //     char *test_index = split_by_delim(test_indices, &offs_2, '/', '\0');
    //     for (; test_index != NULL; test_index = split_by_delim(test_indices, &offs_2, '/', '\0'))
    //     {
    //         printf("%s ", test_index);
    //     }
    //     putchar('\n');
    // }
    // return NULL;

    // char test[] = "a//";
    // tokenizer test_tok = {.src = test, .offset = 0, .delimiter = '/'};
    // char *t;
    // while ((t = next(&test_tok)))
    // {
    //     puts(t);
    // }
    // return NULL;

    char *buffer;
    size_t len;
    if (!mt_file_read(path, (byte **)(&buffer), &len))
    {
        return NULL;
    }

    // float vertices[1024 * 20 * 3];
    float *vertices = malloc(1024 * 20 * 3 * sizeof(float));
    size_t vertex_count = 0;

    // int indices[1024 * 20];
    int *indices = malloc(1024 * 20 * sizeof(int));
    size_t index_count = 0;

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
                vertices[vertex_count++] = strtof(vertex, NULL);
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
                indices[index_count++] = strtoul(next(&index_tok), NULL, 10);
            }
        }
    }

    printf("vertex count: %zu\n", vertex_count / 3);
    printf("triangle count: %zu\n", index_count / 3);

    free(buffer);

    mt_mesh mesh;
    mesh.attribute_count = 1;

    mesh.vertices = malloc(vertex_count * sizeof(float));
    memcpy(mesh.vertices, vertices, vertex_count * sizeof(float));
    mesh.vertex_count = vertex_count;

    mesh.indices = malloc(index_count * sizeof(uint));
    mesh.index_count = index_count;
    memcpy(mesh.indices, indices, index_count * sizeof(float));

    mt_entity *entity = malloc(sizeof(mt_entity));
    entity->meshes[0] = mesh;

    return entity;
}
