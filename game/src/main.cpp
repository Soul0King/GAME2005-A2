#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <array>

struct PhysicsBody
{
    int id = -1;
    Vector2 position = Vector2Zeros;
    Vector2 velocity = Vector2Zeros;
};

struct PhysicsWorld
{
    Vector2 gravity = { 0.0f, 1.6f };
    std::vector<PhysicsBody> entities;
};

constexpr float PROJECTILE_RADIUS = 10.0f;
constexpr float LAUNCH_HEIGHT = 600.0f;


float LaunchAnglesArray[] = { 0, 3.19, 6.42, 9.736, 13.194, 16.874, 20.905,	25.529,	31.367,	45,	58.633,	64.471,	69.095,	73.126,	76.806,	80.264,	83.58, 86.81, 90 };

int main()
{
    // An example of using C file libraries + raylib (which also uses C files internally)!
    char* testData = new char[2048];
    int byteCount = 0;
    for (int i = 0; i < (sizeof(LaunchAnglesArray) / sizeof(LaunchAnglesArray[0])); i++)
    {
        int testNumber = i + 1;
        float testRange = (14400 * (sin(2 * (LaunchAnglesArray[i] * DEG2RAD)))) / 1.6;
        float testTime = (240 * (sin(LaunchAnglesArray[i] * DEG2RAD))) / 1.6;
        byteCount += sprintf(testData + byteCount, "Range %i: %f units, Time %i: %f seconds.\n", testNumber, testRange, testNumber, testTime);
    }

    const char* fileName = "test.txt";
    SaveFileText(fileName, testData);               // <-- Creates a new empty file (deletes previous file if there was one with the same name)
    delete[] testData;

    char* fileData = LoadFileText(fileName);        // <-- Loads a file as text data. File must exist otherwise an error is output to the console!
    TraceLog(LOG_INFO, "File data: %s", fileData);  // Output our test (C-style) string to the console!

    PhysicsWorld world;
    
    for (int i = 0; i < (sizeof(LaunchAnglesArray) / sizeof(LaunchAnglesArray[0])); i++)
    {
        float launchAngle = LaunchAnglesArray[i] * DEG2RAD;
        float launchSpeed = 120.0f;

        PhysicsBody entity{};
        entity.id = i;
        entity.position = { 0.0f, LAUNCH_HEIGHT - 1.0f };
        entity.velocity = Vector2Rotate(Vector2UnitX, -launchAngle) * launchSpeed;
        world.entities.push_back(entity);
    }

    InitWindow(800, 800, "Physics-1");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Motion loop
        for (size_t i = 0; i < world.entities.size(); i++)
        {
            PhysicsBody& e = world.entities[i];
            Vector2 acc = world.gravity;

            e.velocity += acc * dt;             // v = a * t
            e.position += e.velocity * dt;      // p = v * t
        }

        // Collision loop
        world.entities.erase(std::remove_if(world.entities.begin(), world.entities.end(),
            [](PhysicsBody& entity)
            {
                bool remove = entity.position.y >= LAUNCH_HEIGHT;
                if (remove)
                {
                    TraceLog(LOG_INFO, "Entity %i has hit the ground at %f.\n", entity.id, entity.position.x);
                    // entity.position.x is the projectile's range
                    // TODO -- Figure out how to keep track of each projectile's flight time!
                    // TODO -- Figure out how to export all desired data to a .txt or .csv file!
                }
                return remove;
            }
        ), world.entities.end());

        // Render loop
        BeginDrawing();
            ClearBackground(WHITE);
            for (const PhysicsBody& e : world.entities)
            {
                DrawCircleV(e.position, PROJECTILE_RADIUS, LIGHTGRAY);
            }
            DrawRectangle(0, LAUNCH_HEIGHT, GetScreenWidth(), 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
