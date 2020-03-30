#pragma once

// lang::CwC

#include "object.h"
#include "string.h"
#include "helper.h"
#include <iostream>

using namespace std;

/**
 * @brief A Map of Object type to primitive type
 * 
 * @tparam T non-primitive Object or subtype thereof
 * @tparam U primitive type (int, float, bool, size_t,  etc.)
 * 
 * @author BK and AT
 */
template <class T, class U>
class OPMap : public Object
{
public:
  T **key_arr_;
  U *val_arr_;
  size_t size_;
  size_t memory_size_;

  OPMap<T, U>() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new U[memory_size_];
    size_ = 0;
  }

  ~OPMap()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    T **tempKeys = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      tempKeys[i] = nullptr;
    }
    U *tempVals = new U[memory_size_];

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
  U get(T *key)
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
        return -1;
    }

    return -1;
  }

  /**
   * Set the value at the given key in this map.
   */
  void add(T *key, U value)
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

  void set(T *key, U value)
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
  void remove(T *key)
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
  bool has(T *key)
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
    key_arr_ = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new U[memory_size_];
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
  void keys(T **dest)
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
 * @brief A Map of Object type to Object type
 * 
 * @tparam T non-primitive Object or subtype thereof
 * @tparam U non-primitive Object or subtype thereof
 * 
 * @author BK and AT
 */
template <class T, class U>
class OOMap : public Object
{
public:
  T **key_arr_;
  U **val_arr_;
  size_t size_;
  size_t memory_size_;

  OOMap<T, U>() : Object()
  {
    memory_size_ = 16;
    key_arr_ = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new U *[memory_size_];
    size_ = 0;
  }

  ~OOMap()
  {
    delete key_arr_;
    delete val_arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;
    T **tempKeys = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      tempKeys[i] = nullptr;
    }
    U **tempVals = new U *[memory_size_];

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
  U *get(T *key)
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
  void add(T *key, U *value)
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

  void set(T *key, U *value)
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
  void remove(T *key)
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
  bool has(T *key)
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
    key_arr_ = new T *[memory_size_];
    for (int i = 0; i < memory_size_; i++)
    {
      key_arr_[i] = nullptr;
    }
    val_arr_ = new U *[memory_size_];
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
  void keys(T **dest)
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