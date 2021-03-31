
#include <math.h>
// NOTE(bora): polygon'da kullandım.
#include <vector>

union v2
{
    struct
    {
        float x, y;
    };

    struct
    {
        float E[2];
    };
};

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return Result;
}

struct polygon
{
    // TODO(bora): I didn't wanna be to one to get vectors. And I will,
    // eventually, get rid of it.
    std::vector<v2> V;
};

struct game_object
{
    polygon Shape;
    v2 Pos;
    float Angle;
};

inline v2
RotateVector(v2 A, float angle)
{
    v2 Result;
    Result.x = A.x*cosf(angle) - A.y*sinf(angle);
    Result.y = A.x*sinf(angle) + A.y*cosf(angle);

    return Result;
}

inline float
Length(v2 A)
{
    return sqrt(A.x*A.x + A.y*A.y);
}

inline v2
Direction(game_object A)
{
    v2 Result = {0, -1};
    Result = RotateVector(Result, A.Angle);

    // TODO(bora): Use a decent assert macro
    if(abs(Length(Result) - 1.0f) > 0.0000001)
        while(true) printf("BIR YANLISLIK OLDU: %f\n", Length(Result));

    return Result;
}


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

void
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

void
Initialize(game_object *Player)
{
    Player->Shape.V = {
        {  0, -20},
        {-10,  10},
        {  0,   0},
        { 10,  10}
    };
    Player->Pos = {100, 100};
    Player->Angle = 0;
}

void
Update(game_object *Player)
{
    SetColor(0, 0, 0);
    SDL_RenderClear(Game.Renderer);

    SetColor(255, 255, 255);

    polygon DisplayShape;
    DisplayShape.V = {};
    for(int Vertex = 0;
        Vertex < Player->Shape.V.size();
        ++Vertex)
    {
        DisplayShape.V.push_back(RotateVector(Player->Shape.V[Vertex], Player->Angle));
    }
    DrawPolygon(DisplayShape, Player->Pos);

    SetColor(180, 20, 50);
    SDL_RenderDrawPoint(Game.Renderer, Player->Pos.x, Player->Pos.y);
    SDL_RenderDrawPoint(Game.Renderer, Player->Pos.x, Player->Pos.y - 1);
    SDL_RenderDrawPoint(Game.Renderer, Player->Pos.x, Player->Pos.y + 1);
    SDL_RenderDrawPoint(Game.Renderer, Player->Pos.x - 1, Player->Pos.y);
    SDL_RenderDrawPoint(Game.Renderer, Player->Pos.x + 1, Player->Pos.y);

    // if(Game.Input.Up)    --Player->Pos.y;
    // if(Game.Input.Down)  ++Player->Pos.y;
    // if(Game.Input.Right) ++Player->Pos.x;
    // if(Game.Input.Left)  --Player->Pos.x;

    v2 Dir = Direction(*Player);
    printf("Direction: (%f, %f) Angle: %f\n", Dir.x, Dir.y, Player->Angle);
    if(Game.Input.Up)
    {
        Player->Pos.x += Dir.x;
        Player->Pos.y += Dir.y;
    }
    if(Game.Input.Down)
    {
        Player->Pos.x -= Dir.x;
        Player->Pos.y -= Dir.y;
    }
    if(Game.Input.Right)
    {
        Player->Angle += .02f;
    }
    if(Game.Input.Left)
    {
        Player->Angle -= .02f;
    }
}
