#pragma once

#include <QCache>
#include <QObject>
#include <QImage>

#include "TileRenderer.h"

class TileCache : public QObject
{
    Q_OBJECT

public:
    explicit TileCache(QObject* parent = nullptr);

    ~TileCache();

    QImage get(int zoom, int column, int row);

private:
    TileRenderer m_tileRenderer;

    QCache<uint, QImage> imageCache;

    Q_DISABLE_COPY_MOVE(TileCache)
};


