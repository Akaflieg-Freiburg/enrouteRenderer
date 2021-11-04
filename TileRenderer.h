#pragma once

#include <QObject>

class TileRenderer : public QObject
{
    Q_OBJECT

public:
    explicit TileRenderer(QObject* parent = nullptr);

    QImage render(int zoom, int column, int row);

private:
    Q_DISABLE_COPY_MOVE(TileRenderer)
};


