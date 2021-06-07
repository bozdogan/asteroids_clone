
#include "asteroids.h"
#include "asteroids_draw.h"
#include "asteroids_move.h"

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

#if DEBUG
    // NOTE(bora): Collision circle
    SetColor(V4u(255, 42, 0));
    DrawCircle(Level->Ship.Size,
               Centroid(Level->Ship.Shape) + Level->Ship.Pos);
#endif
    for(int i = 0; i < Level->Asteroids.size(); ++i)
    {
        SetColor(Level->Asteroids[i].Color);
        DrawPolygon(Level->Asteroids[i].Shape, Level->Asteroids[i].Pos);

#if DEBUG
        // NOTE(bora): Collision circle
        SetColor(V4u(255, 42, 0));
        DrawCircle(Level->Asteroids[i].Size,
                   Centroid(Level->Asteroids[i].Shape) + Level->Asteroids[i].Pos);
#endif
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

    MoveShip(Level);

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
