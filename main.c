//Makeing a basic planet explorer game.

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"


#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

typedef struct Player{
    Vector3 pos;
    Vector3 front;
    Vector3 up;
    Vector3 right;

} Player;


Vector3 WorldVectorToPlayer(const Player *p1, Vector3 v1) {

    return (Vector3) {
    .x = Vector3DotProduct(v1, p1->right),
    .y = Vector3DotProduct(v1, p1->up),
    .z = Vector3DotProduct(v1, p1->front)
};
}

bool ENABLE_DEBUG = false;
bool ENABLE_FREECAM = false;

int main(int argc, char* argv[])
{
	printf("Hello, world!\n");

    // Argument Handling
    printf("Arguments: ");
    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
        if (strcmp(argv[i], "--debug") == 0) {
            ENABLE_DEBUG = true;
        }
        if (strcmp(argv[i], "--freecam") == 0) {
            ENABLE_FREECAM = true;

        }
    }
    printf("\n");

	// Window-specific setup
	const int screenWidth = 800;
	const int screenHeight = 450;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "raylib projeccs (GLFW!)");

    // Player-specific setup
    Camera3D camera = { 0 };
	camera.position = (Vector3){ 0.0f, 16.0f, 16.0f };
	camera.target = (Vector3){ 10.0f, 2.0f, 10.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

    struct Player player;
    player.pos = (Vector3){ 0.0f, 0.0f, 0.0f };
    player.front = (Vector3){ 0.0f, 0.0f, 1.0f };
    player.up = (Vector3){0.0f, 1.0f, 0.0f};
    player.right = Vector3CrossProduct(player.front, player.up);

    bool DEBUG_ON = false;
    bool FREECAM_ON = false;

	// World-specific config
    Vector3 orbitalPlanetPos = (Vector3){ 0.0f, 0.0f, 0.0f };

    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
	Model skybox = LoadModelFromMesh(cube);
	Image img = LoadImage("assets/StandardCubeMap.png");
	TextureCubemap cubemap = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);

	Shader ShaderCubemap = LoadShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
	skybox.materials[0].shader = ShaderCubemap;

	skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemap;
	UnloadImage(img);


	// Final touches
	DisableCursor();
	SetTargetFPS(60);

	// Do stuff while window is open, A.K.A game loop.
	while (!WindowShouldClose()) {
		// -----------------------
		// Update variables here
		// -----------------------
        if (IsKeyPressed(KEY_K) && ENABLE_FREECAM) FREECAM_ON = !FREECAM_ON;
        if (IsKeyPressed(KEY_G) && ENABLE_DEBUG) DEBUG_ON = !DEBUG_ON;

        if (FREECAM_ON) {
            UpdateCamera(&camera, CAMERA_FREE);
            camera.up = (Vector3){0.0f, 1.0f, 0.0f};
        } else {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        };

		orbitalPlanetPos = (Vector3){ sin(GetTime() * 0.25f) * 25.0f, 0.0f, cos(GetTime() * 0.25f) * 25.0f};

        // Player world height translation
		if (IsKeyDown(KEY_R)) {
            camera.target.y += 1.0f / 5.0f;
            camera.position.y += 1.0f / 5.0f;
        };
		if (IsKeyDown(KEY_F)) {
            camera.target.y -= 1.0f / 5.0f;
            camera.position.y -= 1.0f / 5.0f;
        };

        // Allow camera roll
        if (IsKeyDown(KEY_Q)) {
            CameraRoll(&camera, -0.0625 * PI);
              /* Vector3Normalize(Vector3CrossProduct(Tempright, camera.target)); */
        }
        if (IsKeyDown(KEY_E)) {
            CameraRoll(&camera, 0.0625 * PI);

        }

        if (!FREECAM_ON) {
            player.pos = Vector3Subtract(camera.target, (Vector3){ 0.0f, 0.0f, 0.0f});
            //camera.target = Vector3Add(player.pos, player.front);
            player.front = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
            player.up = Vector3Normalize(camera.up);
            player.right = Vector3Normalize(Vector3CrossProduct(player.front, player.up));
        }

		// Draw
		// ---------------------

		BeginDrawing();

		// set glClearColor (hope i got that jargon right)
		ClearBackground(BLACK);

		BeginMode3D(camera);

		// Draw skybox first
		rlDisableBackfaceCulling();
		rlDisableDepthMask();
		DrawModel(skybox, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
		rlEnableBackfaceCulling();
		rlEnableDepthMask();

		// draw "player model"
        DrawSphere(player.pos, 0.5, BLACK);
        DrawSphereWires(player.pos, 0.6f, 10, 10, GRAY);

        // Player DEBUG axes
        if(DEBUG_ON) DrawLine3D(player.pos,  Vector3Add(player.pos, Vector3Scale(player.up, 2)), GREEN);
        if(DEBUG_ON) DrawLine3D(player.pos,  Vector3Add(player.pos, Vector3Scale(player.right,2 )), BLUE);
        if(DEBUG_ON) DrawLine3D(player.pos,  Vector3Add(player.pos, Vector3Scale(player.front, 2)), RED);

        // Draw 'star' sphere
		DrawSphere((Vector3){0,0,0}, 10, YELLOW);
		DrawSphereWires((Vector3){0,0,0}, 10.05f, 20, 20, GRAY);

		// Draw 'orbital' sphere

		DrawSphere(orbitalPlanetPos, 3, RED);

		DrawGrid(80, 1.0f);

		EndMode3D();
	   
		// make some text
		DrawText("WOW! TEXT!", 10, 10, 20, BLACK);
		DrawText(TextFormat("CameraPos: %.1f, %.1f, %.1f", camera.position.x, camera.position.y, camera.position.z), 10, 30, 20, RAYWHITE);
		DrawText(TextFormat("CameraUp: %.2f, %.2f, %.2f", camera.up.x, camera.up.y, camera.up.z), 10, 50, 20, RAYWHITE);
		DrawText(TextFormat("CameraTarget: %.1f, %.1f, %.1f", camera.target.x, camera.target.y, camera.target.z), 10, 70, 20, RAYWHITE);

		// explanitory on self?
		EndDrawing();
	}

	// De-init
	CloseWindow(); // Close window and GL contextx

	return 0;
}



