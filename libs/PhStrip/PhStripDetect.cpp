/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include "PhStripDetect.h"

PhStripDetect::PhStripDetect(PhDetectType type, PhTime timeIn, PhPeople *people, PhTime timeOut, int track)
	: PhStripPeopleObject(timeIn, people, timeOut, track),
	_type(type)
{

}
