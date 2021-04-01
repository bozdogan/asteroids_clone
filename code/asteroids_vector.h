#ifndef ASTEROIDS_VECTOR_H

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

inline v2 &
operator+=(v2 &B, v2 &A)
{
    B = B + A;
    return B;
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return Result;
}

inline v2 &
operator-=(v2 &B, v2 &A)
{
    B = B - A;
    return B;
}

inline v2
operator*(v2 A, v2 B)
{
    v2 Result;
    Result.x = A.x*B.x;
    Result.y = A.y*B.y;
    
    return Result;
}

inline v2
operator*(v2 A, float B)
{
    v2 Result;
    // NOTE(bora): Did you know? B can be infinity. How fun is that!
    if(isnormal(B))
    {
        Result.x = A.x*B;
        Result.y = A.y*B;
    }
    else
    {
        Result = {0, 0};
    }

    return Result;
}

inline v2
operator*(float A, v2 B)
{
    return B*A;
}

inline float
VectorInner(v2 A, v2 B)
{
    return A.x*B.x + A.y*B.y;
}

inline float
VectorLength(v2 A)
{
    return sqrt(A.x*A.x + A.y*A.y);
}


inline v2
UnitVector(v2 A)
{
    return A*(1/VectorLength(A));
}

inline v2
VectorRotate(v2 A, float angle)
{
    v2 Result;
    Result.x = A.x*cosf(angle) - A.y*sinf(angle);
    Result.y = A.x*sinf(angle) + A.y*cosf(angle);

    return Result;
}

#define ASTEROIDS_VECTOR_H
#endif