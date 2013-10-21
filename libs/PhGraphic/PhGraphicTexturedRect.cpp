/**
* Copyright (C) 2012-2013 Phonations
* License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
*/

#include "PhTools/PhDebug.h"
#include "PhGraphicTexturedRect.h"

PhGraphicTexturedRect::PhGraphicTexturedRect(int x, int y, int w, int h)
	: PhGraphicRect(x, y, w, h), _tu(1.0f), _tv(1.0f)
{
}


bool PhGraphicTexturedRect::createTextureFromSurface(SDL_Surface *surface)
{
	GLenum textureFormat = 0;

    switch (surface->format->BytesPerPixel) {
    case 1:
        textureFormat = GL_ALPHA;
        break;
    case 3:     // no alpha channel
        if (surface->format->Rmask == 0x000000ff)
            textureFormat = GL_RGB;
        else
            textureFormat = GL_BGR;
        break;
    case 4:     // contains an alpha channel
        if (surface->format->Rmask == 0x000000ff)
            textureFormat = GL_RGBA;
        else
            textureFormat = GL_BGRA;
        break;
    default:
        PHDEBUG << "Warning: the image is not truecolor...";
		return false;
    }

	glEnable( GL_TEXTURE_2D );
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &_texture );

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, _texture );


    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D( GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
                  textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool PhGraphicTexturedRect::createTextureFromYUVBuffer(void *data, int width, int height)
{
    glEnable( GL_TEXTURE_2D );
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &_texture );

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, _texture );


    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                  GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}


void PhGraphicTexturedRect::draw(){

	//PHDEBUG << "PhGraphicTexturedRect::draw()";
    glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, _texture);

	glEnable(GL_TEXTURE_2D);

//        (0,0) ------ (1,0)
//          |            |
//          |            |
//        (0,1) ------ (1,1)

	glBegin(GL_QUADS); 	//Begining the cube's drawing
	{
		glTexCoord3f(0, 0, 1);		glVertex3f(_x,		_y,	_z);
		glTexCoord3f(_tu, 0, 1);	glVertex3f(_x + _w,	_y,	_z);
		glTexCoord3f(_tu, _tv, 1);	glVertex3f(_x + _w,	_y + _h,  _z);
		glTexCoord3f(0, _tv, 1);	glVertex3f(_x,		_y + _h,  _z);
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}

void PhGraphicTexturedRect::setTextureCoordinate(float tu, float tv)
{
	_tu = tu;
	_tv = tv;
}

