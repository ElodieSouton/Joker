/**
 * Copyright (C) 2012-2014 Phonations
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#ifndef GRAPHICSTRIPTEST_H
#define GRAPHICSTRIPTEST_H

#include <QObject>

class GraphicStripTest : public QObject
{
	Q_OBJECT
public:
	explicit GraphicStripTest(QObject *parent = 0);

private slots:
	void testStripDocObject();
};

#endif // GRAPHICSTRIPTEST_H
