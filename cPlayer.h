// Ping, Copyright Scott Girling 2007
#pragma once

#include "defines.h"

class cPlayer
{
private:
	SDL_Surface*	p_Image;
	SDL_Rect		p_Rect;

public:
	SDL_Rect		m_Rect;
	SDL_Surface*	m_Image;
	int				m_iVelX;
	int				m_iVelY;
	int				m_iPosX;
	int				m_iPosY;
	int				m_iW;
	int				m_iH;
	int				m_iX;
	int				m_iY;
	int				m_Score;

	cPlayer()
	{
	}

	cPlayer(SDL_Surface* image, int rX, int rY, int rW, int rH, int posX, int posY)
	{
		m_iVelX		= 0;
		m_iVelY		= 0;

		m_Score		= 0;

		m_iPosX		= posX;
		m_iPosY		= posY;

		m_iW		= rW;
		m_iH		= rH;

		m_iX		= rX;
		m_iY		= rY;

		m_Image		= image;
		p_Image		= m_Image;
		
		m_Rect.x	= rX;
		m_Rect.x	= p_Rect.x;
		
		m_Rect.y	= rY;
		m_Rect.y	= p_Rect.y;
		
		m_Rect.w	= rW;
		m_Rect.w	= p_Rect.w;
		
		m_Rect.h	= rH;
		m_Rect.h	= p_Rect.w;
	}

};

