#ifndef ASTEROIDS_H

#include <math.h>
#include "asteroids_vector.h"
// NOTE(bora): polygon'da kullandım.
#include <vector>

#define PI32 (3.14159274f)

struct input_state
{
    bool32 Up;
    bool32 Right;
    bool32 Down;
    bool32 Left;
    bool32 Action;

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
    int32 Size;
    polygon Shape;
    v2 Pos;
    v2 Vel;
    v2 Acc;
    float Angle;

    v4u Color;
};

struct projectile
{
    v2 Pos;
    v2 Vel;
    bool32 Friendly;
    float DistanceTravelled;
};

struct stage
{
    game_object Ship;
    std::vector<game_object> Asteroids;
    std::vector<projectile> Projectiles;

    v4u BackColor;
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

inline int32
RandomInt(int32 low, int32 high)
{
    // NOTE(bora): Both ends included
    return (int32)(rand()%(high + 1 - low)) + low;
}

internal float
SignedArea(polygon Shape)
{
    float Result = 0;

    int Next = 1;
    for (int Point = 0; Point < Shape.V.size(); ++Point)
    {
        Result += Shape.V[Point].x * Shape.V[Next].y - Shape.V[Next].x * Shape.V[Point].y;
        Next = (Next + 1)%Shape.V.size();
    }

    return Result/2;
}

internal v2
Centroid(polygon Shape)
{
    float Area = SignedArea(Shape);

    float Cx = 0;
    float Cy = 0;

    int Next = 1;
    for (int Point = 0; Point < Shape.V.size(); ++Point)
    {
        float t = Shape.V[Point].x*Shape.V[Next].y - Shape.V[Next].x*Shape.V[Point].y;
        
        Cx += (Shape.V[Point].x+Shape.V[Next].x) * t;
        Cy += (Shape.V[Point].y+Shape.V[Next].y) * t;
        Next = (Next + 1) % Shape.V.size();
    }

    Cx = Cx / (6.0 * Area);
    Cy = Cy / (6.0 * Area);

    return v2{Cx, Cy};
}

inline v2
Centroid(game_object A)
{
    return Centroid(A.Shape) + A.Pos;
}

inline bool32
Collide(game_object A, game_object B)
{
    v2 CenterA = Centroid(A.Shape) + A.Pos;
    v2 CenterB = Centroid(B.Shape) + B.Pos;
    return VectorLength(CenterA - CenterB) <= (A.Size + B.Size);
}

inline bool32
Collide(game_object A, projectile B)
{
    v2 CenterA = Centroid(A.Shape) + A.Pos;
    return VectorLength(CenterA - B.Pos) <= A.Size;
}

#define ASTEROIDS_H
#endif