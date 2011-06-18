// Ping, Copyright Scott Girling 2007
#include <stack>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_audio.h"
#include "SDL/SDL_image.h"
#include "defines.h"
#include "cPlayer.h"

using namespace std;

// State Stack
struct StateStruct
{
	void(*StatePointer)();
};

stack<StateStruct> g_StateStack;

// Global Variables
SDL_Surface*	g_Backdrop[NUMBER_BACKDROPS]	= { false };
SDL_Surface*	g_Window						= NULL;
SDL_Surface*	g_GameImage						= IMG_Load("Data/Game.png");
SDL_Event		g_Event;
Mix_Music*		g_Music[NUMBER_SONGS]			= { false };
int				g_Timer;
cPlayer			g_PlayerOne;
cPlayer			g_PlayerTwo;
cPlayer			g_Ball;

// Function Prototypes
void Menu();
void Game();
void Exit();
void Win();

void Init();
void Shutdown();

void DrawBackdrop(SDL_Surface *blit);
void ClearScreen();
void HandleMenuInput();
void HandleGameInput();
void HandleExitInput();
void HandleWinInput();

void drawSprite(cPlayer player, SDL_Surface* imageSurface,
				SDL_Surface* s_screenSurface, int dx, int dy, int dw, int dh);

void transColour(SDL_Surface* imageSurface,
				 int r, int g, int b);

bool check_collision(SDL_Rect &A, SDL_Rect &B);

int main(int argc, char **argv)
{
	Init();

	while(!g_StateStack.empty())
	{
		g_StateStack.top().StatePointer();
	}

	Shutdown();

	return 0;

}

void DrawBackdrop(SDL_Surface *blit)
{
	SDL_Rect source = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	SDL_Rect destination = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	SDL_BlitSurface(blit,&source,g_Window,&destination);
}

void ClearScreen()
{
	SDL_FillRect(g_Window, 0, 0);
}

void Init()
{
	printf("Initializing System.\n");
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO );

	g_Window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);

	SDL_WM_SetCaption(WINDOW_CAPTION, 0);

	printf("Opening Audio.\n");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	g_Timer = SDL_GetTicks();

	printf("Loading Resources into Memory.\n");
	g_Backdrop[0] = IMG_Load("Data/Menu.png");
	g_Backdrop[1] = IMG_Load("Data/Exit.png");
	g_Music[0] = Mix_LoadMUS("Data/Menu.ogg");
	g_Music[1] = Mix_LoadMUS("Data/boing.ogg");

	printf("Loading States.\n");
	StateStruct state;
	state.StatePointer = Exit;
	g_StateStack.push(state);

	state.StatePointer = Menu;
	g_StateStack.push(state);

	Mix_PlayMusic(g_Music[0], -1);

	printf("Setting up Players.\n");
	g_PlayerOne = cPlayer(g_GameImage,PLAYER_ONE_BLOCK_X,PLAYER_ONE_BLOCK_Y,
		PLAYER_BLOCK_W,PLAYER_BLOCK_H,PLAYER_ONE_START_X,PLAYER_ONE_START_Y);

	g_PlayerTwo = cPlayer(g_GameImage,PLAYER_TWO_BLOCK_X,PLAYER_TWO_BLOCK_Y,
		PLAYER_BLOCK_W,PLAYER_BLOCK_H,PLAYER_TWO_START_X,PLAYER_TWO_START_Y);

	g_Ball = cPlayer(g_GameImage,BALL_X,BALL_Y,BALL_W,BALL_H,BALL_START_X,BALL_START_Y);

	g_Ball.m_iVelY = 10;

	printf("System Initialized. Play on!\n\n");

}

void Shutdown()
{
	SDL_FreeSurface(g_Window);

	SDL_FreeSurface(g_Backdrop[0]);
	SDL_FreeSurface(g_Backdrop[1]);

	Mix_FreeMusic(g_Music[0]);
	Mix_FreeMusic(g_Music[1]);

	printf("Good bye!");

	SDL_Quit();
}

void Menu()
{
	if(Mix_PausedMusic())
		{
			Mix_ResumeMusic();
		}

	if((SDL_GetTicks() - g_Timer) >= FRAME_RATE)
	{

		HandleMenuInput();

		ClearScreen();

		DrawBackdrop(g_Backdrop[0]);

		SDL_UpdateRect(g_Window,0,0,0,0);

		g_Timer = SDL_GetTicks();

	}
}

void Exit()
{
	if((SDL_GetTicks() - g_Timer) >= FRAME_RATE)
	{
		HandleExitInput();

		ClearScreen();

		DrawBackdrop(g_Backdrop[1]);

		SDL_UpdateRect(g_Window,0,0,0,0);

		g_Timer = SDL_GetTicks();
	}
}

