
#include "asteroids.h"

inline void
SetColor(uint8 r, uint8 g, uint8 b)
{
    SDL_SetRenderDrawColor(Game.Renderer, r, g, b, 0xFF);
}

inline void
DrawLine(v2 StartPos, v2 EndPos)
{
    SDL_RenderDrawLine(Game.Renderer, (int32)StartPos.x, (int32)StartPos.y, 
                                      (int32)EndPos.x,   (int32)EndPos.y);
}

internal void
DrawPolygon(polygon Poly, v2 ScreenPos)
{
    for(int Vertex = 1; Vertex < Poly.V.size(); ++Vertex)
    {
        v2 Start = Poly.V[Vertex] + ScreenPos;
        v2 End =  Poly.V[Vertex - 1] + ScreenPos;
        DrawLine(Start, End);
    }
    DrawLine(Poly.V[Poly.V.size() - 1] + ScreenPos, Poly.V[0] + ScreenPos);
}

internal void
Initialize(game_object *Ship)
{
    Ship->Shape.V = {
        {  0, -15},
        {-10,  10},
        {  0,   0},
        { 10,  10}
    };
    Ship->Pos = {(float)(Game.FrameWidth/2),
                   (float)(Game.FrameHeight/2)};
    Ship->Vel = {0, 0};
    Ship->Acc = {0, 0};
    Ship->Angle = 0;
}

internal void
Update(game_object *Ship)
{
    SetColor(0, 0, 0);
    SDL_RenderClear(Game.Renderer);

    SetColor(255, 255, 255);

    polygon DisplayShape;
    DisplayShape.V = {};
    for(int Vertex = 0;
        Vertex < Ship->Shape.V.size();
        ++Vertex)
    {
        DisplayShape.V.push_back(VectorRotate(Ship->Shape.V[Vertex], Ship->Angle));
    }
    DrawPolygon(DisplayShape, Ship->Pos);

    SetColor(180, 20, 50);
    SDL_RenderDrawPoint(Game.Renderer, Ship->Pos.x, Ship->Pos.y);

    v2 ShipDirection = Direction(*Ship);
    float Acceleration = 500;
    float RotationSpeed = 4;

    printf("Ship Position: (%.2f, %.2f)    \tAngle: %.4f\tVel: (%.2f, %.2f)\tAcc: (%.2f, %.2f)\n",
           Ship->Pos.x, Ship->Pos.y, Ship->Angle,
           Ship->Vel.x, Ship->Vel.y,
           Ship->Acc.x, Ship->Acc.y);
    if(Game.Input.Up)
    {
        Ship->Acc = Acceleration*ShipDirection;
    }

    if(Game.Input.Down)
    {
        Ship->Acc = -Acceleration*ShipDirection;
    }
    
    if(!(Game.Input.Up || Game.Input.Down))
    {
        Ship->Acc = {0, 0};
    }

    if(Game.Input.Right)
    {
        Ship->Angle += RotationSpeed*Game.DeltaTime;
    }

    if(Game.Input.Left)
    {
        Ship->Angle -= RotationSpeed*Game.DeltaTime;
    }

    float SpeedLimit = 200;
    float Friction = 0.1*SpeedLimit;
    float SpeedEpsilon = 1;  // pixels per second

    if(VectorLength(Ship->Vel) > 0)
    {
        Ship->Acc -= Friction*UnitVector(Ship->Vel);
    }

    Ship->Vel += Ship->Acc*Game.DeltaTime;

    if(VectorLength(Ship->Vel) < SpeedEpsilon)
    {
        Ship->Vel = {0, 0};
    }

    // NOTE(bora): Velocity cap
    if(VectorLength(Ship->Vel) > SpeedLimit)
    {
        Ship->Vel = SpeedLimit*UnitVector(Ship->Vel);
    }

    Ship->Pos += Ship->Vel*Game.DeltaTime;

    // NOTE(bora): Wrap around
    if(Ship->Pos.y < 0)
    {
        Ship->Pos.y = Game.FrameHeight + fmod(Ship->Pos.y, Game.FrameHeight);
    }
    if(Ship->Pos.y > Game.FrameHeight)
    {
        Ship->Pos.y = fmod(Ship->Pos.y, Game.FrameHeight);
    }

    if(Ship->Pos.x < 0)
    {
        Ship->Pos.x = Game.FrameWidth + fmod(Ship->Pos.x, Game.FrameWidth);
    }
    if(Ship->Pos.x > Game.FrameWidth)
    {
        Ship->Pos.x = fmod(Ship->Pos.x, Game.FrameWidth);
    }
}
