#ifndef DEPART_H
#define DEPART_H
#include <QString>


class Depart
{
public:
    Depart();
    int id;
    int parentId;
    int level;
    QString dname;
};

#endif // DEPART_H
