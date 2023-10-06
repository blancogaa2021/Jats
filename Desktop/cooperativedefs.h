#ifndef COOPERATIVEDEFS_H
#define COOPERATIVEDEFS_H

#include "QStringList"

namespace Coop
{

const QStringList & educatorsTier()
{
	// Want to thank an institution on the welcomepage?
	// Just add an entry in the list below:
	static QStringList list =
	{
		"University of Amsterdam",
		"M&S / Faculty of Social Sciences / Utrecht University",
		"Nyenrode Business University",
		"KU Leuven",
		"Tilburg University",
	};

	return list;
}

const QString & howToSupport()
{
	static QString howTo = "https://jasp-stats.org/cooperative-how-to-join/";
	return howTo;
}

const QString & cooperativeUrl()
{
	static QString coop = "https://jasp-stats.org/cooperative-vision-and-goals/";
	return coop;
}

}
#endif // COOPERATIVEDEFS_H
