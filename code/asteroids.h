#ifndef ASTEROIDS_H

#include <math.h>
#include "asteroids_vector.h"
// NOTE(bora): polygon'da kullandım.
#include <vector>

struct input_state
{
    bool32 Up;
    bool32 Right;
    bool32 Down;
    bool32 Left;

    bool32 Reset;
};

struct game_info
{
    int32 WindowWidth;
    int32 WindowHeight;
    char *Title;
    bool32 Running;
    
    int32 FrameWidth;
    int32 FrameHeight;

    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Texture *Frame;

    input_state Input;
    float DeltaTime;
};

struct polygon
{
    // TODO(bora): I didn't wanna be the one to use "vectors." And I will,
    // eventually, get rid of it.
    std::vector<v2> V;
};

struct game_object
{
    polygon Shape;
    v2 Pos;
    v2 Vel;
    v2 Acc;
    float Angle;
};


inline v2
Direction(game_object A)
{
    v2 Result = {0, -1};
    Result = VectorRotate(Result, A.Angle);

    // NOTE(bora): Make sure it is a unit vector
    float Len = VectorLength(Result);
    SDL_assert(abs(Len - 1.0f) < 0.0000001);

    return Result;
}

#define ASTEROIDS_H
#endif