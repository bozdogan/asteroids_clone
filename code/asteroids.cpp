
#include "asteroids.h"

inline void
SetColor(v4u Color)
{
    SDL_SetRenderDrawColor(Game.Renderer,
                           Color.r, Color.g, Color.b, Color.a);
}

inline void
DrawPoint(v2 Pos)
{
    SDL_RenderDrawPoint(Game.Renderer, (int32)Pos.x, (int32)Pos.y);
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
DrawCircle(float Radius, v2 ScreenPos)
{
    // NOTE(bora): It's not a circle, it's a manygon!!1 

    polygon Manygon; // Such poly.
    int32 Edges = 23; // Many edges.
    for(int i = 0; i < Edges; ++i)
    {
        float Theta = i*(2*PI32/Edges); // So math.
        Manygon.V.push_back({Radius*cosf(Theta), Radius*sinf(Theta)});
    }

    DrawPolygon(Manygon, ScreenPos);
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
    Asteroid.Color = V4u(rand()%255, rand()%255, rand()%255);

    int32 Edges = RandomInt(8 , 12);
    for(int i = 0; i < Edges; ++i)
    {
        float Theta = i*(2*PI32/Edges);
        Asteroid.Shape.V.push_back({r*cosf(Theta) + RandomInt(3, 10),
                                    r*sinf(Theta) + RandomInt(3, 10)});
    }
        
    return Asteroid;
}

inline projectile
CreateProjectile(game_object Shooter, float Speed, bool Friendly=true)
{
    projectile Projectile;
    
    Projectile.Pos = Shooter.Pos;
    Projectile.Vel = Speed*Direction(Shooter);
    Projectile.Friendly = Friendly;
    Projectile.DistanceTravelled = 0;

    return Projectile;
}

internal void
Initialize(stage *Level)
{
    float r = 10;
    Level->Ship.Size = r;
    Level->Ship.Shape.V = {
        { 0, -r},
        {-r,  r},
        { 0,  r/2},
        { r,  r}
    };
    Level->Ship.Pos = {(float)(Game.FrameWidth/2),
                             (float)(Game.FrameHeight/2)};
    Level->Ship.Vel = {0, 0};
    Level->Ship.Acc = {0, 0};
    Level->Ship.Angle = 0;

    Level->Asteroids.clear();
    for(int i = 0; i < 10; ++i)
    {
        Level->Asteroids.push_back(GenerateAsteroid(RandomInt(1, 3)));
    }

    Level->Projectiles.clear();

    Level->BackColor = V4u(0, 0, 0);
}

internal void
Update(stage *Level)
{
    SetColor(Level->BackColor);
    SDL_RenderClear(Game.Renderer);

    SetColor(V4u(255, 255, 255));

    polygon DisplayShape;
    DisplayShape.V = {};
    for(int Vertex = 0;
        Vertex < Level->Ship.Shape.V.size();
        ++Vertex)
    {
        DisplayShape.V.push_back(VectorRotate(Level->Ship.Shape.V[Vertex], Level->Ship.Angle));
    }
    DrawPolygon(DisplayShape, Level->Ship.Pos);

    // // NOTE(bora): Collision circle
    // SetColor(V4u(255, 42, 0));
    // DrawCircle(Level->Ship.Size,
    //            Centroid(Level->Ship.Shape) + Level->Ship.Pos);

    for(int i = 0; i < Level->Asteroids.size(); ++i)
    {
        SetColor(Level->Asteroids[i].Color);
        DrawPolygon(Level->Asteroids[i].Shape, Level->Asteroids[i].Pos);

        // // NOTE(bora): Collision circle
        // SetColor(V4u(255, 42, 0));
        // DrawCircle(Level->Asteroids[i].Size,
        //            Centroid(Level->Asteroids[i].Shape) + Level->Asteroids[i].Pos);
    }

    SetColor(V4u(255, 255, 255));
    for(int i = 0; i < Level->Projectiles.size(); ++i)
    {
        DrawPoint(Level->Projectiles[i].Pos);
    }

    SetColor(V4u(180, 20, 50));
    DrawPoint(Level->Ship.Pos);

    // NOTE(bora):
    //
    //      === MOVEMENT CODE ===

    v2 ShipDirection = Direction(Level->Ship);
    float Acceleration = 500;
    float RotationSpeed = 4;

    // printf("Level->Ship Position: (%.2f, %.2f)    \tAngle: %.4f\tVel: (%.2f, %.2f)\tAcc: (%.2f, %.2f)\n",
    //        Level->Ship.Pos.x, Level->Ship.Pos.y, Level->Ship.Angle,
    //        Level->Ship.Vel.x, Level->Ship.Vel.y,
    //        Level->Ship.Acc.x, Level->Ship.Acc.y);

    if(Game.Input.Up)
    {
        Level->Ship.Acc = Acceleration*ShipDirection;
    }
    if(Game.Input.Down)
    {
        Level->Ship.Acc = -Acceleration*ShipDirection;
    }
    if(!(Game.Input.Up || Game.Input.Down))
    {
        Level->Ship.Acc = {0, 0};
    }

    if(Game.Input.Right)
    {
        Level->Ship.Angle += RotationSpeed*Game.DeltaTime;
    }

    if(Game.Input.Left)
    {
        Level->Ship.Angle -= RotationSpeed*Game.DeltaTime;
    }

    float SpeedLimit = 200;
    float Friction = 0.1*SpeedLimit;
    float SpeedEpsilon = 1;  // pixels per second

    if(VectorLength(Level->Ship.Vel) > 0)
    {
        Level->Ship.Acc -= Friction*UnitVector(Level->Ship.Vel);
    }

    Level->Ship.Vel += Level->Ship.Acc*Game.DeltaTime;

    if(VectorLength(Level->Ship.Vel) < SpeedEpsilon)
    {
        Level->Ship.Vel = {0, 0};
    }

    // NOTE(bora): Velocity cap
    if(VectorLength(Level->Ship.Vel) > SpeedLimit)
    {
        Level->Ship.Vel = SpeedLimit*UnitVector(Level->Ship.Vel);
    }

    Level->Ship.Pos += Level->Ship.Vel*Game.DeltaTime;

    // NOTE(bora): Wrap around
    if(Level->Ship.Pos.y < 0)
    {
        Level->Ship.Pos.y = Game.FrameHeight + fmod(Level->Ship.Pos.y, Game.FrameHeight);
    }
    if(Level->Ship.Pos.y > Game.FrameHeight)
    {
        Level->Ship.Pos.y = fmod(Level->Ship.Pos.y, Game.FrameHeight);
    }

    if(Level->Ship.Pos.x < 0)
    {
        Level->Ship.Pos.x = Game.FrameWidth + fmod(Level->Ship.Pos.x, Game.FrameWidth);
    }
    if(Level->Ship.Pos.x > Game.FrameWidth)
    {
        Level->Ship.Pos.x = fmod(Level->Ship.Pos.x, Game.FrameWidth);
    }

    // NOTE(bora):
    //
    //     === COLLISION CODE ===

    int32 NumCollisions = 0;
    for(int i = 0; i < Level->Asteroids.size(); ++i)
    {
        if(Collide(Level->Ship, Level->Asteroids[i]))
        {
            ++NumCollisions;
            printf("PARKOUR!");
        }
    }

    // TODO(bora): Shoot projectile
    if(Game.Input.Action)
    {
        Level->BackColor = V4u(0, 120, 0);
    }
    else if(NumCollisions)
    {
        Level->BackColor = V4u(120, 0, 0);
    }
    else
    {
        Level->BackColor = V4u(0, 0, 0);
    }
}
