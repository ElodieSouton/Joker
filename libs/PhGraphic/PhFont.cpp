/**
* Copyright (C) 2012-2013 Phonations
* License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
*/

#include "PhFont.h"

#include <QtGlobal>



#include <glu.h>

#include "PhTools/PhDebug.h"

PhFont::PhFont(): _texture(-1), _glyphHeight(0)
{
	font = NULL;
	boldness = 0;
}

bool PhFont::setFontFile(QString fontFile)
{
	PHDEBUG << fontFile;

	if(fontFile != this->fontFile)
	{
		this->fontFile = fontFile;
		init(this->fontFile);
	}

}

QString PhFont::getFontFile()
{
	return fontFile;
}

// This will split the setting of the bolness and the fontfile, which allow to change the boldness without reloading a font
bool PhFont::init(QString fontFile)
{
	if(font != NULL){
		TTF_CloseFont(font);
	}
	font = TTF_OpenFont(fontFile.toStdString().c_str(), 100);

	//Font foreground color is white
	SDL_Color color = {255, 255, 255, 255};

	// used to set the base surface
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
	SDL_Surface * matrixSurface = SDL_CreateRGBSurface(0, 2048, 2048, 32, rmask, gmask, bmask, amask);

	// Font background color is transparent
	Uint32 backgroundColor = 0x00000000;
	SDL_FillRect(matrixSurface, NULL, backgroundColor);

	// Space between glyph
	int space = 128;
	_glyphHeight = 0;

	//set the boldness
	PHDEBUG << "Setting the font boldness to :" << boldness;
	for(int i = 0; i <= boldness; i++)
	{
		TTF_SetFontOutline(font, i);
		// We get rid of the 32 first useless char
		for(Uint16 ch = 0; ch < 256; ++ch)
		{
			if(TTF_GlyphIsProvided(font, ch))
			{
				int minx, maxx, miny, maxy, advance;
				TTF_GlyphMetrics(font, ch, &minx,&maxx, &miny, &maxy, &advance);
				if(advance != 0)
				{
					// First render the glyph to a surface
					SDL_Surface * glyphSurface = TTF_RenderGlyph_Blended(font, ch, color);
					if (!glyphSurface)
						PHDEBUG << "Error during the Render Glyph of " << (char) ch << SDL_GetError();
					SDL_Rect glyphRect;
					glyphRect.x = (ch % 16) * space;
					glyphRect.y = (ch / 16) * space;
					glyphRect.w = glyphSurface->w;
					glyphRect.h = glyphSurface->h;
					if(glyphRect.h > _glyphHeight)
						_glyphHeight = glyphRect.h;
					//PHDEBUG << ch << (char) ch << minx << maxx << miny << maxy << advance << _glyphHeight;
					// Then blit it to the matrix
					SDL_BlitSurface( glyphSurface, NULL, matrixSurface, &glyphRect );

					// Store information about the glyph
					_glyphAdvance[ch] = advance;

					SDL_FreeSurface(glyphSurface);
				}
				else
					PHDEBUG <<" Error with Glyph of char:" << ch << (char) ch << minx << maxx << miny << maxy << advance;
			}
			else
				_glyphAdvance[ch] = 0;
		}
	}

	glEnable( GL_TEXTURE_2D );
	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &_texture );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, _texture );


	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, matrixSurface->format->BytesPerPixel, matrixSurface->w, matrixSurface->h, 0,
				  GL_RGBA, GL_UNSIGNED_BYTE, matrixSurface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Once the texture is created, the surface is no longer needed.
	SDL_FreeSurface(matrixSurface);
//	TTF_CloseFont(font);

	return true;
}

int PhFont::getAdvance(unsigned char ch)
{
	return _glyphAdvance[ch];
}

void PhFont::select()
{
	glBindTexture(GL_TEXTURE_2D, (GLuint)_texture);
}
int PhFont::getBoldness() const
{
	return boldness;
}

void PhFont::setBoldness(int value)
{
	boldness = value;
	init(fontFile);
}

