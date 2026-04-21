#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "time.h"

#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200
#define K 10

typedef struct {
    Vector2 *items;
    size_t count;
    size_t capacity;
    Color color;
} Points;

void generate_cluster(Points *points, int points_count, Vector2 center, int size) {
    for (int i = 0; i < points_count; ++i) {
        Vector2 point = {
            GetRandomValue(center.x-size/2, center.x+size/2), 
            GetRandomValue(center.y-size/2, center.y+size/2)
        };
        da_append(points, point);
    }
}

void generate_means(Vector2 *means) {
    for (int i = 0; i < K; i++) {
        Vector2 mean = {
            GetRandomValue(0, SCREEN_WIDTH),
            GetRandomValue(0, SCREEN_HEIGHT)
        };
        means[i] = mean;
    }
}

void recluster_state(Points points, Vector2 means[], Points *clusters) {
    for (size_t i = 0; i < K; ++i) {
        clusters[i].count = 0;
    }
    for (size_t i = 0; i < points.count; ++i) {
        Vector2 point = points.items[i];
        size_t closest_mean = 0;
        for (size_t k = 0; k < K-1; ++k) {
            if (Vector2DistanceSqr(point, means[closest_mean]) >
                    Vector2DistanceSqr(point, means[k+1])) {
                closest_mean = k+1;
            }
        } 
        da_append(&clusters[closest_mean], point);
    }
}

Vector2 get_centroid(Points points) {
    if (points.count <= 0) return Vector2Zero();
    float x_sum = 0;
    float y_sum = 0;
    for (size_t i = 0; i < points.count; ++i) {
        x_sum += points.items[i].x;
        y_sum += points.items[i].y;
    }
    return (Vector2) {x_sum/points.count, y_sum/points.count};
}

void update_means(Vector2 means[], Points clusters[]) {
    for (size_t i = 0; i < K; ++i) {
        means[i] = get_centroid(clusters[i]);
    }
}

static Points clusters[K] = {0};
static Vector2 means[K] = {0};
static Color colors[] = {
    GOLD,
    PINK,
    MAROON,
    LIME,
    SKYBLUE,
    VIOLET,
};
#define colors_count ARRAY_LEN(colors)

int main() {
    SetRandomSeed(time(NULL));
    Points points = {0};
    Vector2 center = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

    generate_cluster(&points, 2000, center, SCREEN_HEIGHT);
    generate_means(means);

    recluster_state(points, means, clusters);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "kmeans");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x141414FF));
        if (IsKeyPressed(KEY_R)) {
            points.count = 0;
            generate_cluster(&points, 2000, center, SCREEN_HEIGHT);
            generate_means(means);
            recluster_state(points, means, clusters);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            update_means(means, clusters);
            recluster_state(points, means, clusters);
        }
        for (size_t i = 0; i < K; ++i) {
            for (size_t k = 0; k < clusters[i].count; ++k) {
                DrawCircleV(clusters[i].items[k], 5, colors[i%colors_count]);
            }
            DrawCircleV(means[i], 10, WHITE);
        }
        EndDrawing();
    }
    return 0;
}
