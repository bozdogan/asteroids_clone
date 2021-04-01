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


inline v2
VectorRotate(v2 A, float angle)
{
    v2 Result;
    Result.x = A.x*cosf(angle) - A.y*sinf(angle);
    Result.y = A.x*sinf(angle) + A.y*cosf(angle);

    return Result;
}

inline float
VectorLength(v2 A)
{
    return sqrt(A.x*A.x + A.y*A.y);
}

#define ASTEROIDS_VECTOR_H
#endif