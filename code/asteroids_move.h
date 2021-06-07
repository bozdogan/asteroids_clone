#ifndef ASTEROIDS_MOVE_H

internal void
MoveShip(stage *Level)
{
    v2 ShipDirection = Direction(Level->Ship);
    float Acceleration = 500;
    float RotationSpeed = 4;

#if DEBUG
    printf("Level->Ship Position: (%.2f, %.2f)    \tAngle: %.4f\tVel: (%.2f, %.2f)\tAcc: (%.2f, %.2f)\n",
           Level->Ship.Pos.x, Level->Ship.Pos.y, Level->Ship.Angle,
           Level->Ship.Vel.x, Level->Ship.Vel.y,
           Level->Ship.Acc.x, Level->Ship.Acc.y);
#endif

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
}

#define ASTEROIDS_MOVE_H
#endif