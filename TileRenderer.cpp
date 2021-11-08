
#include "compression.h"
#include "TileRenderer.h"

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QSqlDatabase>
#include <QSqlQuery>


TileRenderer::TileRenderer(QObject* parent)
    : QObject(parent)
{

}



QImage TileRenderer::render(int zoom, int column, int row)
{
    //QString fileName("/home/kebekus/Austausch/ICAO+VFR+DE+21Q1.mbtiles");
    //QString fileName("/home/kebekus/Austausch/ed_256.mbtiles");
    QString fileName("/home/kebekus/Austausch/aviation_maps/Europe/Germany.mbtiles");

    auto db = QSqlDatabase::database(fileName);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", fileName);
        db.setDatabaseName(fileName);
        db.open();
        if (db.isOpenError()) {
            qWarning() << "Unable to open database";
            return {};
        }
    }

    auto rowFlipped = (1 << zoom)-1-row;
    QString queryString = QString("select zoom_level, tile_column, tile_row, tile_data from tiles where zoom_level=%1 and tile_column=%2 and tile_row=%3;").arg(zoom).arg(column).arg(rowFlipped);
    QSqlQuery query(db);
    query.exec(queryString);
    if (!query.first()) {
        return {};
    }
    QByteArray tileData = query.value(3).toByteArray();
    if (tileData.isEmpty()) {
        return {};
    }

    // Check if data is image. In that case, our job is easy.
    auto image = QImage::fromData(tileData, "PNG");
    if (!image.isNull()) {
        return image;
    }

    auto uncompressedData = compression::gzUncompress(tileData);

    QFile file("/home/kebekus/experiment/t.data");
    file.open(QIODeviceBase::WriteOnly);
    file.write(tileData);
    file.close();

    return {};
}
