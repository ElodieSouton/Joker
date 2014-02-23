#ifndef PHDOCUMENTWINDOW_H
#define PHDOCUMENTWINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "PhDocumentWindowSettings.h"

/**
 * @brief Common window behaviour for application handling document
 *
 * This class provide common user interface mechanism for application
 * handling document:
 * -
 */
class PhDocumentWindow : public QMainWindow
{
	Q_OBJECT
public:
	/**
	 * @brief PhDocumentWindow constructor
	 * @param settings The application settings
	 * @param parent The parent object
	 */
	explicit PhDocumentWindow(PhDocumentWindowSettings *settings, QWidget *parent = 0);

#warning TODO move to PhApplication
	/**
	 * @brief Process the application argument
	 * @param argc The arguments count
	 * @param argv The arguments values
	 */
	void processArg(int argc, char *argv[]);
protected:
#warning TODO rename to openDocument
	/**
	 * @brief Open a document
	 * @param fileName The document file name
	 * @return True if success, false otherwise
	 */
	virtual bool openFile(QString fileName) = 0;

	/**
	 * @brief Set the current document
	 *
	 * Update the current document settings, the windows title and the recent file list.
	 *
	 * @param fileName The document file name
	 */
	void setCurrentDocument(QString fileName);

	/**
	 * @brief The recent document menu item
	 *
	 * In order to make the recent document history work,
	 * the child window must implement this method to give
	 * the UI menu element to PhDocumentWindow.
	 *
	 * @return A menu item
	 */
	virtual QMenu *recentDocumentMenu() = 0;

private slots:
	void onOpenRecentDocumentTriggered();

private:
	void updateRecentDocumentMenu();
	PhDocumentWindowSettings * _settings;
};

#endif // PHDOCUMENTWINDOW_H
