/**
 * @file
 * @copyright (C) 2012-2014 Phonations
 * @license http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>

#include "SonyVideoStripSynchronizer.h"

namespace Ui {
class PreferencesDialog;
}
/**
 * @brief Joker preferences dialog
 *
 * The dialog contains the following user settings:
 * - General HUD: enable/disable element like current timecode
 * - Synchronization: delay, sync mode, sync mode configuration
 * - PhGraphicStrip setting: boldness, speed, font, height
 * - Log
 */
class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @brief The PreferencesDialog constructor
	 *
	 * @param settings The application settings
	 * @param parent The parent object
	 */
	explicit PreferencesDialog(QSettings *settings, QWidget *parent = 0);
	~PreferencesDialog();

private slots:

	void on_spinBoxDelay_valueChanged(int delay);
	void on_radioButtonQF_toggled(bool checked);
	void on_sliderStripHeight_valueChanged(int position);
	void on_spinBoxSpeed_valueChanged(int speed);
	void on_listWidgetFont_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

	void on_cBoxLastFile_toggled(bool checked);

	void on_sliderBoldness_valueChanged(int value);

	void on_lineEditFilter_textEdited(const QString &arg1);

	void on_cBoxDeinterlace_clicked();

	void on_cBoxDisplayTC_clicked();

	void on_cBoxDisplayNextTC_clicked();

	void on_cBoxDisplayNextText_clicked();

	void on_cBoxDisplayTitle_clicked();

	void on_cBoxDisplayLoop_clicked();

	void on_pButtonReset_clicked();

	void on_lblPathToLogFile_linkActivated(const QString &link);

	void onLogMaskButtonClicked();

	void on_listWidgetSync_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void on_listWidgetInputs_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
	Ui::PreferencesDialog *ui;
	QSettings *_settings;
	QString _oldFont;
	bool _oldUseQuarterFrame;
	int _oldDelay;
	int _oldSpeed;
	int _oldBolness;
	float _oldStripHeight;
	bool _oldOpenLastFile;
	bool _oldDeinterlace;
	bool _oldDisplayTC;
	bool _oldDisplayNextTC;
	bool _oldDisplayNextText;
	bool _oldDisplayTitle;
	bool _oldDisplayLoop;
	int _oldLogMask;
	int _oldSyncProtocol;
	QString _oldLTCInput;

	QMap<QString, QString> fontList;

	void showParamLTC(bool show);
	void showParamSony(bool show);
};

#endif // PREFERENCESDIALOG_H
