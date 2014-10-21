#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QTextEdit>

#include <PhTools/PhDebug.h>

#include "TextEditTestWindow.h"
#include "ui_TextEditTestWindow.h"

TextEditTestWindow::TextEditTestWindow(TextEditTestSettings *settings) :
	PhEditableDocumentWindow(settings),
	ui(new Ui::TextEditTestWindow),
	_settings(settings),
	_isModified(false)
{
	ui->setupUi(this);

	connect(ui->textEdit, &QTextEdit::textChanged, this, &TextEditTestWindow::onTextChanged);
}

TextEditTestWindow::~TextEditTestWindow()
{
	delete ui;
}

bool TextEditTestWindow::openDocument(const QString &fileName)
{
	PHDEBUG << fileName;
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return false;

	QTextStream ts(&file);
	ui->textEdit->setText(ts.readAll());
	QTextCursor cursor = ui->textEdit->textCursor();
	cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	ui->textEdit->setTextCursor(cursor);

	file.close();

	_isModified = false;

	return PhEditableDocumentWindow::openDocument(fileName);
}

bool TextEditTestWindow::saveDocument(const QString &fileName)
{
	PHDEBUG << fileName;
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Text))
		return false;

	QTextStream ts(&file);
	PHDEBUG << ui->textEdit->toPlainText();
	ts << ui->textEdit->toPlainText();

	file.close();

	_isModified = false;

	return PhEditableDocumentWindow::saveDocument(fileName);
}

QMenu *TextEditTestWindow::recentDocumentMenu()
{
	return ui->menuOpen_recent;
}

QAction *TextEditTestWindow::fullScreenAction()
{
	return NULL;
}

bool TextEditTestWindow::isDocumentModified()
{
	return _isModified;
}

void TextEditTestWindow::on_actionNew_triggered()
{
	PHDEBUG;
	if(checkDocumentModification()) {
		ui->textEdit->clear();
		_isModified = false;

		resetDocument();
	}
}

void TextEditTestWindow::on_actionOpen_triggered()
{
	PHDEBUG << _settings->lastDocumentFolder();
	if(checkDocumentModification()) {
		QString fileName = QFileDialog::getOpenFileName(this, "Open a text file...", _settings->lastDocumentFolder(), "Text file (*.txt)");

		if(QFile::exists(fileName)) {
			if(!openDocument(fileName))
				QMessageBox::critical(this, "", tr("Unable to open ") + fileName);
		}
	}
}

void TextEditTestWindow::on_actionSave_triggered()
{
	PHDEBUG << _settings->currentDocument();
	if(QFile(_settings->currentDocument()).exists())
		saveDocument(_settings->currentDocument());
	else
		on_actionSave_as_triggered();
}

void TextEditTestWindow::on_actionSave_as_triggered()
{
	QString fileName = _settings->currentDocument();
	if(fileName.isEmpty())
		fileName = _settings->lastDocumentFolder();

	fileName = QFileDialog::getSaveFileName(this, "Save a text file...", fileName, "Text file (*.txt)");
	PHDEBUG << fileName;
	if(!fileName.isEmpty()) {
		if(!saveDocument(fileName))
			QMessageBox::critical(this, "Error", "Unable to save " + fileName);
	}
}

void TextEditTestWindow::onTextChanged()
{
	_isModified = true;
}