void Game()
{
	
	if((SDL_GetTicks() - g_Timer) >= FRAME_RATE)
	{
		HandleGameInput();

		ClearScreen();

		g_PlayerOne.m_Rect.x = g_PlayerOne.m_iPosX;
		g_PlayerOne.m_Rect.y = g_PlayerOne.m_iPosY;
		g_PlayerOne.m_Rect.w = 85;
		g_PlayerOne.m_Rect.h = 21;

		g_PlayerTwo.m_Rect.x = g_PlayerTwo.m_iPosX;
		g_PlayerTwo.m_Rect.y = g_PlayerTwo.m_iPosY;
		g_PlayerTwo.m_Rect.w = 85;
		g_PlayerTwo.m_Rect.h = 21;

		g_Ball.m_Rect.x = g_Ball.m_iPosX;
		g_Ball.m_Rect.y = g_Ball.m_iPosY;
		g_Ball.m_Rect.w = 22;
		g_Ball.m_Rect.h = 22;

		SDL_FillRect(g_Window,&g_PlayerOne.m_Rect,SDL_MapRGB(g_Window->format,0,0,0));
		SDL_FillRect(g_Window,&g_PlayerTwo.m_Rect,SDL_MapRGB(g_Window->format,0,0,0));
		SDL_FillRect(g_Window,&g_Ball.m_Rect,SDL_MapRGB(g_Window->format,0,0,0));


		g_PlayerOne.m_iPosX += g_PlayerOne.m_iVelX;

		if((g_PlayerOne.m_iPosX < 0) ||(g_PlayerOne.m_iPosX + g_PlayerOne.m_iW > WINDOW_WIDTH))
		{
			g_PlayerOne.m_iPosX -= g_PlayerOne.m_iVelX;
		}

		g_PlayerTwo.m_iPosX += g_PlayerTwo.m_iVelX;

		if((g_PlayerTwo.m_iPosX < 0) ||(g_PlayerTwo.m_iPosX + g_PlayerTwo.m_iW > WINDOW_WIDTH))
		{
			g_PlayerTwo.m_iPosX -= g_PlayerTwo.m_iVelX;
		}

		g_Ball.m_iPosY += g_Ball.m_iVelY;
		g_Ball.m_iPosX += g_Ball.m_iVelX;

		if(g_Ball.m_iPosX + g_Ball.m_iW > WINDOW_WIDTH)
		{
			g_Ball.m_iVelX -= 5;
		}

		if(g_Ball.m_iPosX < 0)
		{
			g_Ball.m_iVelX += 5;
		}

		if(check_collision(g_Ball.m_Rect, g_PlayerOne.m_Rect))
		{
			Mix_PlayMusic(g_Music[1], 0);

			g_Ball.m_iVelY = -10;
			if(g_PlayerOne.m_iVelX > 0)
			{
				g_Ball.m_iVelX = 5;
			}
			if(g_PlayerOne.m_iVelX < 0)
			{
				g_Ball.m_iVelX = -5;
			}
		}

		if(check_collision(g_Ball.m_Rect, g_PlayerTwo.m_Rect))
		{
			Mix_PlayMusic(g_Music[1], 0);

			g_Ball.m_iVelY = 10;
			if(g_PlayerTwo.m_iVelX > 0)
			{
				g_Ball.m_iVelX = 5;
			}
			if(g_PlayerTwo.m_iVelX < 0)
			{
				g_Ball.m_iVelX = -5;
			}
		}

		if(g_Ball.m_iPosY > g_Ball.m_iH + WINDOW_HEIGHT)
		{
			g_PlayerTwo.m_Score += 1;
			SDL_Delay(300);
			g_Ball.m_iPosX = BALL_START_X;
			g_Ball.m_iPosY = BALL_START_Y;
			g_Ball.m_iVelX = 0;
		}

		if(g_Ball.m_iPosY < -22)
		{
			g_PlayerOne.m_Score += 1;
			SDL_Delay(300);
			g_Ball.m_iPosX = BALL_START_X;
			g_Ball.m_iPosY = BALL_START_Y;
			g_Ball.m_iVelX = 0;
		}

		transColour(g_Ball.m_Image,255,255,255);

		drawSprite(g_PlayerOne, g_PlayerOne.m_Image, g_Window, 
			g_PlayerOne.m_iPosX, g_PlayerOne.m_iPosY, g_PlayerOne.m_iW, g_PlayerOne.m_iH);

		drawSprite(g_PlayerTwo, g_PlayerTwo.m_Image, g_Window,
			g_PlayerTwo.m_iPosX, g_PlayerTwo.m_iPosY, g_PlayerTwo.m_iW, g_PlayerTwo.m_iH);

		drawSprite(g_Ball, g_Ball.m_Image, g_Window, g_Ball.m_iPosX, g_Ball.m_iPosY, 
			g_Ball.m_iW, g_Ball.m_iH);

		SDL_UpdateRect(g_Window,0,0,0,0);

		g_Timer = SDL_GetTicks();

		SDL_Delay(1);
	}
}

