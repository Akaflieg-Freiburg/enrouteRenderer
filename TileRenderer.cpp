
#include "compression.h"
#include "TileRenderer.h"
#include "vector_tile.pb.h"

#include <QDebug>
#include <QtEndian>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlQuery>


#define GZIP_MAGIC      0x1F8B
#define GZIP_MAGIC_MASK 0xFFFF
#define PBF_MAGIC       0x1A00
#define PBF_MAGIC_MASK  0xFF00

static bool isMagic(quint16 magic, quint16 mask, quint16 value)
{
    return ((qFromBigEndian(value) & mask) == magic);
}

static bool isGZIPPBF(quint16 magic)
{
    return isMagic(GZIP_MAGIC, GZIP_MAGIC_MASK, magic);
}

static bool isPlainPBF(quint16 magic)
{
    return isMagic(PBF_MAGIC, PBF_MAGIC_MASK, magic);
}




TileRenderer::TileRenderer(QObject* parent)
    : QObject(parent)
{

}

QByteArray TileRenderer::getTileData(int zoom, int column, int row)
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
        qWarning() << queryString  << "Query has no result";
        return {};
    }
    return query.value(3).toByteArray();
}


QImage TileRenderer::render(int zoom, int column, int row)
{
    auto tileData = getTileData(zoom, column, row);
    if (tileData.size() < 2) {
        return {};
    }

    // Check if data is gzip compressed. Uncompress if it is
    quint16 magic = *((quint16*)tileData.constData());
    if (isGZIPPBF(magic)) {
        tileData = compression::gzUncompress(tileData);
        magic = *((quint16*)tileData.constData());
    }

    // Check if data is PBF.
    if (isPlainPBF(magic)) {
        return renderTile(tileData);
    }

    // If data is not PBF, we assume that it is a PNG image
    return QImage::fromData(tileData, "PNG");
}


void renderTransportation(QPainter* painter, vector_tile::Tile_Layer& pbfLayer)
{
    qWarning() << "Render Transportation";

    for (int i = 0; i < pbfLayer.features().size(); i++) {
        auto feature = pbfLayer.features(i);
        qWarning() << QString::fromStdString(feature.DebugString());
    }
}


QImage TileRenderer::renderTile(QByteArray PBFdata)
{
    qWarning() << "Render PBF tile";

    QImage image(imageSize, imageSize, QImage::Format_RGB32);
    image.fill(Qt::white);


    // Parse protobuffer
    vector_tile::Tile data;
    if (!data.ParseFromArray(PBFdata.constData(), PBFdata.size())) {
        qCritical() << "Invalid PBF data";
        return {};
    }

    // Go through layers
    QPainter paint(&image);
    for(int i=0; i< data.layers_size(); i++) {
        auto layer = data.layers(i);
        auto name = QString::fromStdString(layer.name());
        if (name == "transportation") {
            renderTransportation(&paint, layer);
        }
    }
    paint.end();

    return image;
}
