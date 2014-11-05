#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

#ifdef QT_NO_DEBUG

typedef QApplication Application;

#else

#include "common.h"

class Application : public QApplication
{
	Q_OBJECT
public:
	explicit Application(int &argc, char **argv);

	virtual bool notify(QObject *receiver, QEvent *event) OVERRIDE;

signals:

public slots:

};

#endif // QT_NO_DEBUG

#endif // APPLICATION_H
