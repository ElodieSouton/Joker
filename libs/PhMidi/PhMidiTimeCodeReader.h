/**
 * Copyright (C) 2012-2014 Phonations
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#ifndef PHMIDITIMECODEREADER_H
#define PHMIDITIMECODEREADER_H

#include <QObject>
#include <QTimer>

#include "PhSync/PhClock.h"

#include "PhMidiInput.h"

/**
 * @brief PhMidiTimeCodeReader read midi timecode message
 *
 * This class can open a new midi port, read
 * midi timecode messages and update the clock accordingly.
 */
class PhMidiTimeCodeReader : public PhMidiInput
{
	Q_OBJECT
public:
	/**
	 * @brief The PhMidiTimeCodeReader constructor
	 * @param tcType The initial clock timecode type.
	 */
	PhMidiTimeCodeReader(PhTimeCodeType tcType);

	/**
	 * @brief The current reader decoded timecode type
	 * @return A timecode type value.
	 */
	PhTimeCodeType timeCodeType() {
		return _tcType;
	}

	/**
	 * @brief The PhMidiTimeCodeWriter clock
	 *
	 * Subscribe to this clock synchronized with the
	 * incoming midi timecode message.
	 *
	 * @return A clock instance.
	 */
	PhClock *clock() {
		return &_clock;
	}

signals:
	/**
	 * @brief Signal sent upon a different timecode type message
	 * @param tcType A timecode type value.
	 */
	void timeCodeTypeChanged(PhTimeCodeType tcType);

protected:
	void onQuarterFrame(unsigned char data);
	void onTimeCode(int hh, int mm, int ss, int ff, PhTimeCodeType tcType);

private slots:
	void checkPause();

private:
	PhTimeCodeType _tcType;
	PhClock _clock;
	QTimer _pauseDetectionTimer;
	int _pauseDetectionCounter;
};

#endif // PHMIDITIMECODEREADER_H
