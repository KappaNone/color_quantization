#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "time.h"

#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    Color *items;
    size_t count;
    size_t capacity;
} Colors;

#define K 12
static Colors clusters[K] = {0};
static Color means[K] = {0};

void generate_means(Color *means) {
    for (int i = 0; i < K; i++) {
        Color mean = {
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            GetRandomValue(0, 255),
            255
        };
        means[i] = mean;
    }
}

size_t find_closest_mean(Color color, Color means[]) {
    Vector3 point = {color.r, color.g, color.b};
    size_t closest_mean = 0;

    for (size_t k = 0; k < K-1; ++k) {
        Vector3 m0 = { means[closest_mean].r, means[closest_mean].g, means[closest_mean].b };
        Vector3 m1 = { means[k + 1].r, means[k + 1].g, means[k + 1].b };

        if (Vector3DistanceSqr(point, m0) > Vector3DistanceSqr(point, m1)) {
            closest_mean = k+1;
        }
    } 

    return closest_mean;
}

void recluster_pixels(Image image, Color means[], Colors *clusters) {
    Color *pixels = (Color *)image.data;

    for (size_t i = 0; i < K; ++i) {
        clusters[i].count = 0;
    }

    for (int i = 0; i < image.width*image.height; ++i) {
        Color color = pixels[i];
        size_t closest_mean = find_closest_mean(color, means);
        da_append(&clusters[closest_mean], color);
    }
}

Color get_centroid(Colors points) {
    if (points.count <= 0) return (Color){0, 0, 0, 0};

    float r_sum = 0;
    float g_sum = 0;
    float b_sum = 0;

    for (size_t i = 0; i < points.count; ++i) {
        r_sum += points.items[i].r;
        g_sum += points.items[i].g;
        b_sum += points.items[i].b;
    }

    return (Color) {r_sum/points.count, g_sum/points.count, b_sum/points.count, 255};
}

void update_means(Color means[], Colors clusters[]) {
    for (size_t i = 0; i < K; ++i) {
        means[i] = get_centroid(clusters[i]);
    }
}

void apply_means(Image *image, Color means[])
{
    Color *pixels = (Color *)image->data;
    int total = image->width * image->height;

    for (int i = 0; i < total; i++) {
        int closest = find_closest_mean(pixels[i], means);

        pixels[i] = means[closest];
    }
}

static inline unsigned char clamp_u8(int v)
{
    return (v < 0) ? 0 : (v > 255 ? 255 : v);
}

void add_noise(Image *img, float intensity, int amount)
{
    if (!img || !img->data) return;

    Color *pixels = (Color *)img->data;
    int total = img->width * img->height;

    for (int i = 0; i < total; i++)
    {
        if ((float)rand() / RAND_MAX < intensity)
        {
            int noise = (rand() % (amount * 2 + 1)) - amount;

            pixels[i].r = clamp_u8(pixels[i].r + noise);
            pixels[i].g = clamp_u8(pixels[i].g + noise);
            pixels[i].b = clamp_u8(pixels[i].b + noise);
        }
    }
}

void grayscale(Image *img)
{
    if (!img || !img->data) return;

    Color *pixels = (Color *)img->data;
    int total = img->width * img->height;

    for (int i = 0; i < total; i++)
    {
        unsigned char gray =
            (unsigned char)(
                pixels[i].r * 0.299f +
                pixels[i].g * 0.587f +
                pixels[i].b * 0.114f
            );

        pixels[i].r = gray;
        pixels[i].g = gray;
        pixels[i].b = gray;
    }
}

int main() {
    Image image = LoadImage("Ada_lovelace.png");

    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    generate_means(means);

    for (int i = 0; i < 20; i++) {
        recluster_pixels(image, means, clusters);
        update_means(means, clusters);
        clusters[i].count = 0;
    }

    apply_means(&image, means);

    ExportImage(image, "output.png");

    UnloadImage(image);

    return 0;
}

