/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include "PhGraphicText.h"

PhGraphicText::PhGraphicText(PhFont* font, QString content, int x, int y, int w, int h)
	: PhGraphicRect(x, y, w, h), _font(font), _content(content)
{
}

PhGraphicText::~PhGraphicText()
{

}


void PhGraphicText::setContent(QString content)
{
	_content = content;
}
void PhGraphicText::setFont(PhFont * font)
{
	_font = font;
}

QString PhGraphicText::getContent()
{
	return _content;
}
PhFont * PhGraphicText::getFont()
{
	return _font;
}

void PhGraphicText::draw()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(_color.redF(), _color.greenF(), _color.blueF());

	_font->select();

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	int totalAdvance = 0;
	//Compute the natural width of the content to scale it later
	for(int i = 0; i < _content.length(); i++) {
		totalAdvance += _font->getAdvance(_content.at(i).toLatin1());
	}

	// Set the letter initial horizontal offset
	int advance = 0;
	float space = 0.0625f; // all glyph are in a 1/16 x 1/16 box
	// Display a string
	for(int i = 0; i < _content.length(); i++) {
		unsigned char ch = (unsigned char)_content.at(i).toLatin1();
		if(_font->getAdvance(ch) > 0) {
			// computing texture coordinates
			float tu1 = (ch % 16) * space;
			float tv1 = (ch / 16) * space;
			float tu2 = tu1 + space;
			float tv2 = tv1 + space;

			// computing quads coordinate;
			int h = _h * 128 / _font->getHeight();
			int w = _w * 128 / totalAdvance;

			//        (tu1, tv1) --- (tu2, tv1)
			//            |              |
			//            |              |
			//        (tu1, tv2) --- (tu2, tv2)

			int offset = _x + advance * _w / totalAdvance;
			glBegin(GL_QUADS);  //Begining the cube's drawing
			{
				glTexCoord3f(tu1, tv1, 1);  glVertex3f(offset,      _y, _z);
				glTexCoord3f(tu2, tv1, 1);  glVertex3f(offset + w,  _y, _z);
				glTexCoord3f(tu2, tv2, 1);  glVertex3f(offset + w,  _y + h,  _z);
				glTexCoord3f(tu1, tv2, 1);  glVertex3f(offset,      _y + h,  _z);
			}
			glEnd();

		}
		// Inc the advance
		advance += _font->getAdvance(ch);
	}

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
}

