/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include "PhTools/PhDebug.h"
#include "PhSynchronizer.h"

PhSynchronizer::PhSynchronizer()
	: _syncType(NoSync),
	_stripClock(NULL),
	_videoClock(NULL),
	_syncClock(NULL),
	_settingStripTime(false),
	_settingVideoTime(false),
	_settingSonyTime(false),
	_settingStripRate(false),
	_settingVideoRate(false),
	_settingSonyRate(false)
{
}

void PhSynchronizer::setStripClock(PhClock *clock)
{
	_stripClock = clock;
	if(clock) {
		connect(_stripClock, &PhClock::timeChanged, this, &PhSynchronizer::onStripTimeChanged);
		connect(_stripClock, &PhClock::rateChanged, this, &PhSynchronizer::onStripRateChanged);
	}
}

void PhSynchronizer::setVideoClock(PhClock *clock)
{
	_videoClock = clock;
	if(clock) {
		connect(_videoClock, &PhClock::timeChanged, this, &PhSynchronizer::onVideoTimeChanged);
		connect(_videoClock, &PhClock::rateChanged, this, &PhSynchronizer::onVideoRateChanged);
	}
}

void PhSynchronizer::setSyncClock(PhClock *clock, SyncType type)
{
	_syncClock = clock;
	_syncType = type;
	if(_syncClock) {
		connect(_syncClock, &PhClock::timeChanged, this, &PhSynchronizer::onSyncTimeChanged);
		connect(_syncClock, &PhClock::rateChanged, this, &PhSynchronizer::onSyncRateChanged);
	}
}

void PhSynchronizer::onStripTimeChanged(PhTime time)
{
	if(!_settingStripTime) {
		PHDBG(2) << time;
		if(_syncClock) {
			// Apply precise correction.
			// We don't change sony clock because this would desynchronize the sony master.
#warning /// @todo Make the error a settings
			if(qAbs(time - _syncClock->time()) > 1000) {
				PHDEBUG << "correct :" << _stripClock->time() << _syncClock->time();
				_settingStripTime = true;
				_stripClock->setTime(_syncClock->time());
				_settingStripTime = false;
			}
		}

		if(_syncType != Sony) {
			_settingVideoTime = true;
			_videoClock->setTime(time);
			_settingVideoTime = false;
		}
	}
}

void PhSynchronizer::onStripRateChanged(PhRate rate)
{
	if(!_settingStripRate) {
		PHDEBUG << rate;
		if(_syncClock) {
			_settingSonyRate = true;
			_syncClock->setRate(rate);
			_settingSonyRate = false;
		}
		_settingVideoRate = true;
		_videoClock->setRate(rate);
		_settingVideoRate = false;
	}
}

void PhSynchronizer::onVideoTimeChanged(PhTime)
{
//	if(!_settingVideoTime)
//		PHDEBUG << time;
}

void PhSynchronizer::onVideoRateChanged(PhRate)
{
}

void PhSynchronizer::onSyncTimeChanged(PhTime time)
{
	if(!_settingSonyTime) {
		PHDBG(3) << time;
		if(_syncType == Sony) {
			_settingVideoTime = true;
			_videoClock->setTime(time);
			_settingVideoTime = false;
		}
		// We apply correction here only when there is a significant change of sony time.
		// Precise correction occurs in onStripTimeChanged() that is called after
		// on SonyTimeChanged (see VideoStripView::paint()).
		PhTime error = qAbs(time - _stripClock->time());
		if((error > 10000) || ((_stripClock->rate() == 0) && (error > 0))) {
			PHDEBUG << "correct error:" << time << _stripClock->time();
			_settingStripTime = true;
			_stripClock->setTime(time);
			_settingStripTime = false;
		}
	}
}

void PhSynchronizer::onSyncRateChanged(PhRate rate)
{
	if(!_settingSonyRate) {
		PHDEBUG << rate;
		_settingStripRate = true;
		_stripClock->setRate(rate);
		_settingStripRate = false;
		_settingVideoRate = true;
		_videoClock->setRate(rate);
		_settingVideoTime = false;
	}
}
