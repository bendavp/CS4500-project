#pragma once

// lang::CwC

#include "string.h"
#include "helper.h"
#include <iostream>
#include "array.h"

using namespace std;

/**
 * @brief A Map of Object type to primitive type. Map does not own its contents.
 * 
 * @tparam T non-primitive Object or subtype thereof
 * @tparam U primitive type (int, float, bool, size_t,  etc.)
 * 
 * @author BK and AT
 */
template <class T, class U>
class Map : public Object
{
public:
  T *key_arr_;
  U *val_arr_;
  size_t size_;
  size_t memory_size_;

  Map<T, U>() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new T[memory_size_];
    for (size_t i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new U[memory_size_];
    size_ = 0;
  }

  Map<T, U>(Map<T, U> &map) : Object()
  {
    memory_size_ = map.memory_size_;
    key_arr_ = map.key_arr_;
    val_arr_ = map.val_arr_;
    size_ = map.size_;
  }

  ~Map()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    T *tempKeys = new T[memory_size_];
    for (size_t i = 0; i < memory_size_; i++)
      tempKeys[i] = nullptr;
    U *tempVals = new U[memory_size_];

    size_t hash_index;
    for (size_t i = 0; i < size_; i++)
    {
      hash_index = key_arr_[i]->hash() % memory_size_;
      for (size_t j = hash_index; j < memory_size_; j++)
      {
        if (tempKeys[j] == nullptr)
        {
          tempKeys[j] = key_arr_[i];
          tempVals[j] = val_arr_[i];
          break;
        }

        if (hash_index != 0 && j == memory_size_ - 1)
          j = 0;
      }
    }

    delete key_arr_;
    delete val_arr_;

    key_arr_ = tempKeys;
    val_arr_ = tempVals;
  }

  /**
   * Returns the value which was set for this key.
   * Returns nullptr if not in map.
   */
  U get(T key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
        return val_arr_[i];

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return NULL;
    }

    return NULL;
  }

  /**
   * Set the value at the given key in this map.
   */
  void add(T key, U value)
  {
    assert(key != nullptr);
    assert(!has(key));

    if (size_ * 2 == memory_size_)
      incrementSize();

    size_t hash_index = key->hash() % memory_size_;
    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] == nullptr)
      {
        key_arr_[i] = key;
        val_arr_[i] = value;
        size_++;
        return;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;
    }
  }

  void set(T key, U value)
  {
    assert(key != nullptr);
    assert(value != nullptr);

    size_t hash_index = key->hash() % memory_size_;
    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        val_arr_[i] = value;
        return;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;
    }
  }

  /**
   * Determine if the given key is in this map.
   */
  bool has(T key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
        return true;

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return false;
    }

    return false;
  }

  /**
   * Return the number of entries in this map.
   */
  size_t size() { return size_; }
};

/**  Item_ are entries in a Map, they are not exposed, are immutable, own
 *   they key, but the value is external.  author: jv */
class Items_
{
public:
  Array keys_;
  Array vals_;

  Items_() : keys_(8), vals_(8) {}

  Items_(Object *k, Object *v) : keys_(8), vals_(8)
  {
    keys_.push_back(k);
    vals_.push_back(v);
  }

  bool contains_(Object &k)
  {
    for (size_t i = 0; i < keys_.length(); i++)
      if (k.equals(keys_.get_(i)))
        return true;
    return false;
  }

  Object *get_(Object &k)
  {
    for (size_t i = 0; i < keys_.length(); i++)
      if (k.equals(keys_.get_(i)))
        return vals_.get_(i);
    return nullptr;
  }

  size_t set_(Object &k, Object *v)
  {
    for (size_t i = 0; i < keys_.length(); i++)
      if (k.equals(keys_.get_(i)))
      {
        vals_.put(i, v);
        return 0;
      }
    // The keys are owned, but the key is received as a reference, i.e. not owned so we must make a copy of it.
    keys_.push_back(k.clone());
    vals_.push_back(v);
    return 1;
  }

  size_t erase_(Object &k)
  {
    for (size_t i = 0; i < keys_.length(); i++)
      if (k.equals(keys_.get_(i)))
      {
        keys_.erase_(i);
        vals_.erase_(i);
        return 1;
      }
    return 0;
  }
};

/** A generic map class from Object to Object. Subclasses are responsibly of
 * making the types more specific.  author: jv */
class JVMap : public Object
{
public:
  size_t capacity_;
  // TODO this was not size of the map, but number of occupied item positions in the top level
  size_t size_ = 0;
  Items_ *items_; // owned

  JVMap() : JVMap(10) {}
  JVMap(size_t cap)
  {
    capacity_ = cap;
    items_ = new Items_[capacity_];
  }

  ~JVMap() { delete[] items_; }

  /** True if the key is in the map. */
  bool contains(Object &key) { return items_[off_(key)].contains_(key); }

  /** Return the number of elements in the map. */
  size_t size()
  {
    return size_;
  }

  size_t off_(Object &k) { return k.hash() % capacity_; }

  /** Get the value.  nullptr is allowed as a value.  */
  Object *get_(Object &key) { return items_[off_(key)].get_(key); }

  /** Add item->val_ at item->key_ either by updating an existing Item_ or
   * creating a new one if not found.  */
  void set(Object &k, Object *v)
  {
    if (size_ >= capacity_)
      grow();
    size_ += items_[off_(k)].set_(k, v);
  }

  /** Removes element with given key from the map.  Does nothing if the
      key is not present.  */
  void erase(Object &k)
  {
    size_ -= items_[off_(k)].erase_(k);
  }

  /** Resize the map, keeping all Item_s. */
  void grow()
  {
    //LOG("Growing map from capacity " << capacity_);
    JVMap newm(capacity_ * 2);
    for (size_t i = 0; i < capacity_; i++)
    {
      size_t sz = items_[i].keys_.length();
      for (size_t j = 0; j < sz; j++)
      {
        Object *k = items_[i].keys_.get_(j);
        Object *v = items_[i].vals_.get_(j);
        newm.set(*k, v);
        // otherwise the values would get deleted (if the array's destructor was doing its job I found later:)
        items_[i].vals_.put(j, nullptr);
      }
    }
    delete[] items_;
    items_ = newm.items_;
    capacity_ = newm.capacity_;
    assert(size_ == newm.size_);
    newm.items_ = nullptr;
  }
};

/***************************************************************************
 * 
 **********************************************************author:jvitek */
class Num : public Object
{
public:
  size_t v = 0;
  Num() {}
  Num(size_t v) : v(v) {}
};

class SIMap : public JVMap
{
public:
  SIMap() {}
  Num *get(String &key) { return dynamic_cast<Num *>(get_(key)); }
  void set(String &k, Num *v)
  {
    assert(v);
    SIMap::set(k, v);
  }
};