#ifndef UTILITY_H
#define UTILITY_H

#include <QString>

enum DebugLevel { Normal, Warning, Error };
enum Transparency { Always, Auto, Never };

class Utility
{
public:
    Utility();
    ~Utility();

    static QString heroToLogNumber(const QString &hero);
    static QString appPath();
};

#endif // UTILITY_H
