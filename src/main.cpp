#include <stdio.h>
#include "raylib.h"
#include <cstdlib>
#include <iostream>

class Paddle;

class Ball{
    public:
    float x, y;
    int speed_x;
    int speed_y;
    int radius;

    void Draw() {
        DrawCircle(x,y, radius, BLACK);
    }
    void Update() {
        x += speed_x;
        y += speed_y;
        if(y +radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
        if(x +radius >= GetScreenWidth() || x - radius <= 0) {
            speed_x *= -1;
        }
    }
};

class Paddle {
    protected:
    void KeepInbounds() {
        if(y<0) {
            y=0;
        }
        if(y>GetScreenHeight()-height) {
            y=GetScreenHeight()-height;
        }
    }

    public:
    float x, y;
    float speed;
    float width, height;

    void Draw() {
    DrawRectangle(x,y, width,height, RAYWHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y += speed;
        }
        KeepInbounds();
    }
};

class Endzone {
    public:
    float x, y;
    float width, height;
    void Draw() {
    DrawRectangle(x,y, width,height, BLACK);
    }
};

//extend the paddle class
class CpuPaddle: public Paddle {
    public:
    void Update(bool versus_player, int ball_y) {
        if(versus_player) {
            PlayerMovement();
        } else {
            CpuMovement(ball_y);
        }
        KeepInbounds();
    }
    void PlayerMovement() {
        if (IsKeyDown(KEY_Q)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_A)) {
            y += speed;
        }
    }
    void CpuMovement(int ball_y) {
        if(y <= ball_y) {
            y+=speed;
        }
        if(y >= ball_y) {
            y-=speed;
        }
    }
};

class GameState {
    public:
    bool ball_in_play = true;
    int left_score = 0;
    int right_score = 0;
    bool versus_player = false;
    void Update(Ball &ball) {
        DrawText(TextFormat("%d", left_score), GetScreenWidth()/2 -100, 50, 100, BLACK);
        DrawText(TextFormat("%d", right_score), GetScreenWidth()/2 +50, 50, 100, BLACK);
        DrawText(TextFormat("SPEED: %d", abs(ball.speed_x)), GetScreenWidth()/2 +20, GetScreenHeight()-50, 25, BLACK);
        if(!versus_player) {
            DrawText("CPU MODE", GetScreenWidth()/2 -180, GetScreenHeight()-50, 25, BLACK);
        } else {
            DrawText("PLAYER MODE", GetScreenWidth()/2 -180, GetScreenHeight()-50, 25, BLACK);
        }
        CheckGameState(ball);
        CheckPlayer2Input();
        CheckDifficultyChange(ball);
    }
    //pass by reference in order to modify object and not create local copy
    void CheckGameState(Ball& ball) {
        if(!ball_in_play && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP))) {
            ball.speed_x = 7;
            ball.speed_y = 7;
            ball_in_play = true;
        }
    }
    void CheckPlayer2Input() {
        if(IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_A)) {
            versus_player = true;
        }
    }
    void CheckDifficultyChange(Ball& ball) {
        if(IsKeyPressed(KEY_BACKSPACE)) {
            if(ball.speed_x > 0) {
                ball.speed_x -= 1;
                ball.speed_y -= 1;
            } else {
                ball.speed_x += 1;
                ball.speed_y += 1;
            }
        }
        if(IsKeyPressed(KEY_ENTER)) {
            if(ball.speed_x > 0) {
                ball.speed_x += 1;
                ball.speed_y += 1;
            } else {
                ball.speed_x -= 1;
                ball.speed_y -= 1;
            }
        }
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;
Endzone left;
Endzone right;
GameState game;

void CheckForCollisions(int screen_height,int screen_width, Sound fxWav) {
        if(CheckCollisionCircleRec(Vector2{ball.x,ball.y}, ball.radius, Rectangle{player.x,player.y, player.width,player.height})) {
            ball.speed_x *= -1;
        }
        if(CheckCollisionCircleRec(Vector2{ball.x,ball.y}, ball.radius, Rectangle{cpu.x,cpu.y, cpu.width,cpu.height})) {
            ball.speed_x *= -1;
        }
        if(CheckCollisionCircleRec(Vector2{ball.x,ball.y}, ball.radius, Rectangle{left.x,left.y, left.width,left.height})) {
            ball.y = screen_height/2;
            ball.x = screen_width/2;
            ball.speed_x = 0;
            ball.speed_y = 0;
            game.right_score += 1;
            game.ball_in_play = false;
            PlaySound(fxWav);
        }
        if(CheckCollisionCircleRec(Vector2{ball.x,ball.y}, ball.radius, Rectangle{right.x,right.y, right.width,right.height})) {
            ball.y = screen_height/2;
            ball.x = screen_width /2;
            ball.speed_x = 0;
            ball.speed_y = 0;
            game.left_score += 1;
            game.ball_in_play = false;
            PlaySound(fxWav);
        }
        return;
}

void InitObjects(int screen_width, int screen_height) {
    ball.radius = 20;
    ball.x = screen_width/2;
    ball.y = screen_height/2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    left.width = 25;
    left.height = screen_height;
    left.x = 0;
    left.y = 0;

    right.width = 25;
    right.height = screen_height;
    right.x = screen_width-right.width;
    right.y = 0;

    player.width = 25;
    player.height = 120;
    player.speed = 6;
    player.x = left.x+left.width+5;
    player.y = screen_height/2-player.height/2;

    cpu.width = 25;
    cpu.height = 120;
    cpu.speed = 8;
    cpu.x = right.x - right.width - 5;
    cpu.y = screen_height/2-player.height/2;
};

int main() {
    printf("0");
    const int screen_height = 666;
    const int screen_width = 1512;
    InitWindow(screen_width, screen_height, "pong");
    InitAudioDevice(); 
    Sound fxWav = LoadSound("../resources/windowserror.wav");
    Music music = LoadMusicStream("../resources/mw2-spawn.mp3");
    PlayMusicStream(music);
    SetWindowPosition(0, 0);
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);
    InitObjects(screen_width,screen_height);

    while(!WindowShouldClose()) {
        UpdateMusicStream(music);
        BeginDrawing();
        game.Update(ball);
        ball.Update();
        player.Update();
        cpu.Update(game.versus_player, ball.y);
        CheckForCollisions(screen_height,screen_width, fxWav);
        //without this it would leave trace of ball (redraw)
        ClearBackground((Color){184,194,254,0});
        DrawFPS(40,20);
        DrawLine(screen_width/2,0,screen_width/2,screen_height, BLACK);

        ball.Draw();
        player.Draw();
        cpu.Draw();
        right.Draw();
        left.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}