#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QSvgRenderer>
#include <QDebug>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->listView->setModel(m_model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadFile(QString filePath)
{
    m_model->clear();

    QList<QStandardItem*> iconList;

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        return false;
    }
    QByteArray rawContent(file.readAll());
    file.close();

    if (rawContent.startsWith("!function")) {
        int start = rawContent.indexOf("<svg");
        int end = rawContent.lastIndexOf("/svg>");
        if (start != -1 && end != -1) {
            rawContent = rawContent.mid(start, end - start + QByteArray("/svg>").size());
        }
    }

    QXmlStreamReader reader(rawContent);
    int glyphUnitsPerEm = 1000;
    int glyphHorizAdvX = 0;
    if (reader.readNextStartElement()) {
        if (reader.name() == "svg") {
            while (reader.readNextStartElement()) {
                qDebug() << "reading" << reader.name();

                const QStringList containUsefulNode {
                    "font", // include <font-face> for <glyph>s
                    "defs", // include a set of <symbol>s or <glyph>s with <font-face>
                };
                if (containUsefulNode.contains(reader.name())) {
                    continue;
                } else if (reader.name() == "font") {
                    QXmlStreamAttributes attrs = reader.attributes();

                    if (attrs.hasAttribute("horiz-adv-x")) {
                        glyphHorizAdvX = attrs.value("horiz-adv-x").toInt();
                    }

                    continue;
                } else if (reader.name() == "font-face") {
                    qDebug() << "font-face";
                    QXmlStreamAttributes attrs = reader.attributes();

                    if (attrs.hasAttribute("units-per-em")) {
                        glyphUnitsPerEm = attrs.value("units-per-em").toInt();
                    }

                    QString s = reader.readElementText();
                } else if (reader.name() == "symbol") {
                    QXmlStreamAttributes attrs = reader.attributes();
                    QString viewBox = attrs.value("viewBox").toString();
                    QString name = attrs.hasAttribute("id") ? attrs.value("id").toString()
                                                            : "id not found";

                    qint64 start = reader.characterOffset();
                    reader.skipCurrentElement();
                    qint64 end = reader.characterOffset();
                    QByteArray rawChild = rawContent.mid(start, end - start - QByteArray("</symbol>").size());

                    QByteArray content = svgFromSymbolContent(viewBox, rawChild);
                    iconList.append(createItem(content, name));
                } else if (reader.name() == "glyph") {
                    QXmlStreamAttributes attrs = reader.attributes();

                    QString name = attrs.hasAttribute("glyph-name") ? attrs.value("glyph-name").toString()
                                                                    : "glyph-name not found";

                    if (!attrs.hasAttribute("d")) {
                        qDebug() << name << "skipping since no attr named 'd'";
                        reader.skipCurrentElement();
                        continue;
                    }

                    int horizAdvX = attrs.hasAttribute("horiz-adv-x") ? attrs.value("horiz-adv-x").toInt()
                                                                    : glyphHorizAdvX;
                    QString path = attrs.value("d").toString();
                    QString s = reader.readElementText();
                    qDebug() << "glyph" << name;

                    QByteArray content = svgFromPath(QSize(horizAdvX, glyphUnitsPerEm), path);
                    iconList.append(createItem(content, name, true));
                } else {
                    qDebug() << "skipping:" << reader.name();
                    reader.skipCurrentElement();
                }
            }
        } else {
            // not a svg file.
            return false;
        }
    }

    m_model->appendColumn(iconList);
}

QByteArray MainWindow::svgFromSymbolContent(QString &viewBox, QByteArray &content)
{
    QString templateSvg(
R"svg(<svg viewBox="%1" xmlns="http://www.w3.org/2000/svg"
        xmlns:xlink="http://www.w3.org/1999/xlink">
     %2
   </svg> )svg");

    return templateSvg.arg(viewBox).arg(QString(content)).toLatin1();
}

QByteArray MainWindow::svgFromPath(QSize size, QString &path)
{
    QString templateSvg(
R"svg(<svg width="%1" height="%2" xmlns="http://www.w3.org/2000/svg"
        xmlns:xlink="http://www.w3.org/1999/xlink">
     <path d="%5"></path>
   </svg> )svg");

    return templateSvg.arg(size.width()).arg(size.height()).arg(path).toLatin1();
}

QStandardItem *MainWindow::createItem(QByteArray & svgContent, QString &iconName, bool downwardYAxis)
{
    QSvgRenderer render(svgContent);
    qDebug() << render.defaultSize();
    render.setAspectRatioMode(Qt::KeepAspectRatio);

    QPixmap img(64, 64);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    render.render(&painter);
    QIcon icon(downwardYAxis ? img.transformed(QTransform::fromScale(1, -1)) : img);

    return new QStandardItem(icon, iconName);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString fileName = urls.first().toLocalFile();
    loadFile(fileName);
}
