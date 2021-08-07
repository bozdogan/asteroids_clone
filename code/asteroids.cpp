
#include "asteroids.h"
#include "asteroids_draw.h"
#include "asteroids_update.h"

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
        Level->Asteroids.push_back(CreateRandomAsteroid(RandomInt(1, 3)));
    }

    Level->Projectiles.clear();

    Level->BackColor = V4u(0, 0, 0);
    Level->BulletSpeed = 400;
    Level->DistanceCap = VectorLength({(float)Game.FrameWidth,
                                       (float)Game.FrameHeight})/2.0f;
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

#if VISUALDEBUG
    // NOTE(bora): Collision circle
    SetColor(V4u(255, 42, 0));
    DrawCircle(Level->Ship.Size,
               Centroid(Level->Ship.Shape) + Level->Ship.Pos);
#endif
    for(int i = 0; i < Level->Asteroids.size(); ++i)
    {
        SetColor(Level->Asteroids[i].Color);
        DrawPolygon(Level->Asteroids[i].Shape, Level->Asteroids[i].Pos);

#if VISUALDEBUG
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
    MoveAsteroids(Level);
    MoveProjectiles(Level);

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

        for(int j = 0;
            j < Level->Projectiles.size();
            ++j)
        {
            if(Collide(Level->Asteroids[i], Level->Projectiles[j]))
            {
                // NOTE(bora): Splitting an asteroid
                //     Asteroid size is in pixels. It is assumed that size of
                //     an asteroid is either 10, 20 or 30px. Because it is how
                //     I designed the code initially and it is most likely that
                //     the game design will change in the future so I'm basically
                //     writing a long comment to remind myself why this matters.
                
                Level->Projectiles.erase(Level->Projectiles.begin() + j);

                // TODO(bora): I'm extracting the "tier" of an asteroid from its `Size`
                // property. Asteroids are "game_objects" now but if that changes in the
                // future this process may not be necessary.
                int32 OldSize = Level->Asteroids[i].Size/10;
                v2 CollisionPos = Level->Asteroids[i].Pos;
                v2 CollisionVel = Level->Asteroids[i].Vel;

                int32 NewSize = OldSize - 1;
                float frac_Old_New = (float)OldSize/(float)NewSize;
                // NOTE(bora): It felt too fast but I still wanted proportional
                // velocity so I tuned it down a bit.
                v2 NewVel2 = 0.8*frac_Old_New*VectorRotate(CollisionVel, DegreesOf(-90));
                v2 NewVel1 = 0.8*frac_Old_New*VectorRotate(CollisionVel, DegreesOf(+90));
                
                Level->Asteroids.erase(Level->Asteroids.begin() + i);
                if(OldSize > 1)
                {
                    Level->Asteroids.push_back(
                        CreateAsteroid(NewSize, CollisionPos, NewVel1));
                    Level->Asteroids.push_back(
                        CreateAsteroid(NewSize, CollisionPos, NewVel2));
                }
            }
        }
    }

    // TODO(bora): Shoot projectile
    if(INPUT_NOREPEAT(Action))
    {
        Level->Projectiles.push_back(CreateProjectile(Level->Ship, Level->BulletSpeed));
    }
    
    if(NumCollisions)
    {
        Level->BackColor = V4u(120, 0, 0);
    }
    else
    {
        Level->BackColor = V4u(0, 0, 0);
    }
}
