/**
* Copyright (C) 2012-2013 Phonations
* License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
*/


#include <QApplication>
#include <QDebug>

#include "PhStrip/PhStripDoc.h"

#include "MainWindow.h"

int main(int argc, char *argv[])
{

	PhDebug::init(false, true, true, true, true, true, 0x01, APP_NAME);
	PHDEBUG << ORG_NAME << APP_NAME << APP_VERSION;

	QApplication a(argc, argv);
	MainWindow w;

    w.show();

	if (argc > 1)
	{
		QString fileName = argv[1];
		w.openFile(fileName);
	}

    return a.exec();

}
