#include "messageforwarder.h"
#include "utilities/desktopcommunicator.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QRegularExpression>
#include <QString>
#include "qutils.h"
#include "appdirs.h"
#include "log.h"

MessageForwarder::MessageForwarder(QObject *main) : QQuickItem(nullptr)
{
	if(_singleton != nullptr)
		throw std::runtime_error("There can be only ONE MessageForwarder!");

	_singleton = this;

	setParent(main);
}

void MessageForwarder::log(QString msg)
{
	Log::log() << msg << std::endl;
}

bool MessageForwarder::useNativeFileDialogs()
{
	return DesktopCommunicator::singleton()->useNativeFileDialog();
}

MessageForwarder * MessageForwarder::_singleton = nullptr;

void MessageForwarder::showWarning(QString title, QString message)
{
	QMessageBox box;
	box.setText(title);
	box.setInformativeText(message);
	box.setIcon(QMessageBox::Warning);
}

bool MessageForwarder::showYesNo(QString title, QString message, QString YesButtonText, QString NoButtonText)
{
	if(YesButtonText == "")		YesButtonText	= tr("Yes");
	if(NoButtonText == "")		NoButtonText	= tr("No");

	QMessageBox box;

	box.setText(title);
	box.setInformativeText(message);
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setIcon(QMessageBox::Question);

	box.setButtonText(QMessageBox::Yes,		YesButtonText);
	box.setButtonText(QMessageBox::No,		NoButtonText);
	return box.exec() == QMessageBox::Yes;
}

MessageForwarder::DialogResponse MessageForwarder::showYesNoCancel(QString title, QString message, QString YesButtonText, QString NoButtonText, QString CancelButtonText)
{
	if(YesButtonText == "")		YesButtonText		= tr("Yes");
	if(NoButtonText == "")		NoButtonText		= tr("No");
	if(CancelButtonText == "")	CancelButtonText	= tr("Cancel");

	QMessageBox box;

	box.setText(title);
	box.setInformativeText(message);
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	box.setIcon(QMessageBox::Question);

	box.setButtonText(QMessageBox::Yes,		YesButtonText);
	box.setButtonText(QMessageBox::No,		NoButtonText);
	box.setButtonText(QMessageBox::Cancel,	CancelButtonText);

	switch(box.exec())
	{
	case QMessageBox::Yes:	return DialogResponse::Yes;
	case QMessageBox::No:	return DialogResponse::No;
	default:				return DialogResponse::Cancel;
	}
}

MessageForwarder::DialogResponse MessageForwarder::showSaveDiscardCancel(QString title, QString message, QString saveText, QString discardText, QString cancelText)
{
	QMessageBox box;

	box.setText(title);
	box.setInformativeText(message);
	box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	box.setIcon(QMessageBox::Question);

	if(saveText == "")		saveText		= tr("Save");
	if(discardText == "")	discardText = tr("Don't Save");
	if(cancelText == "")	cancelText	= tr("Cancel");

	box.setButtonText(QMessageBox::Save,		saveText);
	box.setButtonText(QMessageBox::Discard,		discardText);
	box.setButtonText(QMessageBox::Cancel,		cancelText);

	switch(box.exec())
	{
	case QMessageBox::Save:			return DialogResponse::Save;
	case QMessageBox::Discard:		return DialogResponse::Discard;
	default:						return DialogResponse::Cancel;
	}
}

QString MessageForwarder::askPassword(QString title, QString message)
{
//	here we can open a nice QInputDialog with a password field etc (modally)
	return QInputDialog::getText(nullptr, title, message, QLineEdit::Password);
}

QString MessageForwarder::browseOpenFile(QString caption, QString browsePath, QString filter)
{
	if(useNativeFileDialogs())	return 	QFileDialog::getOpenFileName(nullptr, caption, browsePath, filter);
	else						return 	QFileDialog::getOpenFileName(nullptr, caption, browsePath, filter, nullptr, QFileDialog::DontUseNativeDialog);
}

QString MessageForwarder::browseOpenFileDocuments(QString caption, QString filter)
{
	return browseOpenFile(caption, AppDirs::documents(), filter);
}

QString MessageForwarder::browseSaveFileDocuments(QString caption, QString filter)
{
	return browseSaveFile(caption, AppDirs::documents(), filter);
}

QString MessageForwarder::browseSaveFile(QString caption, QString browsePath, QString filter, QString * selectedExtension)
{
	Log::log() << "MessageForwarder::browseSaveFile(\"" << caption.toStdString() << "\", \"" << browsePath.toStdString() << "\", \"" << filter.toStdString() << "\")" << std::endl;

	QString saveFileName, selectedFilter;

	if(useNativeFileDialogs())	saveFileName = 	QFileDialog::getSaveFileName(nullptr, caption, browsePath, filter, &selectedFilter);
	else						saveFileName = 	QFileDialog::getSaveFileName(nullptr, caption, browsePath, filter, &selectedFilter, QFileDialog::DontUseNativeDialog);

	Log::log() << "Selected save file: " << saveFileName << " and selected filter: " << selectedFilter << std::endl;

	//Lets make sure the extension is added:
	const QRegularExpression extReg("\\*\\.(\\w+)");
	QRegularExpressionMatch  possibleMatch = extReg.match(selectedFilter);

	if(possibleMatch.hasMatch())
	{
		QString ext = possibleMatch.captured(1);

		if(!saveFileName.endsWith(ext))
			saveFileName += "." + ext;

		if(selectedExtension)
			*selectedExtension = ext;
	}
	else if(selectedExtension)//So the filter doesnt tell us the extension but the caller expects to know what is what
	{
		if(saveFileName.lastIndexOf('.') >= 0)	*selectedExtension = saveFileName.mid(saveFileName.lastIndexOf('.') + 1);
		else									*selectedExtension = ""; //???
	}

	if(selectedExtension)
		Log::log() << "Selected extension: '" << *selectedExtension << "'" << std::endl;

	return saveFileName;
}

QString MessageForwarder::browseOpenFolder(QString caption, QString browsePath)
{
	if(useNativeFileDialogs())	return QFileDialog::getExistingDirectory(nullptr, caption, browsePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	else						return QFileDialog::getExistingDirectory(nullptr, caption, browsePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog);
}

QString MessageForwarder::browseOpenFolder(QString caption)
{
	return browseOpenFolder(caption, AppDirs::documents());
}
