
#include "asteroids.h"

inline void
SetColor(uint8 r, uint8 g, uint8 b)
{
    SDL_SetRenderDrawColor(Game.Renderer, r, g, b, 0xFF);
}

inline void
DrawLine(v2 StartPos, v2 EndPos)
{
    SDL_RenderDrawLineF(Game.Renderer, 
                        StartPos.x, StartPos.y,
                        EndPos.x, EndPos.y);
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

    v2 PlayerDirection = Direction(*Ship);
    // printf("Ship Direction: (%.2f, %.2f)  \tAngle: %.4f\tVel: (%.2f, %.2f)\tAcc: (%.2f, %.2f)\n",
    //        PlayerDirection.x, PlayerDirection.y, Ship->Angle,
    //        Ship->Vel.x, Ship->Vel.y,
    //        Ship->Acc.x, Ship->Acc.y);
    if(Game.Input.Up)
    {
        Ship->Acc.x += (0.01)*PlayerDirection.x;
        Ship->Acc.y += (0.01)*PlayerDirection.y;
    }

    if(Game.Input.Down)
    {
        Ship->Acc.x -= (0.01)*PlayerDirection.x;
        Ship->Acc.y -= (0.01)*PlayerDirection.y;
    }
    
    if(!(Game.Input.Up || Game.Input.Down))
    {
        Ship->Acc.x = 0;
        Ship->Acc.y = 0;
    }

    if(Game.Input.Right)
    {
        Ship->Angle += .02f;
    }

    if(Game.Input.Left)
    {
        Ship->Angle -= .02f;
    }


    // NOTE(bora): Acceleration decay
    if(VectorLength(Ship->Acc) > 0.1)
    {
        Ship->Acc.x = Ship->Acc.x/2; 
        Ship->Acc.y = Ship->Acc.y/2; 
    }

    Ship->Vel = Ship->Vel + Ship->Acc;
    Ship->Pos = Ship->Pos + Ship->Vel;

    // NOTE(bora): Wrap around
    if(Ship->Pos.y < 0)
    {
        Ship->Pos.y = Game.FrameHeight + Ship->Pos.y;
    }
    if(Ship->Pos.y > Game.FrameHeight)
    {
        Ship->Pos.y = Ship->Pos.y - Game.FrameHeight;
    }

    if(Ship->Pos.x < 0)
    {
        Ship->Pos.x = Game.FrameWidth + Ship->Pos.x;
    }
    if(Ship->Pos.x > Game.FrameWidth)
    {
        Ship->Pos.x = Ship->Pos.x - Game.FrameWidth;
    }
}
