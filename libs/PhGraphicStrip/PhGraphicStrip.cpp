/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>

#include "PhTools/PhDebug.h"
#include "PhGraphic/PhGraphicDisc.h"
#include "PhGraphicStrip.h"

PhGraphicStrip::PhGraphicStrip(QObject *parent) :
	QObject(parent),
	_doc(this),
	_clock(_doc.timeCodeType()),
	_trackNumber(4),
	_settings(NULL),
	_maxDrawElapsed(0),
	_timeIn(0),
	_timeOut(0),
	_correctionDelta(0),
	_correctionTimeIn(0),
	_correctionTimeOut(0),
	_correctionTimeIn2(0),
	_correctionTimeOut2(0)
{
	// update the  content when the doc changes
	this->connect(&_doc, SIGNAL(changed()), this, SLOT(onDocChanged()));
}

PhStripDoc *PhGraphicStrip::doc()
{
	return &_doc;
}

PhClock *PhGraphicStrip::clock()
{
	return &_clock;
}

void PhGraphicStrip::setSettings(PhGraphicStripSettings *settings)
{
	PHDEBUG;
	_settings = settings;
}

bool PhGraphicStrip::setFontFile(QString fontFile)
{
	if(_textFont.setFontFile(fontFile)) {
		if(_settings)
			_settings->setTextFontFile(fontFile);
		return true;
	}
	return false;
}

bool PhGraphicStrip::init()
{
	PHDEBUG << _settings;

	PHDEBUG << "Load the strip background";
	_stripBackgroundImage.setFilename(QCoreApplication::applicationDirPath() + PATH_TO_RESSOURCES + "/motif-240.png");
	_stripBackgroundImage.init();

	_stripBackgroundImageInverted.setFilename(QCoreApplication::applicationDirPath() + PATH_TO_RESSOURCES + "/motif-240_black.png");
	_stripBackgroundImageInverted.init();

	PHDEBUG << "Init the sync bar";
	_stripSyncBar.setColor(QColor(225, 86, 108));

	PHDEBUG << "Load the font file";
	QString fontFile = "";
	if(_settings != NULL)
		fontFile = _settings->textFontFile();
	else
		PHDEBUG << "no settings...";

	if(!QFile(fontFile).exists()) {
		PHDEBUG << "File not found:" << fontFile;
		fontFile = QCoreApplication::applicationDirPath() + PATH_TO_RESSOURCES + "/" + "SWENSON.TTF";
		if(_settings != NULL)
			_settings->textFontFile();
		else
			PHDEBUG << "no settings...";
	}
	_textFont.setFontFile(fontFile);

	if(_settings != NULL)
		_textFont.setBoldness(_settings->textBoldness());

	// Init the sync bar
	_stripSyncBar.setColor(QColor(225, 86, 108));

	_hudFont.setFontFile(QCoreApplication::applicationDirPath() + PATH_TO_RESSOURCES + "/" + "ARIAL.TTF");

	// This is used to make some time-based test
	_testTimer.start();

	return true;
}

void PhGraphicStrip::onDocChanged()
{
	_trackNumber = 4;
	foreach(PhStripText *text, _doc.texts()) {
		if(text->track() >= _trackNumber)
			_trackNumber = text->track() + 1;
	}

	foreach(PhStripDetect *detect, _doc.detects()) {
		if(detect->track() >= _trackNumber)
			_trackNumber = detect->track() + 1;
	}
}

PhFont *PhGraphicStrip::getTextFont()
{
	return &_textFont;
}

PhFont *PhGraphicStrip::getHUDFont()
{
	return &_hudFont;
}

void PhGraphicStrip::onExternalCorrection(PhTime delta)
{
	_correctionDelta = delta;
	_correctionTimeIn = _correctionTimeIn2 = _timeIn;
	_correctionTimeOut = _correctionTimeOut2 = _timeOut;
}

