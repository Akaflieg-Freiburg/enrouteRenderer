
#include "zlib.h"

#include "compression.h"
#include <QDebug>

auto compression::zlibCompress(QByteArray &src, zlibCompressionStrategy strategy) -> QByteArray
{
    // Destination buffer, must be at least (1.01X + 12) bytes as large as source.. we made it 1.1X + 12bytes
    uLongf sizeDataCompressed  = (src.size() * 1.1) + 12;
    QByteArray compressed(sizeDataCompressed, 0);


    uLong sourceLen = src.size();

    z_stream stream;
    stream.zalloc = nullptr;
    stream.zfree  = nullptr;
    stream.opaque = nullptr;

    const uInt max = static_cast<uInt>(-1);
    uLong left = sizeDataCompressed;

    int err = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, 15, 8, strategy == filtered ? Z_FILTERED : Z_DEFAULT_STRATEGY);
    if (err != Z_OK) {
        return QByteArray();
    }

    stream.next_out  = reinterpret_cast<Bytef*>(compressed.data());
    stream.avail_out = 0;
    stream.next_in   = reinterpret_cast<z_const Bytef *>(src.data());
    stream.avail_in  = 0;

    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong)max ? max : static_cast<uInt>(left);
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = sourceLen > (uLong)max ? max : (uInt)sourceLen;
            sourceLen -= stream.avail_in;
        }
        err = deflate(&stream, sourceLen ? Z_NO_FLUSH : Z_FINISH);
    } while (err == Z_OK);
    deflateEnd(&stream);


    if (err != Z_STREAM_END) {
        return QByteArray();
    }

    // The variable 'sizeDataCompressed' now contains the size of the compressed data
    // in bytes.
    compressed.truncate(stream.total_out);

    return compressed;
}


auto compression::zlibUncompress(const QByteArray& src, quint64 _estimatedOutputSize) -> QByteArray
{
    QByteArray result;

    uLongf estimatedResultSize;
    if (_estimatedOutputSize > 0) {
        estimatedResultSize = _estimatedOutputSize;
    } else {
        estimatedResultSize = 2*src.size()+1000;
    }

    int z_result = 0;
    do{
        estimatedResultSize = estimatedResultSize*2;
        result.clear();
        result.resize(estimatedResultSize);
        z_result = uncompress((Bytef *)result.data(), (uLongf *)&estimatedResultSize, (const Bytef *)src.data(), src.size());
        qWarning() << z_result;
    }while(z_result == Z_BUF_ERROR);


    if (z_result != Z_OK) {
        return QByteArray();
    }

    result.truncate(estimatedResultSize);
    return result;
}


QByteArray compression::gzUncompress(QByteArray& src, quint64 estimatedOutputSize)
{
    QByteArray output;

    z_stream inflate_s;
    inflate_s.zalloc = Z_NULL;
    inflate_s.zfree = Z_NULL;
    inflate_s.opaque = Z_NULL;
    inflate_s.avail_in = 0;
    inflate_s.next_in = Z_NULL;

    // The windowBits parameter is the base two logarithm of the window size (the size of the history buffer).
    // It should be in the range 8..15 for this version of the library.
    // Larger values of this parameter result in better compression at the expense of memory usage.
    // This range of values also changes the decoding type:
    //  -8 to -15 for raw deflate
    //  8 to 15 for zlib
    // (8 to 15) + 16 for gzip
    // (8 to 15) + 32 to automatically detect gzip/zlib header
    constexpr int window_bits = 15 + 32; // auto with windowbits of 15

    if (inflateInit2(&inflate_s, window_bits) != Z_OK) {
        return {};
    }

    inflate_s.next_in = reinterpret_cast<z_const Bytef*>(src.data());
    inflate_s.avail_in = static_cast<unsigned int>(src.size());
    std::size_t size_uncompressed = 0;
    do {
        output.resize(size_uncompressed + 2*src.size());
        inflate_s.avail_out = static_cast<unsigned int>(2*src.size());
        inflate_s.next_out = reinterpret_cast<Bytef*>(output.data() + size_uncompressed);
        auto ret = inflate(&inflate_s, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR)  {
            inflateEnd(&inflate_s);
            return {};
        }
        size_uncompressed += (2*src.size() - inflate_s.avail_out);
    } while (inflate_s.avail_out == 0);
    inflateEnd(&inflate_s);
    output.resize(size_uncompressed);
    output.squeeze();

    return output;
}
