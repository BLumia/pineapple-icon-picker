#include "iconmodel.h"

#include <QMimeData>
#include <QTemporaryFile>
#include <QUrl>
#include <QDebug>

IconModel::IconModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

QMimeData *IconModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty()) return nullptr;
    QByteArray svgContent(indexes.at(0).data(Qt::UserRole + 1).toByteArray());

    QTemporaryFile file("pineapple-icon-picker.XXXXXX.svg");
    file.open();
    file.write(svgContent);
    file.close();
    file.setAutoRemove(false);

    qDebug() << file.fileName();

    QMimeData * mimeData = new QMimeData();
    mimeData->setData("image/svg+xml", svgContent);
    mimeData->setData("application/octet-stream", svgContent);
    mimeData->setUrls({ QUrl::fromLocalFile(file.fileName()) });
    return mimeData;
}

Qt::DropActions IconModel::supportedDropActions() const
{
    return Qt::CopyAction;
}
