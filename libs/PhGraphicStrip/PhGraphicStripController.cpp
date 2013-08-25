#include "PhGraphicStripController.h"


PhGraphicStripController::PhGraphicStripController() : PhGraphicController()
{

	qDebug() << "PhGraphicStripController::PhGraphicStripController()";
	 _doc = new PhStripDoc();
 #if defined(Q_OS_MAC)
        _settingsPath = QDir::homePath() + "/Library/Preferences/com.phonations.Joker.plist";
         _settingsFormat = QSettings::NativeFormat;
 #elif defined(Q_OS_UNIX)
         _settingsPath = QDir::homePath() + "StripTest.ini"; //TODO : change to specific setting folder
         _settingsFormat = QSettings::IniFormat;
 #elif defined(Q_OS_WIN)//TODO handle settings under Windows and trigger compilation error
 #else
     //TODO trigger compilation error cf rtm
 #endif
         loadSettings();
}


PhStripDoc PhGraphicStripController::getDoc()
{
	qDebug() <<  "PhGraphicStripController::getDoc()";
	return * _doc;
}

void PhGraphicStripController::loadSettings()
{
        // Try to load a settings file (temp)

        if(QFile( _settingsPath).exists())
        {
            _settings = new QSettings(_settingsPath, _settingsFormat);
            qDebug() << "Settings loaded fine";
            getLastFile();
    }
    else
    {
            qDebug() << "Prefs file is missing, creating new pref file with default values :";

            _settings = new QSettings(_settingsPath, _settingsFormat);

            _settings->setValue("last_file", "");
            _settings->setValue("natural_scroll", true);
    }

        QStringList values = _settings->allKeys();

		foreach (QString value, values)
        {
            qDebug() << value << ":" <<  _settings->value(value).toString();
        }
}


QString PhGraphicStripController::getLastFile()
{
    qDebug() << "lastfile :" << _settings->value("last_file", "").toString();
    return _settings->value("last_file").toString();
}

bool PhGraphicStripController::openDoc(QString fileName)
{
    bool succeed = _doc->openDetX(fileName);
    //if (succeed)
        //emit docChanged();
    return succeed;
}

bool PhGraphicStripController::getNaturalScrollPref()
{
    return _settings->value("natural_scroll", "true").toBool();
}

void PhGraphicStripController::setLastFile(QString filename)
{
    _settings->setValue("last_file", filename);
}


void PhGraphicStripController::onRateChanged2(float rate)
{
}

void PhGraphicStripController::onPositionChanged2(int move)
{
}
