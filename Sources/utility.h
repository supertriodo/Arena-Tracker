#ifndef UTILITY_H
#define UTILITY_H

#include <QString>

enum DebugLevel { Normal, Warning, Error};

class Utility
{
public:
    Utility();
    ~Utility();

    static QString heroToLogNumber(const QString &hero);
};

#endif // UTILITY_H
