#pragma once

#include "key.h"
#include "value.h"

class kvstore : public Object
{
public:
    Key **key_arr_;
    Value **val_arr_;
    size_t size_;
    size_t memory_size_;

    kvstore() : Object()
    {
        memory_size_ = 16;
        key_arr_ = new Key *[memory_size_];
        for (int i = 0; i < memory_size_; i++)
        {
            key_arr_[i] = nullptr;
        }
        val_arr_ = new Value *[memory_size_];
        size_ = 0;
    }

    ~kvstore()
    {
        delete key_arr_;
        delete val_arr_;
    }

    void incrementSize()
    {
        memory_size_ *= 2;
        Key **tempKeys = new Key *[memory_size_];
        for (int i = 0; i < memory_size_; i++)
        {
            tempKeys[i] = nullptr;
        }
        Value **tempVals = new Value *[memory_size_];

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
    Value *get(Key *key)
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
     * @brief Get the And Wait -- waits for the key? TO BE IMPLEMENTED WHEN WE HAVE NETWORKING
     * 
     * @param key 
     * @return Value* 
     */
    Value *getAndWait(Key *key)
    {
        assert(key != nullptr);
        return nullptr;
    }

    /**
   * Set the value at the given key in this map.
   */
    void put(Key *key, Value *value)
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

    void set(Key *key, Value *value)
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
    void remove(Key *key)
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
    bool has(Key *key)
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
        key_arr_ = new Key *[memory_size_];
        val_arr_ = new Value *[memory_size_];
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
    void keys(Key **dest)
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

// makes a DataFrame with the given float array (as a float column); puts DataFrame as Value into given KVstore with the given Key
DataFrame *DataFrame::fromArray(Key *key, kvstore *kv, size_t sz, float *vals)
{
    Schema *scm = new Schema();
    DataFrame *df = new DataFrame(*scm);
    Column *fc = new FloatColumn();
    for (int i = 0; i < sz; i++)
    {
        fc->push_back(vals[i]);
    }
    df->add_column(fc, nullptr);
    kv->put(key, new Value(df));
    return df;
}

// makes a DataFrame with the given float val; puts DataFrame as Value into given KVstore with the given Key
DataFrame *DataFrame::fromScalar(Key *key, kvstore *kv, float sum)
{
    Schema *scm = new Schema();
    DataFrame *df = new DataFrame(*scm);
    Column *fc = new FloatColumn();
    fc->push_back(sum);
    df->add_column(fc, nullptr);
    kv->put(key, new Value(df));
    return df;
}