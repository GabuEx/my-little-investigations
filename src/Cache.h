/**
 * Class for simple MRU (Most Recent Used) cache.
 * It's behavior is like limited size map,
 * where least recent used element erased, if the size exceeded the limit
 *
 * @author mad-mix
 * @since 1.0.7
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <list>

using namespace std;

template <typename KeyType, typename ValueType>
class MRUCache
{
public:
    class ItemHandler
    {
    public:
        virtual void releaseItem(const KeyType &key, const ValueType &value) { }
        virtual ValueType newItem(const KeyType &key) { return ValueType(); }
        virtual ~ItemHandler() {}
    };

    MRUCache(unsigned int maxLength, ItemHandler *itemHandler) : maxLength(maxLength), itemHandler(itemHandler) { }

    ValueType & operator [] (const KeyType &key)
    {
        typedef typename list<ListEntry>::iterator ListIterator;
        typedef typename map<KeyType, ListIterator>::iterator MapIterator;
        MapIterator it = cacheMap.find(key);
        if (it == cacheMap.end())
        {
            ListEntry newEntry = {key, itemHandler->newItem(key)};
            cacheList.push_front(newEntry);
            cacheMap[key] = cacheList.begin();
            if (cacheList.size() > maxLength)
            {
                const ListEntry &entry = cacheList.back();
                itemHandler->releaseItem(entry.key, entry.value);
                cacheMap.erase(entry.key);
                cacheList.pop_back();
            }

            return cacheList.front().value;
        }
        else
        {
            const ListEntry &entry = *(it->second);
            cacheList.push_front(entry);
            cacheList.erase(it->second);
            it->second = cacheList.begin();

            return cacheList.front().value;
        }
    }

    bool contains(const KeyType &key) { return cacheMap.find(key) != cacheMap.end(); }

    void clear()
    {
        typedef typename list<ListEntry>::iterator ListIterator;
        for (ListIterator it = cacheList.begin(); it != cacheList.end(); it++)
        {
            itemHandler->releaseItem(it->key, it->value);
        }
        cacheMap.clear();
        cacheList.clear();
    }

    ~MRUCache()
    {
        clear();
        delete itemHandler;
    }

private:
    const unsigned int maxLength;

    typedef struct
    {
        KeyType key;
        ValueType value;
    } ListEntry;

    list<ListEntry> cacheList;

    typedef typename list<ListEntry>::iterator ListIterator;
    map<KeyType, ListIterator> cacheMap;

    ItemHandler *itemHandler;
};
#endif // CACHE_H
