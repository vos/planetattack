#ifndef BIHASH_H
#define BIHASH_H

#include <QHash>
#include <QDebug>

template<typename K, typename V>
class BiHash
{
public:
    inline void insert(const K &key, const V &value) {
        m_hash.insert(key, value);
        m_reverseHash.insert(value, key);
    }

    inline void removeKey(const K &key) {
        m_reverseHash.remove(m_hash.take(key));
    }

    inline void removeValue(const V &value) {
        m_hash.remove(m_reverseHash.take(value));
    }

    inline const V value(const K &key) const {
        return m_hash.value(key);
    }

    inline const K key(const V &value) const {
        return m_reverseHash.value(value);
    }

    inline bool containsKey(const K &key) const {
        return m_hash.contains(key);
    }

    inline bool containsValue(const V &value) const {
        return m_reverseHash.contains(value);
    }

    inline int size() const {
        return m_hash.size();
    }

    inline bool isEmpty() const {
        return m_hash.isEmpty();
    }

    inline QList<V> values() const {
        return m_reverseHash.keys();
    }

    inline QList<K> keys() const {
        return m_hash.keys();
    }

    inline V takeValue(const K &key) {
        V value = m_hash.take(key);
        m_reverseHash.remove(value);
        return value;
    }

    inline K takeKey(const V &value) {
        K key = m_reverseHash.take(value);
        m_hash.remove(key);
        return key;
    }

    inline void clear() {
        m_hash.clear();
        m_reverseHash.clear();
    }

    BiHash<V, K> reverse() const {
        BiHash<V, K> hash;
        hash.m_hash = m_reverseHash;
        hash.m_reverseHash = m_hash;
        return hash;
    }

    inline QHash<K, V>& hash() { return m_hash; }
    inline QHash<V, K>& reverseHash() { return m_reverseHash; }

private:
    QHash<K, V> m_hash;
    QHash<V, K> m_reverseHash;

    friend class BiHash<V, K>;

    #ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const BiHash<K, V> &hash);
    #endif

};

#endif // BIHASH_H
