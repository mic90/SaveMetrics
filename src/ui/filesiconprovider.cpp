#include "filesiconprovider.h"

FilesIconProvider::FilesIconProvider()
{

}

QIcon FilesIconProvider::icon(const QFileInfo &info) const
{
    if(info.isDir())
    {
        return QIcon("://icons/actions/folder.png");
    }

    if(info.suffix() == "lua")
    {
        return QIcon("://icons/script/script_code.png");
    }

    return QFileIconProvider::icon(info);
}

