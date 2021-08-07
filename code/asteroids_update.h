#ifndef ASTEROIDS_UPDATE_H

internal game_object
CreateAsteroid(int32 Size, v2 Pos, v2 Vel)
{
    if(Size < 1) Size = 1;

    game_object Asteroid;
    float r = Size*10;
    Asteroid.Size = r;
    Asteroid.Pos = Pos;
    Asteroid.Vel = Vel;
    Asteroid.Acc = {0, 0};
    Asteroid.Color = V4u(255, 255, 255);

    int32 Edges = RandomInt(8 , 12);
    for(int i = 0; i < Edges; ++i)
    {
        float Theta = i*(2*PI32/Edges);
        Asteroid.Shape.V.push_back({r*cosf(Theta) + RandomInt(3, 10),
                                    r*sinf(Theta) + RandomInt(3, 10)});
    }
        
    return Asteroid;
}

internal game_object
CreateRandomAsteroid(int32 Size)
{
    v2 Pos = {(float)RandomInt(0, Game.FrameWidth - 1),
                    (float)RandomInt(0, Game.FrameHeight - 1)};
    v2 Vel;
    {
        float Theta = RandomFloat(0, 2*PI32);
        SDL_assert(Theta <= 2*PI32);

        float Velocity = RandomFloat(10, 50);
        Vel = {Velocity*cosf(Theta), Velocity*sinf(Theta)};
    }
        
    return CreateAsteroid(Size, Pos, Vel);
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
MoveShip(stage *Level)
{
    v2 ShipDirection = Direction(Level->Ship);
    float Acceleration = 500;
    float RotationSpeed = 4;

#if DEBUG && 0
    printf("Level->Ship Position: (%.2f, %.2f)    \tAngle: %.4f\tVel: (%.2f, %.2f)\tAcc: (%.2f, %.2f)\n",
           Level->Ship.Pos.x, Level->Ship.Pos.y, Level->Ship.Angle,
           Level->Ship.Vel.x, Level->Ship.Vel.y,
           Level->Ship.Acc.x, Level->Ship.Acc.y);
#endif

    if(Game.Input.Up)
    {
        Level->Ship.Acc = Acceleration*ShipDirection;
    }
    // NOTE(bora): Ignore Down input
    // if(Game.Input.Down)
    // {
    //     Level->Ship.Acc = -Acceleration*ShipDirection;
    // }
    if(!(Game.Input.Up))
    // if(!(Game.Input.Up || Game.Input.Down))
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
}

internal void
MoveProjectiles(stage *Level)
{
    for(int i = 0;
        i < Level->Projectiles.size();)
    {
        if(Level->Projectiles[i].DistanceTravelled >= Level->DistanceCap)
        {
            Level->Projectiles.erase(Level->Projectiles.begin() + i);
        }
        else
        {
            v2 VelocityPerFrame = Level->Projectiles[i].Vel*Game.DeltaTime;
            Level->Projectiles[i].Pos += VelocityPerFrame;
            Level->Projectiles[i].DistanceTravelled += VectorLength(VelocityPerFrame);
            
            // NOTE(bora): Wrap around
            if(Level->Projectiles[i].Pos.y < 0)
            {
                Level->Projectiles[i].Pos.y = Game.FrameHeight + fmod(Level->Projectiles[i].Pos.y, Game.FrameHeight);
            }
            if(Level->Projectiles[i].Pos.y > Game.FrameHeight)
            {
                Level->Projectiles[i].Pos.y = fmod(Level->Projectiles[i].Pos.y, Game.FrameHeight);
            }

            if(Level->Projectiles[i].Pos.x < 0)
            {
                Level->Projectiles[i].Pos.x = Game.FrameWidth + fmod(Level->Projectiles[i].Pos.x, Game.FrameWidth);
            }
            if(Level->Projectiles[i].Pos.x > Game.FrameWidth)
            {
                Level->Projectiles[i].Pos.x = fmod(Level->Projectiles[i].Pos.x, Game.FrameWidth);
            }

            ++i;
        }
    }
}

internal void
MoveAsteroids(stage *Level)
{
    for(int i = 0;
        i < Level->Asteroids.size();
        ++i)
    {
        Level->Asteroids[i].Pos += Level->Asteroids[i].Vel*Game.DeltaTime;

        // NOTE(bora): Wrap around
        if(Level->Asteroids[i].Pos.y < 0)
        {
            Level->Asteroids[i].Pos.y = Game.FrameHeight + fmod(Level->Asteroids[i].Pos.y, Game.FrameHeight);
        }
        if(Level->Asteroids[i].Pos.y > Game.FrameHeight)
        {
            Level->Asteroids[i].Pos.y = fmod(Level->Asteroids[i].Pos.y, Game.FrameHeight);
        }

        if(Level->Asteroids[i].Pos.x < 0)
        {
            Level->Asteroids[i].Pos.x = Game.FrameWidth + fmod(Level->Asteroids[i].Pos.x, Game.FrameWidth);
        }
        if(Level->Asteroids[i].Pos.x > Game.FrameWidth)
        {
            Level->Asteroids[i].Pos.x = fmod(Level->Asteroids[i].Pos.x, Game.FrameWidth);
        }
    }
}

#define ASTEROIDS_UPDATE_H
#endif