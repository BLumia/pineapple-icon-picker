#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

class QStandardItem;
class QStandardItemModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool loadFile(QString filePath);

    static QByteArray svgFromSymbolContent(QString & viewBox, QByteArray & content);
    static QByteArray svgFromPath(QSize size, QString & path);
    static QStandardItem * createItem(QByteArray & svgContent, QString & iconName, bool downwardYAxis = false);

private slots:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *m_model;
};
#endif // MAINWINDOW_H
