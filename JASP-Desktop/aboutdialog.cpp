//
// Copyright (C) 2013-2016 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QFile>
#include <QTextstream>
#include <QMessageBox>
#include "lrnam.h"
#include "tempfiles.h"
#include "qutils.h"
#include <QWebFrame>
#include <QWebHistory>
#include "appinfo.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{	
	ui->setupUi(this);

	//Disable maximize option dialog
	setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint /* | Qt::WindowMaximizeButtonHint */ | Qt::CustomizeWindowHint);

	ui->aboutWebView->setUrl((QUrl(QString("qrc:///core/about.html"))));
	connect(ui->aboutWebView, SIGNAL(loadFinished(bool)), this, SLOT(aboutPageLoaded(bool)));

 }

AboutDialog::~AboutDialog()
{
	delete ui;
}

void AboutDialog::on_buttonBox_clicked(QAbstractButton *button)
{
	this->close();
}

void AboutDialog::aboutPageLoaded(bool success)
{

	// File aboutWebView with about.htm
	if (success)
	{
		QString version = tq(AppInfo::version.asString());
		ui->aboutWebView->page()->mainFrame()->evaluateJavaScript("window.setAppVersion('" + version + "')");
		ui->aboutWebView->page()->mainFrame()->evaluateJavaScript("window.setAppBuildDate()");
		ui->aboutWebView->page()->mainFrame()->evaluateJavaScript("window.noInstructions()");
		QString html = ui->aboutWebView->page()->mainFrame()->toHtml();
		ui->label->setText(html);
		ui->aboutWebView->page()->mainFrame()->evaluateJavaScript("window.noPatchinfo()");
		html = ui->aboutWebView->page()->mainFrame()->toHtml();
		ui->textBrowser->setText(html);
	}

}
