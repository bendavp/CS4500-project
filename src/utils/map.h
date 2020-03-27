#pragma once

// lang::CwC

#include "object.h"
#include "string.h"
#include "helper.h"

class ObjectObjectMap : public Object
{
public:
  Object **key_arr_;
  Object **val_arr_;
  size_t size_;
  size_t memory_size_;

  ObjectObjectMap() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new Object *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new Object *[memory_size_];
    size_ = 0;
  }

  ~ObjectObjectMap()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    Object **tempKeys = new Object *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      tempKeys[i] = nullptr;
    }
    Object **tempVals = new Object *[memory_size_];

    size_t hash_index;
    for (size_t i = 0; i < memory_size_ / 2; i++)
    {
      if (key_arr_[i] != nullptr)
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
  Object *get(Object *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return val_arr_[i];
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return nullptr;
    }

    return nullptr;
  }

  /**
   * Set the value at the given key in this map.
   */
  void add(Object *key, Object *value)
  {
    assert(key != nullptr);
    assert(value != nullptr);
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

  void set(Object *key, Object *value)
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
   * Remove the value at the given key in this map. No-op if value not in map.
   */
  void remove(Object *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        key_arr_[i] = nullptr;
        val_arr_[i] = nullptr;
        size_--;
        return;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;
    }
  }

  /**
   * Determine if the given key is in this map.
   */
  bool has(Object *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return true;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return false;
    }

    return false;
  }

  /**
   * Remove all keys from this map.
   */
  void clear()
  {
    delete key_arr_;
    delete val_arr_;

    memory_size_ = 16;
    key_arr_ = new Object *[memory_size_];
    val_arr_ = new Object *[memory_size_];
    size_ = 0;
  }

  /**
   * Return the number of entries in this map.
   */
  size_t size()
  {
    return size_;
  }

  /**
   * Store keys in the given array. Caller responsible for allocating at least
   * Map::size() elements.
   */
  void keys(Object **dest)
  {
    assert(dest != nullptr);

    size_t destIndex = 0;
    for (size_t i = 0; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr)
      {
        dest[destIndex] = key_arr_[i];
        destIndex++;
      }
    }
  }
};

/**
 * A dictionary of string keys and object values.  All keys and values are owned
 * by the caller, and none of the map's methods will modify them.  Keys which
 * are .equals are equal, i.e. the map will never contain two keys which are
 * extensionally equivalent at the same time.
 */
class StringObjectMap : public Object
{
public:
  String **key_arr_;
  Object **val_arr_;
  size_t size_;
  size_t memory_size_;

  StringObjectMap() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new String *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new Object *[memory_size_];
    size_ = 0;
  }

  ~StringObjectMap()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    String **tempKeys = new String *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      tempKeys[i] = nullptr;
    }
    Object **tempVals = new Object *[memory_size_];

    size_t hash_index;
    for (size_t i = 0; i < memory_size_ / 2; i++)
    {
      if (key_arr_[i] != nullptr)
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
  Object *get(String *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return val_arr_[i];
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return nullptr;
    }

    return nullptr;
  }

  /**
   * add the value at the given key in this map.
   */
  void add(String *key, Object *value)
  {
    assert(key != nullptr);
    assert(value != nullptr);
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

  void set(String *key, Object *value)
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
   * Remove the value at the given key in this map. No-op if value not in map.
   */
  void remove(String *key)
  {
    assert(key != nullptr);
    assert(has(key));

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        key_arr_[i] = nullptr;
        val_arr_[i] = nullptr;
        size_--;
        return;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;
    }
  }

  /**
   * Determine if the given key is in this map.
   */
  bool has(String *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return true;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return false;
    }

    return false;
  }

  /**
   * Remove all keys from this map.
   */
  void clear()
  {
    delete key_arr_;
    delete val_arr_;

    memory_size_ = 16;
    key_arr_ = new String *[memory_size_];
    val_arr_ = new Object *[memory_size_];
    size_ = 0;
  }

  /**
   * Return the number of entries in this map.
   */
  size_t size()
  {
    return size_;
  }

  /**
   * Store keys in the given array. Caller responsible for allocating at least
   * Map::size() elements.
   */
  void keys(String **dest)
  {
    assert(dest != nullptr);

    size_t destIndex = 0;
    for (size_t i = 0; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr)
      {
        dest[destIndex] = key_arr_[i];
        destIndex++;
      }
    }
  }
};

/**
 * A dictionary of string keys and string values.  All keys and values are owned
 * by the caller, and none of the map's methods will modify them.  Keys which
 * are .equals are equal, i.e. the map will never contain two keys which are
 * extensionally equivalent at the same time.
 */
class StringStringMap : public Object
{
public:
  String **key_arr_;
  String **val_arr_;
  size_t size_;
  size_t memory_size_;

  StringStringMap() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new String *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new String *[memory_size_];
    size_ = 0;
  }

  ~StringStringMap()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    String **tempKeys = new String *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      tempKeys[i] = nullptr;
    }
    String **tempVals = new String *[memory_size_];

    size_t hash_index;
    for (size_t i = 0; i < memory_size_ / 2; i++)
    {
      if (key_arr_[i] != nullptr)
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
  String *get(String *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return val_arr_[i];
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return nullptr;
    }

    return nullptr;
  }

  /**
   * Set the value at the given key in this map.
   */
  void add(String *key, String *value)
  {
    assert(key != nullptr);
    assert(value != nullptr);
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

  void set(String *key, String *value)
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
   * Remove the value at the given key in this map. No-op if value not in map.
   */
  void remove(String *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        key_arr_[i] = nullptr;
        val_arr_[i] = nullptr;
        size_--;
        return;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;
    }
  }

  /**
   * Determine if the given key is in this map.
   */
  bool has(String *key)
  {
    assert(key != nullptr);

    size_t hash_index = key->hash() % memory_size_;

    for (size_t i = hash_index; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr && key_arr_[i]->equals(key))
      {
        return true;
      }

      if (hash_index != 0 && i == memory_size_ - 1)
        i = 0;

      if (i == hash_index - 1)
        return false;
    }

    return false;
  }

  /**
   * Remove all keys from this map.
   */
  void clear()
  {
    delete key_arr_;
    delete val_arr_;

    memory_size_ = 16;
    key_arr_ = new String *[memory_size_];
    val_arr_ = new String *[memory_size_];
    size_ = 0;
  }

  /**
   * Return the number of entries in this map.
   */
  size_t size()
  {
    return size_;
  }

  /**
   * Store keys in the given array. Caller responsible for allocating at least
   * Map::size() elements.
   */
  void keys(String **dest)
  {
    assert(dest != nullptr);

    size_t destIndex = 0;
    for (size_t i = 0; i < memory_size_; i++)
    {
      if (key_arr_[i] != nullptr)
      {
        dest[destIndex] = key_arr_[i];
        destIndex++;
      }
    }
  }
};