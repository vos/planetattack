#include "bihash.h"

#ifndef QT_NO_DEBUG_STREAM
template<typename K, typename V>
QDebug operator<<(QDebug dbg, const BiHash<K, V> &hash) {
    dbg.nospace() << hash.m_hash;
    return dbg.space();
}
#endif
