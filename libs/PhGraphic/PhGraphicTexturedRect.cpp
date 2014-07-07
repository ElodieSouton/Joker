/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include "PhTools/PhDebug.h"
#include "PhGraphicTexturedRect.h"

PhGraphicTexturedRect::PhGraphicTexturedRect(int x, int y, int w, int h)
	: PhGraphicRect(x, y, w, h),
	_texture(0),
	_tu(1.0f),
	_tv(1.0f),
	_textureWidth(0),
	_textureHeight(0)
{

}

PhGraphicTexturedRect::~PhGraphicTexturedRect()
{
}


bool PhGraphicTexturedRect::createTextureFromSurface(SDL_Surface *surface)
{
	glEnable( GL_TEXTURE_2D );
	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &_texture );

	if(_texture == 0) {
		PHDEBUG << "glGenTextures() errored: is opengl context ready?";
		return false;
	}

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, _texture );

	GLenum textureFormat = 0;

	switch (surface->format->BytesPerPixel) {
	case 1:
		textureFormat = GL_ALPHA;
		break;
	case 3: // no alpha channel
#if defined(Q_OS_MAC)
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGB;
		else
			textureFormat = GL_BGR;
#else
		textureFormat = GL_RGB;
#endif
		break;
	case 4: // contains an alpha channel
#if defined(Q_OS_MAC)
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGBA;
		else
			textureFormat = GL_BGRA;
#else
		textureFormat = GL_RGBA;
#endif

		break;
	default:
		PHDEBUG << "Warning: the image is not truecolor...";
		return false;
	}

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0,
	              textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool PhGraphicTexturedRect::createTextureFromARGBBuffer(void *data, int width, int height)
{
	glEnable( GL_TEXTURE_2D );

	if((width != _textureWidth) || (height != _textureHeight)) {
		if(_texture != 0)
			glDeleteTextures(1, &_texture);
		_textureWidth = width;
		_textureHeight = height;
	}

	// Have OpenGL generate a texture object handle for us
	if(_texture == 0) {
		glGenTextures( 1, &_texture );
		if(_texture == 0) {
			PHDEBUG << "glGenTextures() errored: is opengl context ready?";
			return false;
		}
	}

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, _texture );


	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
	              GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return true;
}

bool PhGraphicTexturedRect::createTextureFromRGBBuffer(void *data, int width, int height)
{
	glEnable( GL_TEXTURE_2D );

	// Have OpenGL generate a texture object handle for us
	if(_texture == 0) {
		glGenTextures( 1, &_texture );
		if(_texture == 0) {
			PHDEBUG << "glGenTextures() errored: is opengl context ready?";
			return false;
		}
	}

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, _texture );

	if((width != _textureWidth) || (height != _textureHeight)) {
		_textureWidth = width;
		_textureHeight = height;

		PHDEBUG << QString("%1x%2").arg(width).arg(height);

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		              GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		glBindTexture( GL_TEXTURE_2D, _texture );
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return true;
}


bool PhGraphicTexturedRect::createTextureFromYUVBuffer(void *data, int width, int height)
{
	glEnable( GL_TEXTURE_2D );
	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &_texture );
	if(_texture == 0) {
		PHDEBUG << "glGenTextures() errored: is opengl context ready?";
		return false;
	}

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, _texture );

	// Edit the texture object's image data using the information SDL_Surface gives us
#if defined(Q_OS_MAC)
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
	              GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, data);
#else
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
	              0x85B9, 0x85BA, data);
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}


void PhGraphicTexturedRect::draw()
{
	PhGraphicRect::draw();

	glBindTexture(GL_TEXTURE_2D, _texture);

	glEnable(GL_TEXTURE_2D);

	//        (0,0) ------ (1,0)
	//          |            |
	//          |            |
	//        (0,1) ------ (1,1)

	glBegin(GL_QUADS);  //Begining the cube's drawing
	{
		glTexCoord3f(0, 0, 1);      glVertex3i(this->x(),      this->y(), this->z());
		glTexCoord3f(_tu, 0, 1);    glVertex3i(this->x() + this->width(), this->y(), this->z());
		glTexCoord3f(_tu, _tv, 1);  glVertex3i(this->x() + this->width(), this->y() + this->height(),  this->z());
		glTexCoord3f(0, _tv, 1);    glVertex3i(this->x(),      this->y() + this->height(),  this->z());
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}

void PhGraphicTexturedRect::setTextureCoordinate(float tu, float tv)
{
	_tu = tu;
	_tv = tv;
}

