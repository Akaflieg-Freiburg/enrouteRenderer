
#include "TileCache.h"


TileCache::TileCache(QObject* parent)
    : QObject(parent)
{

}

TileCache::~TileCache()
{
    imageCache.clear();
}


QImage TileCache::get(int zoom, int column, int row)
{   
    int x[3] = {zoom, column, row};
    uint hash = qHashBits(x, 3*sizeof(int));

    if (imageCache.contains(hash))
        return *(imageCache[hash]);

    QImage* image = new QImage();
    *image = m_tileRenderer.render(zoom, column, row);
    imageCache.insert(hash, image);

    return *(imageCache[hash]);
}
