#ifndef PHSYNCSETTINGS_H
#define PHSYNCSETTINGS_H

#include <QString>

/**
 * @brief The settings for a PhSonyController
 */
class PhSyncSettings {
public:
	/**
	 * @brief Shall the PhSonyController sync on a CTS up front or down front
	 * @return True for up front, false for down front
	 */
	virtual bool videoSyncUp() = 0;
	/**
	 * @brief Sony device id most significant byte
	 * @return A 8 bit value
	 */
	virtual unsigned char sonyDevice1() = 0;
	/**
	 * @brief Sony device id less significant byte
	 * @return A 8 bit value
	 */
	virtual unsigned char sonyDevice2() = 0;

	/**
	 * @brief Sony device fast rate
	 * @return A float value
	 */
	virtual float sonyFastRate() = 0;

	/**
	 * @brief The slave FTDI device number
	 * @return An integer
	 */
	virtual int slaveDeviceNumber() = 0;

	/**
	 * @brief The master FTDI device number
	 * @return An integer
	 */
	virtual int masterDeviceNumber() = 0;
};

#endif // PHSYNCSETTINGS_H
