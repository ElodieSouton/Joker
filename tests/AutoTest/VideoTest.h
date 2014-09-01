/**
 * Copyright (C) 2012-2014 Phonations
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#ifndef VIDEOTEST_H
#define VIDEOTEST_H

#include <QObject>

#include "PhGraphic/PhGraphicView.h"

#include "PhVideo/PhVideoEngine.h"
#include "VideoTestSettings.h"


class VideoTest : public QObject
{
	Q_OBJECT
public:
	explicit VideoTest();

protected slots:
	void paint(int width, int height);

private slots:
	void initTestCase();

	void openMovieTest();
	void goToTest01();
	void goToTest02();
	void goToTest03();
	void playTest();
	void deinterlaceTest();
private:
	VideoTestSettings _settings;
	PhVideoEngine _videoEngine;
	PhGraphicView _view;

#warning /// @todo move to a generic graphicTest implementation
	void saveBuffer(QString fileName);
	void findMatch(QImage source);
};

#endif // VIDEOTEST_H
