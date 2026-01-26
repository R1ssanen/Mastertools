#include "loaders/wavefront_obj.h"

#include <stdlib.h>

#include "logging.h"
#include "scene/entity.h"
#include "types.h"
#include "utility/array.h"
#include "utility/file.h"
#include "utility/mtstring.h"

static inline bool char_within_str(char c, const char *str)
{
    for (; *str != '\0'; ++str)
    {
        if (c == *str)
        {
            return true;
        }
    }

    return false;
}

static char *trim_leading(char *str, const char *whitespace)
{
    char *it = str;
    for (; *it != '\0'; ++it)
    {
        if (!char_within_str(*it, whitespace))
        {
            break;
        }
    }

    return it;
}

typedef struct tokenizer tokenizer;
struct tokenizer
{
    char *src;
    size_t offset;
    char delimiter;
    bool ended;
};

static inline char *next(tokenizer *tokenizer)
{
    if (tokenizer->ended)
    {
        return NULL;
    }

    size_t start = tokenizer->offset;
    for (size_t i = start; tokenizer->src[i] != '\0'; ++i)
    {
        if (tokenizer->src[i] == tokenizer->delimiter)
        {
            tokenizer->src[i] = '\0';
            tokenizer->offset = i + 1;
            return trim_leading(tokenizer->src + start, " \t\v\f\r");
        }
    }

    tokenizer->ended = true;
    return trim_leading(tokenizer->src + start, " \t\v\f\r");
}

struct mt_entity *mt_load_wavefront_obj(mt_string_view path)
{
    // char test_buf[] = "ab// ";
    // tokenizer tok = {.src = test_buf, .offset = 0, .delimiter = '/'};

    // char *res;
    // puts(next(&tok));
    // puts(next(&tok));
    // puts(next(&tok));

    // exit(0);

    char *buffer;
    size_t len;
    if (!mt_file_read(path, &buffer, &len))
    {
        return NULL;
    }

    mt_array_of(float) vertices = mt_array_create(sizeof(float));
    mt_array_of(int) indices = mt_array_create(sizeof(int));

    tokenizer line_tok = {.src = buffer, .offset = 0, .delimiter = '\n'};
    char *line;
    while ((line = next(&line_tok)))
    {
        line = trim_leading(line, " \t\v\f\r");
        // puts(line);
        // continue;

        if ((line[0] == 'v') && (line[1] == ' '))
        {
            tokenizer vertex_tok = {.src = trim_leading(line, " v\t\v\f\r"), .offset = 0, .delimiter = ' '};
            char *vertex;
            while ((vertex = next(&vertex_tok)))
            {
                float coord = strtof(vertex, NULL);
                mt_array_push(&vertices, &coord);
            }
        }

        else if (line[0] == 'f')
        {
            tokenizer vertex_tok = {.src = trim_leading(line, " f\t\v\f\r"), .offset = 0, .delimiter = ' '};
            char *vertex;
            while ((vertex = next(&vertex_tok)))
            {
                tokenizer index_tok = {.src = trim_leading(vertex, " \t\v\f\r"), .offset = 0, .delimiter = '/'};

                // for (char *it = vertex; *it != '\0'; ++it)
                //     printf("(%c) %d ", *it, (int)*it);
                // putchar('\n');

                char *next_id = next(&index_tok);
                // printf("%s ", next_id);

                int id = strtoul(next_id, NULL, 10) - 1;
                mt_array_push(&indices, &id);

                next_id = next(&index_tok);
                // printf("/ %s ", next_id);

                next_id = next(&index_tok);
                // printf("/ %s\n", next_id);
            }
        }
    }

    free(buffer);

    printf("vertex count: %zu\n", vertices.size / 3);
    printf("triangle count: %zu\n", indices.size / 3);

    struct mt_entity *entity = malloc(sizeof *entity);
    mt_mesh mesh = {
        .vertices = vertices,
        .indices = indices,
        .attribute_count = 1,
    };

    entity->meshes = mt_array_create(sizeof(mt_mesh));
    mt_array_push(&entity->meshes, &mesh);

    return entity;
}
