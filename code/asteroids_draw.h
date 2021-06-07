#ifndef ASTEROIDS_DRAW_H

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

#define ASTEROIDS_DRAW_H
#endif