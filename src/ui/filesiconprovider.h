#ifndef CUSTOMFILEICONPROVIDER_H
#define CUSTOMFILEICONPROVIDER_H

#include <QFileIconProvider>

class FilesIconProvider : public QFileIconProvider
{
public:
    FilesIconProvider();

public:
    QIcon icon(const QFileInfo &info) const;
};

#endif // CUSTOMFILEICONPROVIDER_H
