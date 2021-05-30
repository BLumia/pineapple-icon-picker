#pragma once

#include <QStandardItemModel>

class IconModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit IconModel(QObject *parent = nullptr);

protected:
    QMimeData * mimeData(const QModelIndexList &indexes) const override;
    Qt::DropActions supportedDropActions() const override;

private:
};

