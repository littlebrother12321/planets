//Makeing a basic planet explorer game.

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
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

Vector3 LocalToWorld(const Player *p1, Vector3 v1)
{
    Vector3 World = Vector3Add(
        Vector3Add(
            Vector3Scale(p1->right, v1.x),
            Vector3Scale(p1->up, v1.y)),
        Vector3Scale(p1->front, v1.z));

    return Vector3Normalize(World);
}


bool debug = false;

int main(int argc, char* argv[])
{
	printf("Hello, world!\n");

        // Argument Handling

        printf("Arguments: ");
        for (int i = 0; i < argc; i++) {
            printf("%s ", argv[i]);
          if (strcmp(argv[i], "--debug") == 0) {

              debug = true;
          }
        }
        printf("\n");

	// set sum screens
	const int screenWidth = 800;
	const int screenHeight = 450;

	// Resizable window
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // Init window
	InitWindow(screenWidth, screenHeight, "raylib projeccs (GLFW!)");

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

	/* Vector3 playerPos = (Vector3){ 0.0f, 0.0f, 0.0f }; */
    /* Vector3 playerFront = camera.target; */
    /* Vector3 playerUp = (Vector3){0.0f, 1.0f, 0.0f}; */
    /* Vector3 playerRight = Vector3CrossProduct(playerFront, playerUp); */

    Vector3 orbitalPlanetPos = (Vector3){ 0.0f, 0.0f, 0.0f };

	// Generate cubemap
	Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
	Model skybox = LoadModelFromMesh(cube);
	Image img = LoadImage("assets/StandardCubeMap.png");
	TextureCubemap cubemap = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);

	// Specified cubemap shaders
	Shader ShaderCubemap = LoadShader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
	skybox.materials[0].shader = ShaderCubemap;

	skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemap;
	UnloadImage(img);


	// Ready the game
	DisableCursor();

	// set gamer frames
	SetTargetFPS(60);


	// Do stuff wihle window is open
	while (!WindowShouldClose()) {
		// -----------------------
		// Update variables here
		// -----------------------

		UpdateCamera(&camera, CAMERA_THIRD_PERSON);

		// Actual variables
		/* player.pos = Vector3Subtract(camera.target, (Vector3){0.0f, 2.0f, 0.0f}); */
        //camera.position = Vector3Add(player.pos, (Vector3){ 0.0f, 2.0f, 0.0f});

		orbitalPlanetPos = (Vector3){ sin(GetTime() * 0.25f) * 25.0f, 0.0f, cos(GetTime() * 0.25f) * 25.0f};

        // Player movement scripts
		// Needed for player height
		if (IsKeyDown(KEY_R)) {
            camera.target.y += 1.0f / 5.0f;
            camera.position.y += 1.0f / 5.0f;
            player.pos.y += 1.0f / 5.0f;
        };
		if (IsKeyDown(KEY_F)) {
            camera.target.y -= 1.0f / 5.0f;
            camera.position.y -= 1.0f / 5.0f;
            player.pos.y -= 1.0f / 5.0f;
        };

        player.up = (Vector3){0.0f, 1.0f, 0.0f};
        if (IsKeyDown(KEY_Q)) {
          camera.up =
              Vector3RotateByAxisAngle(camera.up, camera.target, PI * 0.25f);
          Vector3 Tempright =
              Vector3Normalize(Vector3CrossProduct(camera.target, camera.up));
          camera.up =
              Vector3Normalize(Vector3CrossProduct(Tempright, camera.target));
        }

        player.pos = Vector3Subtract(camera.target, (Vector3){ 0.0f, 1.0f, 0.0f});
        //camera.target = Vector3Add(player.pos, player.front);
        player.front = camera.target;
        player.up = camera.up;
        player.right = Vector3CrossProduct(player.front, player.up);

        if(debug) DrawLine3D(player.pos,  Vector3Add(player.pos, player.up), (Color){255, 161, 0, 255});
        if(debug) DrawLine3D(player.pos,  Vector3Add(player.pos, player.right), (Color){255, 161, 0, 255});
        if(debug) DrawLine3D(player.pos,  Vector3Add(player.pos, player.front), (Color){255, 161, 0, 255});

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

		// draw "PLAYER MODEL"
		DrawSphere(player.pos, 0.5, BLACK);
        printf("Player Position: %f, %f, %f\n", player.pos.x, player.pos.y, player.pos.z);

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



