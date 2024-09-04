#include "world/objects.h"

/*
    * Small map size for now
    * Increase MAP_SIZE for more object spread
    * Keep MIN_SPAWN_RADIUS less than MAP_SIZE
*/
#define MAP_SIZE 25
#define MIN_SPAWN_RADIUS 20
#define DISTANCE_THRESHOLD 8.0
#define OBJECT_COUNT 4
#define OBJECT_Y 2.0f
#define FOUND_TIME 2.0f

// Struct for hidden objects
typedef struct {
    int id;
    Vector3 position;
    bool isFound;
    float foundTime;
} Object;

// Struct for markers
typedef struct {
    bool isVisible;
} Marker;

Object objects[OBJECT_COUNT];
Marker markers[OBJECT_COUNT];
bool allObjectsFound = false;

static float markerPositionY = 0.0f;
static float markerRotationY = 0.0f;




Model ObjectModel(Shader lightShader) {
    // 3D cube for now
    Mesh cube = GenMeshCube(2.0f, 2.0f, 2.0f);
    Model object = LoadModelFromMesh(cube);
    object.materials[0].shader = lightShader;

    return object;
}

Model MarkerModel() {
    Mesh body = GenMeshCube(0.3f, 0.3f, 0.3f);
    Model marker = LoadModelFromMesh(body);

    return marker;
}

// Initialize hidden objects at random positions in 4 quadrants
void InitObjects() {
    float mapQuadrant = MAP_SIZE / 2.0f;
    float minQuadrant = MIN_SPAWN_RADIUS / 2.0f;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        float offsetX = (i & 2) ? mapQuadrant : -mapQuadrant;
        float offsetZ = (i & 1) ? mapQuadrant : -mapQuadrant;

        float x = offsetX + (float)GetRandomValue(minQuadrant, mapQuadrant - minQuadrant);
        float z = offsetZ + (float)GetRandomValue(minQuadrant, mapQuadrant - minQuadrant);

        objects[i] = (Object){
            .id = i,
            .position = (Vector3){ x, OBJECT_Y, z },
            .isFound = false,
            .foundTime = 0.0f
        };

        markers[i].isVisible = false;
    }
}

/*
    * Update the object's marker when found
    * Rotate and oscillate the marker
*/
void ObjectFound(Object *obj) {
    float time = GetTime();
    float rotationSpeed = 0.5f;
    float height = 6.0f;
    float oscillationSpeed = 0.5f;

    markerPositionY = sinf(time * 0.5f) * oscillationSpeed + height;
    markerRotationY = fmodf(markerRotationY + rotationSpeed, 360.0f);

    if(GetTime() - obj->foundTime > FOUND_TIME){
        markers[obj->id].isVisible = true;
    }
}

// Update hidden objects and check if they are within the distance threshold
void UpdateObjects(Camera *camera) {
    allObjectsFound = true;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        Object *obj = &objects[i];

        float distance = Vector3Distance(camera->position, obj->position);
        if (distance < DISTANCE_THRESHOLD && !obj->isFound) {
            obj->isFound = true;
            obj->foundTime = (float)GetTime();
        }

        if (obj->isFound){
            ObjectFound(obj);
        } else {
            allObjectsFound = false;
        }
    }
}

void DrawObjects(Model object, Camera *camera) {
    UpdateObjects(camera);

    for (int i = 0; i < OBJECT_COUNT; i++) {
            DrawModelEx(object,
                        objects[i].position,
                        (Vector3){ 0.0f, 1.0f, 0.0f },
                        0.0f,
                        Vector3One(),
                        WHITE);
    }
}

void DrawMarkers(Model marker) {
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (markers[i].isVisible) {
            DrawModelEx(marker,
                        (Vector3){ objects[i].position.x,markerPositionY, objects[i].position.z },
                        (Vector3){ 0.0f, 1.0f, 0.0f },
                        markerRotationY,
                        Vector3One(),
                        WHITE);
        }
    }
}