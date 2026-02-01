#ifndef MASTERTOOLS_ANALYTICS_H_
#define MASTERTOOLS_ANALYTICS_H_

#include <string.h>

#include "system/time.h"
#include "types.h"

#ifndef MT_FRAMETIME_SAMPLES
#define MT_FRAMETIME_SAMPLES 64
#endif

typedef struct mt_frame_analytics mt_frame_analytics;
struct mt_frame_analytics
{
    float samples[MT_FRAMETIME_SAMPLES];
    size_t head;
    struct mt_timer *timer;
    float average;
    float max, min;
};

static inline mt_frame_analytics mt_analytics_create(mt_timer *timer)
{
    mt_frame_analytics analytics;
    analytics.timer = timer;
    analytics.head = 0;
    analytics.average = 0.f;
    memset(analytics.samples, 0, MT_FRAMETIME_SAMPLES * sizeof(float));
    return analytics;
}

double mt_analytics_get_average_frametime(mt_frame_analytics *analytics)
{
    float new_sample = (float)mt_timer_get_elapsed(analytics->timer);
    if (new_sample < 10000.f)
    {
        analytics->samples[analytics->head] = new_sample;
    }
    analytics->head = (analytics->head + 1) & (MT_FRAMETIME_SAMPLES - 1);

    float sum = 0;
    analytics->max = 0.f;
    analytics->min = INFINITY;

    for (size_t i = 0; i < MT_FRAMETIME_SAMPLES; ++i)
    {
        float sample = analytics->samples[i];
        if (sample < analytics->min)
        {
            analytics->min = sample;
        }
        if (sample > analytics->max)
        {
            analytics->max = sample;
        }

        sum += sample;
    }

    analytics->average = sum / (float)MT_FRAMETIME_SAMPLES;
    return analytics->average;
}

void render_line(int *pixels, int width, int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    for (;;)
    {
        pixels[y0 * width + x0] = color;

        int e2 = 2 * error;
        if (e2 >= dy)
        {
            if (x0 == x1)
            {
                break;
            }
            else
            {
                error = error + dy;
                x0 = x0 + sx;
            }
        }
        if (e2 <= dx)
        {
            if (y0 == y1)
            {
                break;
            }
            else
            {
                error = error + dx;
                y0 = y0 + sy;
            }
        }
    }
}

#include "math/common.h"

static inline void mt_analytics_render_frame_graph(mt_frame_analytics *analytics, int *pixels, int width, int x, int y,
                                                   int w, int h)
{
    float inv_analytics_range = 1.f / (analytics->max - analytics->min);
    int y_a = y + h * MT_MAX(1.f - (analytics->average - analytics->min) * inv_analytics_range, 0.f);
    render_line(pixels, width, x, y_a, x + w, y_a, 0xff777777);

    float x_step = w / (float)(MT_FRAMETIME_SAMPLES - 1);

    for (size_t i = 0; i < MT_FRAMETIME_SAMPLES - 1; ++i)
    {
        int x0 = x + i * x_step;
        int x1 = x + (i + 1) * x_step;
        int y0 = y + h * MT_MAX(1.f - (analytics->samples[i] - analytics->min) * inv_analytics_range, 0.f);
        int y1 = y + h * MT_MAX(1.f - (analytics->samples[i + 1] - analytics->min) * inv_analytics_range, 0.f);

        render_line(pixels, width, x0, y0, x1, y1, 0xff00ff00);
    }
}

#endif