QColor PhGraphicStrip::computeColor(PhPeople * people, QList<PhPeople*> selectedPeoples, bool invertColor)
{
	if(!invertColor) {
		if(people) {
			if(selectedPeoples.size() && !selectedPeoples.contains(people)) {
				return QColor(100, 100, 100);
			}
			else {
				return people->color();
			}
		}
		else if(selectedPeoples.size())
			return QColor(100, 100, 100);
		else
			return Qt::black;
	}
	else {
		if(people) {
			if(selectedPeoples.size() && !selectedPeoples.contains(people)) {
				return QColor(155, 155, 155);
			}
			else {
				QColor color(people->color());
				return QColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
			}
		}
		else if(selectedPeoples.size())
			return QColor(155, 155, 155);
		else
			return Qt::white;
	}
}

void PhGraphicStrip::draw(int x, int y, int width, int height, int tcOffset, QList<PhPeople *> selectedPeoples)
{
	int counter = 0;
	bool invertedColor = _settings->invertColor();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int lastDrawElapsed = _testTimer.elapsed();
	//PHDEBUG << "time " << _clock.time() << " \trate " << _clock.rate();

	if(height > 0) {
		int timePerPixel = _settings->horizontalTimePerPixel();
		_textFont.setBoldness(_settings->textBoldness());
		_textFont.setFontFile(_settings->textFontFile());

		int loopCounter = 0;
		int offCounter = 0;
		int cutCounter = 0;

		long syncBar_X_FromLeft = width / 6;
		long delay = (int)(24 * _settings->screenDelay() *  _clock.rate());
		PhTime clockTime = _clock.time() + delay;
		long offset = clockTime / timePerPixel - syncBar_X_FromLeft;

		//Compute the visible duration of the strip
		PhTime stripDuration = width * timePerPixel;


		if(_settings->stripTestMode()) {
			foreach(PhStripCut * cut, _doc.cuts())
			{
				counter++;
				if(cut->timeIn() == clockTime) {
					PhGraphicSolidRect white(x, y, width, height);
					white.setColor(Qt::white);
					white.draw();

					//This is useless to continue the foreach if the cut is displayed.
					break;
				}
			}
			return;
		}

		_timeIn = clockTime - syncBar_X_FromLeft * timePerPixel;
		_timeOut = _timeIn + stripDuration;

		if((_correctionTimeOut2 <_timeIn) || _correctionTimeIn2 > _timeOut) {
			_correctionDelta = 0;
			_correctionTimeIn = 0;
			_correctionTimeOut = 0;
			_correctionTimeIn2 = 0;
			_correctionTimeOut2 = 0;
		}

		//Draw backgroung picture
		int n = width / height + 2; // compute how much background repetition do we need
		long leftBG = 0;
		if(offset >= 0)
			leftBG -= offset % height;
		else
			leftBG -= height - ((-offset) % height);

		PhGraphicTexturedRect* backgroundImage = &_stripBackgroundImage;
		if(invertedColor)
			backgroundImage = &_stripBackgroundImageInverted;

		backgroundImage->setX(x + leftBG);
		backgroundImage->setY(y);
		backgroundImage->setSize(height * n, height);
		backgroundImage->setZ(-2);
		backgroundImage->setTextureCoordinate(n, 1);
		backgroundImage->draw();

		_stripSyncBar.setSize(4, height);
		_stripSyncBar.setPosition(x + width/6, y, 0);
		_stripSyncBar.setColor(QColor(225, 86, 108));

		_stripSyncBar.draw();

		if(_settings->displayRuler()) {
			PhTime rulerTimeIn = _settings->rulerTimeIn();
			PhTime timeBetweenRuler = _settings->timeBetweenRuler();
			int rulerNumber = (_timeIn - rulerTimeIn) / timeBetweenRuler;
			if (rulerNumber < 0)
				rulerNumber = 0;

			PhTime rulerTime = rulerTimeIn + rulerNumber * timeBetweenRuler;
			PhGraphicSolidRect rulerRect;
			PhGraphicDisc rulerDisc;
			PhGraphicText rulerText(&_hudFont);
			QColor rulerColor(80, 80, 80);
			if(invertedColor)
				rulerColor = Qt::white;

			int width = 1000 / timePerPixel;

			rulerRect.setColor(rulerColor);
			rulerRect.setWidth(width);
			rulerRect.setHeight(height / 2);
			rulerRect.setZ(0);
			rulerRect.setY(y);

			rulerDisc.setColor(rulerColor);
			rulerDisc.setRadius(2 * width);
			rulerDisc.setY(y + height / 2 + 3 * width);
			rulerDisc.setZ(0);

			rulerText.setColor(rulerColor);
			rulerText.setY(y + height / 2);
			rulerText.setHeight(height / 2);
			rulerText.setZ(0);

			while (rulerTime < _timeOut + timeBetweenRuler) {
				int correct = 0;
				if((rulerTime + timeBetweenRuler >= _correctionTimeIn) && (rulerTime <= _correctionTimeOut)) {
					correct = _correctionDelta / timePerPixel;
					if(rulerTime + timeBetweenRuler > _correctionTimeOut2)
						_correctionTimeOut2 = rulerTime + timeBetweenRuler;
				}

				counter++;
				int x = rulerTime / timePerPixel - offset + correct;

				rulerRect.setX(x - rulerRect.getWidth() / 2);
				rulerRect.draw();

				QString text = QString::number(rulerNumber);
				rulerText.setContent(text);
				int textWidth = _hudFont.getNominalWidth(text);
				rulerText.setWidth(textWidth);
				rulerText.setX(x - textWidth / 2);
				rulerText.draw();

				x += timeBetweenRuler / timePerPixel / 2;

				rulerRect.setX(x - rulerRect.getWidth() / 2);
				rulerRect.draw();

				rulerDisc.setX(x);
				rulerDisc.draw();

				rulerNumber++;
				rulerTime += timeBetweenRuler;
			}
		}

		int minTimeBetweenPeople = 48000;
		int timeBetweenPeopleAndText = 4000;
		PhStripText ** lastTextList = new PhStripText*[_trackNumber];
		for(int i = 0; i < _trackNumber; i++)
			lastTextList[i] = NULL;

		int trackHeight = height / _trackNumber;

		int verticalTimePerPixel = _settings->verticalTimePerPixel();
		bool displayNextText = _settings->displayNextText();
		PhTime maxTimeIn = _timeOut;
		if(displayNextText)
			maxTimeIn += y * verticalTimePerPixel;

		foreach(PhStripText * text, _doc.texts())
		{
			counter++;
			int track = text->track();

			int correct = 0;
			int correctWidth = 0;
			if((text->timeIn() < _correctionTimeIn) && (text->timeOut() > _correctionTimeIn)) {
				correctWidth = _correctionDelta / timePerPixel;
				if(text->timeIn() < _correctionTimeIn2)
					_correctionTimeIn2 = text->timeIn();
			}
			else if((text->timeIn() >= _correctionTimeIn) && (text->timeIn() < _correctionTimeOut)){
				correct = _correctionDelta / timePerPixel;
				if(text->timeOut() > _correctionTimeOut) {
					correctWidth = -_correctionDelta / timePerPixel;
					if(text->timeOut() > _correctionTimeOut2)
						_correctionTimeOut2 = text->timeOut();
				}
			}

			if( !((text->timeOut() < _timeIn) || (text->timeIn() > _timeOut)) ) {

				PhGraphicText gText(&_textFont, text->content());
				gText.setZ(-1);

				gText.setX(x + text->timeIn() / timePerPixel - offset+ correct);
				gText.setWidth((text->timeOut() - text->timeIn()) / timePerPixel + correctWidth);
				gText.setY(y + track * trackHeight);
				gText.setHeight(trackHeight);
				gText.setZ(-1);
				gText.setColor(computeColor(text->people(), selectedPeoples, invertedColor));

				gText.draw();
			}

			PhPeople * people = text->people();
			QString name = people ? people->name() : "???";
			PhGraphicText gPeople(&_hudFont, name);
			gPeople.setWidth(name.length() * 12);

			PhStripText * lastText = lastTextList[track];
			// Display the people name only if one of the following condition is true:
			// - it is the first text
			// - it is a different people
			// - the distance between the latest text and the current is superior to a limit
			if((
			       (lastText == NULL)
			       || (lastText->people() != text->people())
			       || (text->timeIn() - lastText->timeOut() > minTimeBetweenPeople))
			   ) {

				gPeople.setX(x + (text->timeIn() - timeBetweenPeopleAndText) / timePerPixel - offset - gPeople.getWidth() + correct);
				gPeople.setY(y + track * trackHeight);
				gPeople.setZ(-1);
				gPeople.setHeight(trackHeight / 2);

				gPeople.setColor(computeColor(people, selectedPeoples, invertedColor));

				gPeople.draw();
			}

			if(displayNextText && (_timeIn < text->timeIn()) && ((lastText == NULL) || (text->timeIn() - lastText->timeOut() > minTimeBetweenPeople))) {
				PhPeople * people = text->people();

				int howFarIsText = (text->timeIn() - _timeOut) / verticalTimePerPixel;
				//This line is used to see which text's name will be displayed
				gPeople.setX(width - gPeople.getWidth());
				gPeople.setY(y - howFarIsText - gPeople.getHeight());

				gPeople.setZ(-3);
				gPeople.setHeight(trackHeight / 2);

				gPeople.setColor(computeColor(people, selectedPeoples, invertedColor));

				PhGraphicSolidRect background(gPeople.getX(), gPeople.getY(), gPeople.getWidth(), gPeople.getHeight() + 2);
				if(selectedPeoples.size() && !selectedPeoples.contains(people))
					background.setColor(QColor(90, 90, 90));
				else
					background.setColor(QColor(180, 180, 180));

				background.setZ(gPeople.getZ() - 1);

				if(gPeople.getY() > tcOffset) {
					if(!invertedColor)
						background.draw();

					gPeople.draw();
				}
			}

			lastTextList[track] = text;

			if(text->timeIn() > maxTimeIn)
				break;
		}

		delete lastTextList;

		foreach(PhStripCut * cut, _doc.cuts())
		{
			int correct = 0;
			if((cut->timeIn() >= _correctionTimeIn) && (cut->timeIn() <= _correctionTimeOut))
				correct = _correctionDelta / timePerPixel;

			//_counter++;
			if( (_timeIn < cut->timeIn()) && (cut->timeIn() < _timeOut)) {
				PhGraphicSolidRect gCut;
				gCut.setZ(-1);
				gCut.setWidth(2);

				if(invertedColor)
					gCut.setColor(QColor(255, 255, 255));
				else
					gCut.setColor(QColor(0, 0, 0));
				gCut.setHeight(height);
				gCut.setX(x + cut->timeIn() / timePerPixel - offset + correct);
				gCut.setY(y);

				gCut.draw();
				cutCounter++;
			}
			//Doesn't need to process undisplayed content
			if(cut->timeIn() > _timeOut)
				break;
		}

		int loopWidth = height / 4;
		int loopHalfTimeRange = loopWidth * timePerPixel / 2;

		foreach(PhStripLoop * loop, _doc.loops())
		{
			int correct = 0;
			if((loop->timeIn() + loopHalfTimeRange >= _correctionTimeIn) && (loop->timeIn() - loopHalfTimeRange <= _correctionTimeOut)) {
				correct = _correctionDelta / timePerPixel;
				if(loop->timeIn() + loopHalfTimeRange> _correctionTimeOut2)
					_correctionTimeOut2 = loop->timeIn() + loopHalfTimeRange;
				if(loop->timeIn() - loopHalfTimeRange> _correctionTimeIn2)
					_correctionTimeIn2 = loop->timeIn() + loopHalfTimeRange;
			}

			// This calcul allow the cross to come smoothly on the screen (height * timePerPixel / 8)
			if( ((loop->timeIn() + loopHalfTimeRange) > _timeIn) && ((loop->timeIn() - loopHalfTimeRange) < _timeOut)) {
				PhGraphicLoop gLoop;
				if(!invertedColor)
					gLoop.setColor(Qt::black);
				else
					gLoop.setColor(Qt::white);

				gLoop.setX(x + loop->timeIn() / timePerPixel - offset + correct);
				gLoop.setY(y);
				gLoop.setZ(-1);
				gLoop.setHThick(height / 40);
				gLoop.setHeight(height);
				gLoop.setCrossHeight(height / 4);
				gLoop.setWidth(loopWidth);

				gLoop.draw();
				loopCounter++;
			}

			if(displayNextText && ((loop->timeIn() + height * timePerPixel / 8) > _timeIn)) {
				PhGraphicLoop gLoopPred;

				int howFarIsLoop = (loop->timeIn() - _timeOut) / verticalTimePerPixel;
				gLoopPred.setColor(Qt::white);

				gLoopPred.setHorizontalLoop(true);
				gLoopPred.setZ(-3);

				gLoopPred.setX(width - width / 10);
				gLoopPred.setY(y - howFarIsLoop);
				gLoopPred.setHeight(30);

				gLoopPred.setHThick(3);
				gLoopPred.setCrossHeight(20);
				gLoopPred.setWidth(width / 10);

				gLoopPred.draw();
			}
			if((loop->timeIn() - height * timePerPixel / 8) > _timeOut + 25 * 30)
				break;
		}

		foreach(PhStripDetect * detect, _doc.detects())
		{
			int correct = 0;
			int correctWidth = 0;
			if((detect->timeIn() < _correctionTimeIn) && (detect->timeOut() > _correctionTimeIn)) {
				correctWidth = _correctionDelta / timePerPixel;
				if(detect->timeIn() < _correctionTimeIn2)
					_correctionTimeIn2 = detect->timeIn();
			}
			else if((detect->timeIn() >= _correctionTimeIn) && (detect->timeIn() < _correctionTimeOut)){
				correct = _correctionDelta / timePerPixel;
				if(detect->timeOut() > _correctionTimeOut) {
					correctWidth = -_correctionDelta / timePerPixel;
					if(detect->timeOut() > _correctionTimeOut2)
						_correctionTimeOut2 = detect->timeOut();
				}
			}

			if( detect->off() && (_timeIn < detect->timeOut()) && (detect->timeIn() < _timeOut) ) {
				PhGraphicSolidRect gDetect;

				gDetect.setColor(computeColor(detect->people(), selectedPeoples, invertedColor));

				gDetect.setX(x + detect->timeIn() / timePerPixel - offset + correct);
				gDetect.setY(y + detect->track() * trackHeight + trackHeight * 0.8);
				gDetect.setZ(-1);
				gDetect.setHeight(trackHeight / 20);
				gDetect.setWidth((detect->timeOut() - detect->timeIn()) / timePerPixel + correctWidth);
				gDetect.draw();
				offCounter++;
			}
			//Doesn't need to process undisplayed content
			if(detect->timeIn() > _timeOut)
				break;
		}
	}

	//	PHDEBUG << "off counter : " << offCounter << "cut counter : " << cutCounter << "loop counter : " << loopCounter;

	int currentDrawElapsed = _testTimer.elapsed() - lastDrawElapsed;
	if(currentDrawElapsed > _maxDrawElapsed)
		_maxDrawElapsed = currentDrawElapsed;
	_testTimer.restart();

	if(_settings->displayStripInfo()) {
		int inc = 60;
		PhGraphicText text(&_hudFont, "", 0, 0, 200, inc);
		text.setColor(Qt::red);
		text.setContent(QString("Max : %1").arg(_maxDrawElapsed));
		text.draw();
		text.setY(text.getY() + inc);
		text.setContent(QString("Count : %1").arg(counter));
		text.draw();
	}
}