void HandleMenuInput()
{
	if(SDL_PollEvent(&g_Event))
	{
		if(g_Event.type == SDL_QUIT)
		{
			while(!g_StateStack.empty())
			{
				g_StateStack.pop();
			}
		}

		if(g_Event.type == SDL_KEYDOWN)
		{
			if(g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_StateStack.pop();
				return;
			}

			if(g_Event.key.keysym.sym == SDLK_q)
			{
				g_StateStack.pop();
				return;
			}

			if(g_Event.key.keysym.sym == SDLK_n)
			{
				Mix_PauseMusic();

				StateStruct temp;
				temp.StatePointer = Game;
				g_StateStack.push(temp);

				g_PlayerOne.m_Score = 0;
				g_PlayerTwo.m_Score = 0;

				return;
			}
		}
	}
}

void HandleGameInput()
{
	if(SDL_PollEvent(&g_Event))
	{
		if(g_Event.type == SDL_QUIT)
		{
			while(!g_StateStack.empty())
			{
				g_StateStack.pop();
			}
		}

		if(g_Event.type == SDL_KEYDOWN)
		{
			if(g_Event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_StateStack.pop();
				return;
			}
		}

		if(g_Event.type == SDL_KEYDOWN)
		{
			switch(g_Event.key.keysym.sym)
			{
			case SDLK_LEFT: g_PlayerOne.m_iVelX -= 10;
				break;
			case SDLK_RIGHT: g_PlayerOne.m_iVelX += 10;
				break;
			}
		}

		if(g_Event.type == SDL_KEYUP)
		{
			switch(g_Event.key.keysym.sym)
			{
			case SDLK_LEFT: g_PlayerOne.m_iVelX += 10;
				break;
			case SDLK_RIGHT: g_PlayerOne.m_iVelX -= 10;
				break;
			}
		}

		if(g_Event.type == SDL_KEYDOWN)
		{
			switch(g_Event.key.keysym.sym)
			{
			case SDLK_a: g_PlayerTwo.m_iVelX -= 10;
				break;
			case SDLK_d: g_PlayerTwo.m_iVelX += 10;
				break;
			}
		}

		if(g_Event.type == SDL_KEYUP)
		{
			switch(g_Event.key.keysym.sym)
			{
			case SDLK_a: g_PlayerTwo.m_iVelX += 10;
				break;
			case SDLK_d: g_PlayerTwo.m_iVelX -= 10;
				break;
			}
		}
	}
}

void HandleExitInput()
{
	Mix_PauseMusic();

	if(SDL_PollEvent(&g_Event))
	{
		if(g_Event.type == SDL_QUIT)
		{
			while(!g_StateStack.empty())
			{
				g_StateStack.pop();
			}
		}

		if(g_Event.type == SDL_KEYDOWN)
		{
			if(g_Event.key.keysym.sym == SDLK_y)
			{
					g_StateStack.pop();
					return;
			}

			if(g_Event.key.keysym.sym == SDLK_n)
			{
				StateStruct temp;
				temp.StatePointer = Menu;
				g_StateStack.push(temp);
				return;
			}
		}
	}
}

void drawSprite(cPlayer player, SDL_Surface* imageSurface,
				SDL_Surface* s_screenSurface, int dx, int dy, int dw, int dh)
{
	SDL_Rect srcRect;
	srcRect.x = player.m_iX;
	srcRect.y = player.m_iY;
	srcRect.w = player.m_iW;
	srcRect.h = player.m_iH;

	SDL_Rect dstRect;
	dstRect.x = dx;
	dstRect.y = dy;
	dstRect.w = dw;
	dstRect.h = dh;

	SDL_BlitSurface(imageSurface,&srcRect,s_screenSurface,&dstRect);
}

void transColour(SDL_Surface* imageSurface, int r, int g, int b)
{
	SDL_SetColorKey(imageSurface,SDL_SRCCOLORKEY,SDL_MapRGB(imageSurface->format,r,g,b));
}

bool check_collision(SDL_Rect &A, SDL_Rect &B)
{
	int leftA, leftB; 
	int rightA, rightB; 
	int topA, topB; 
	int bottomA, bottomB;

	leftA = A.x;
	rightA = A.x + A.w; 
	topA = A.y; 
	bottomA = A.y + A.h;

	leftB = B.x; 
	rightB = B.x + B.w; 
	topB = B.y; 
	bottomB = B.y + B.h;

	if( bottomA <= topB ) 
	{ 
		return false; 
	} 
	if( topA >= bottomB ) 
	{ 
		return false; 
	} 
	if( rightA <= leftB ) 
	{ 
		return false; 
	} 
	if( leftA >= rightB ) 
	{ 
		return false; 
	} //If none of the sides from A are outside B return true;

	return true;
}
