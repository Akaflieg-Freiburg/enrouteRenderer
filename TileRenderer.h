#pragma once

#include <QObject>

class TileRenderer : public QObject
{
    Q_OBJECT

public:
    explicit TileRenderer(QObject* parent = nullptr);

    QImage render(int zoom, int column, int row);

    QImage renderTile(QByteArray PBFdata);

    QByteArray getTileData(int zoom, int column, int row);

private:
    constexpr static int imageSize {512};

    Q_DISABLE_COPY_MOVE(TileRenderer)
};


