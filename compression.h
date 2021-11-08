
#pragma once

#include <QByteArray>


/** This namespace gathers static methods for data compression and decompression. */

namespace compression
{
  /** Compression strategy

      The zlib compression library allows to tune the compression algorithm, to
      achieve better compression rates depending on the type of input data.
   */
  
  enum zlibCompressionStrategy {
    /** The default strategy is best for general data */
    standard,

    /** Strategy for data thar consists mostly of small values with a somewhat
	random distribution
	
	This compression algorithm works well for images whose data has been
	filtered.
    */
    filtered
  };

  /** Compresses a QByteArray using zlib

      Compresses the contents of a QByteArray using zlib.  In contrast to Qt's
      method "qCompress", the output of this method is a valid zLib data stream.

      @param src QByteArray holding uncompressed data

      @param strategy Strategy used by zlib in the compression

      @returns A QByteArray containing the compressed data, or an empty
      QByteArray in case of error.
  */
  QByteArray zlibCompress(QByteArray &src, zlibCompressionStrategy strategy=standard);

  /** Uncompresses a QByteArray using zlib

      Compresses the contents of a QByteArray using zlib.
      
      @warning The zlib data stream does not contain any information concerning
      the size of the uncompressed data. If the size of the uncompressed data is
      known, this information should be passed on using the parameter
      'estimatedOutputSize'. If no estimated output size the method reserves a
      buffer that is four times the input size and doubles the buffer size
      whenever uncompression fails with 'insufficient buffer size'. This
      trial-and-error approach is, however, far from optimal and can lead to
      substantial slowdown.

      @param src QByteArray holding compressed data

      @param estimatedOutputSize Expected size of the uncompressed data, in
      bytes.

      @returns A QByteArray containing the uncompressed data, or an empty
      QByteArray in case of error.
  */
  QByteArray zlibUncompress(const QByteArray &src, quint64 estimatedOutputSize=0);

#warning
  QByteArray gzUncompress(QByteArray &src, quint64 estimatedOutputSize=0);
};


