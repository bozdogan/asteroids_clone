
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

internal game_object
GenerateAsteroid(int32 Size)
{
    if(Size < 1) Size = 1;

    game_object Asteroid;
    float r = Size*10;
    Asteroid.Size = r;
    Asteroid.Pos = {(float)RandomInt(0, Game.FrameWidth - 1),
                    (float)RandomInt(0, Game.FrameHeight - 1)};
    Asteroid.Vel = {0, 0};
    Asteroid.Acc = {0, 0};

    int32 Edges = RandomInt(8 , 12);
    for(int i = 0; i < Edges; ++i)
    {
        float Theta = i*(2*PI32/Edges);
        Asteroid.Shape.V.push_back({r*cosf(Theta) + RandomInt(3, 10),
                                    r*sinf(Theta) + RandomInt(3, 10)});
    }

    return Asteroid;
}

internal void
Initialize(game_object *Ship, std::vector<game_object> &Asteroids)
{
    float r = 10;
    Ship->Size = r;
    Ship->Shape.V = {
        { 0, -r},
        {-r,  r},
        { 0,  r/2},
        { r,  r}
    };
    Ship->Pos = {(float)(Game.FrameWidth/2),
                 (float)(Game.FrameHeight/2)};
    Ship->Vel = {0, 0};
    Ship->Acc = {0, 0};
    Ship->Angle = 0;

    for(int i = 0; i < 10; ++i)
    {
        Asteroids.push_back(GenerateAsteroid(RandomInt(1, 3)));
    }
}

internal void
Update(game_object *Ship, std::vector<game_object> Asteroids)
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

    for(int i = 0; i < Asteroids.size(); ++i)
    {
        DrawPolygon(Asteroids[i].Shape, Asteroids[i].Pos);
    }

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
