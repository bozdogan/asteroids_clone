#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define internal static
#define global_variable static
#define local_persist static

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "asteroids.h"
global_variable game_info Game;
#include "asteroids.cpp"

#include <stdio.h>


internal void
HandleEvents()
{
    SDL_Event Event;
    SDL_PollEvent(&Event);

    switch(Event.type)
    {
        case SDL_QUIT:
            Game.Running = false;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            bool IsDown = Event.type == SDL_KEYDOWN;

            switch(Event.key.keysym.scancode)
            {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    Game.Input.Up = IsDown ? 1 : 0;
                    break;

                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    Game.Input.Right = IsDown ? 1 : 0;
                    break;

                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    Game.Input.Down = IsDown ? 1 : 0;
                    break;

                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    Game.Input.Left = IsDown ? 1 : 0;
                    break;

                case SDL_SCANCODE_R:
                    Game.Input.Reset = IsDown ? 1 : 0;
                    break;

                case SDL_SCANCODE_ESCAPE:
                    Game.Running = false;
                    break;
            }
        } break;
    }
}

internal void
UpdateFPSOnTitle()
{
    double FrameRate = 1.0/Game.DeltaTime;

    char NewTitle[36];
    snprintf(NewTitle, 36, "%s - FPS: %.1f",
        Game.Title, FrameRate);
    SDL_SetWindowTitle(Game.Window, NewTitle);
}

int main(int argc, char** argv)
{
    int32 ScaleFactor = 2;

    Game.FrameWidth = 400;
    Game.FrameHeight = 300;
    Game.WindowWidth = Game.FrameWidth*ScaleFactor;
    Game.WindowHeight = Game.FrameHeight*ScaleFactor;
    Game.Title = "Asteroids!";

    if(SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Bir yanlışlık oldu.");
        SDL_Quit();
        return 1;
    }

    if(SDL_CreateWindowAndRenderer(Game.WindowWidth, Game.WindowHeight,
                                   0, &Game.Window, &Game.Renderer))
    {
        printf("Bir yanlışlık oldu.");
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowTitle(Game.Window, Game.Title);
    Game.Frame = SDL_CreateTexture(Game.Renderer,
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_TARGET,
                                   Game.FrameWidth, Game.FrameHeight);

    SDL_RenderClear(Game.Renderer);
    SDL_RenderPresent(Game.Renderer);
    input_state PrevInput = {0, 0, 0, 0};

    // NOTE(bora): Tanımlar
    game_object Player;
    Initialize(&Player);

    Game.Running = true;
    uint64 ClockPrev = SDL_GetPerformanceCounter();
    while(Game.Running)
    {
        if( Game.Input.Up != PrevInput.Up 
            && Game.Input.Up)
        {
        }
        if( Game.Input.Right != PrevInput.Right
            && Game.Input.Right)
        {
        }
        if( Game.Input.Down != PrevInput.Down
            && Game.Input.Down)
        {
        }
        if( Game.Input.Left != PrevInput.Left
            && Game.Input.Left)
        {
        }

        if( Game.Input.Reset != PrevInput.Reset
            && Game.Input.Reset)
        {
            printf("\n\n   === RESET ===\n\n");
            Initialize(&Player);
        }

        SDL_SetRenderTarget(Game.Renderer, Game.Frame);
        Update(&Player);
        SDL_SetRenderTarget(Game.Renderer, 0);
        SDL_RenderCopy(Game.Renderer, Game.Frame, 0, 0);
        SDL_RenderPresent(Game.Renderer);
        
        SDL_Delay(16);
        printf("%f\n", Game.DeltaTime);

        PrevInput = Game.Input;
        HandleEvents();

        uint64 ClockNow = SDL_GetPerformanceCounter();
        Game.DeltaTime = (float)(ClockNow - ClockPrev)/SDL_GetPerformanceFrequency();
        UpdateFPSOnTitle();
        ClockPrev = ClockNow;
    }

    SDL_DestroyRenderer(Game.Renderer);
    SDL_DestroyWindow(Game.Window);
    SDL_Quit();

    printf("\nGoodbye.\n");
    return 0;
}