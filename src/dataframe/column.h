//lang:CwC

#pragma once

#include "../utils/array.h"

using namespace std;

class IntColumn;
class BoolColumn;
class FloatColumn;
class StringColumn;

/**
 * @brief Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality.
 * 
 * @author BK + AT
 */
class Column : public Object
{
public:
    /** Type converters: Return same column under its actual type, or
   *  nullptr if of the wrong type.  */
    virtual IntColumn *as_int() { return nullptr; }
    virtual BoolColumn *as_bool() { return nullptr; }
    virtual FloatColumn *as_float() { return nullptr; }
    virtual StringColumn *as_string() { return nullptr; }

    /** Type appropriate push_back methods. Calling the wrong method is
    * undefined behavior. **/
    virtual void push_back(bool val) { assert(false); }
    virtual void push_back(int val) { assert(false); }
    virtual void push_back(float val) { assert(false); }
    virtual void push_back(String *val) { assert(false); }

    /** Returns the number of elements in the column. */
    virtual size_t size() { assert(false); }

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'. */
    char get_type()
    {
        if (this->as_int() != nullptr)
            return 'I';
        else if (this->as_bool() != nullptr)
            return 'B';
        else if (this->as_float() != nullptr)
            return 'F';
        else if (this->as_string() != nullptr)
            return 'S';
        else
            assert(false);
    }

    bool equals(Object *other) override { return other == this; }

    /** Return a copy of the object; nullptr is considered an error */
    virtual Column *clone() override { assert(false); }

    /** Compute the hash code (subclass responsibility) */
    virtual size_t hash_me() override { return 0; }
};

/**
 * @brief BoolColumn -- holds boolean values
 * 
 * @author BK and AT
 */
class BoolColumn : public Column
{
public:
    FastArray<bool> *arr_;

    BoolColumn() : Column() { arr_ = new FastArray<bool>(); }

    BoolColumn(BoolColumn &b) { arr_ = b.arr_->clone(); }

    ~BoolColumn() { delete arr_; }

    bool get(size_t idx) { return arr_->get(idx); }

    BoolColumn *as_bool() override { return this; }

    void set(size_t idx, bool val) { arr_->set(idx, val); }

    size_t size() override { return arr_->size(); }

    void push_back(bool val) override { arr_->push_back(val); }

    /** Return a copy of the object; nullptr is considered an error */
    BoolColumn *clone() override { return new BoolColumn(*this); }

    /** Compute the hash code (subclass responsibility) */
    size_t hash_me() override { return arr_->hash_me(); }
};

/**
 * @brief IntColumn:: Holds int values.
 * 
 * @author BK and AT
 */
class IntColumn : public Column
{
public:
    FastArray<int> *arr_;

    IntColumn() : Column() { arr_ = new FastArray<int>(); }

    IntColumn(IntColumn &i) { arr_ = i.arr_->clone(); }

    ~IntColumn() { delete arr_; }

    int get(size_t idx) { return arr_->get(idx); }

    IntColumn *as_int() override { return this; }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, int val) { arr_->set(idx, val); }

    size_t size() override { return arr_->size(); }

    void push_back(int val) override { arr_->push_back(val); }

    /** Return a copy of the object; nullptr is considered an error */
    IntColumn *clone() override { return new IntColumn(*this); }

    /** Compute the hash code (subclass responsibility) */
    size_t hash_me() override { return arr_->hash_me(); }
};

/**
 * @brief FloatColumn:: Holds float values.
 * 
 * @author BK and AT
 */
class FloatColumn : public Column
{
public:
    FastArray<float> *arr_;

    FloatColumn() : Column() { arr_ = new FastArray<float>(); }

    FloatColumn(FloatColumn &f) { arr_ = f.arr_->clone(); }

    ~FloatColumn() { delete arr_; }

    float get(size_t idx) { return arr_->get(idx); }

    FloatColumn *as_float() override { return this; }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, float val) { arr_->set(idx, val); }

    size_t size() override { return arr_->size(); }

    void push_back(float val) override { arr_->push_back(val); }

    /** Return a copy of the object; nullptr is considered an error */
    FloatColumn *clone() override { return new FloatColumn(*this); }

    /** Compute the hash code (subclass responsibility) */
    size_t hash_me() override { return arr_->hash_me(); }
};

/**
 * @brief StringColumn:: Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 * 
 * @author BK and AT
 */
class StringColumn : public Column
{
public:
    FastArray<String *> *arr_;

    StringColumn() : Column() { arr_ = new FastArray<String *>(); }

    StringColumn(StringColumn &s) { arr_ = s.arr_->clone(); }

    ~StringColumn() { delete arr_; }

    StringColumn *as_string() override { return this; }

    /** Returns the string at idx; undefined on invalid idx.*/
    // returns copy of the string not the same pointer since the String* belongs to column
    String *get(size_t idx) { return arr_->get(idx); }

    /** Out of bound idx is undefined. */
    void set(size_t idx, String *val) { arr_->set(idx, val); }

    size_t size() override { return arr_->size(); }

    void push_back(String *val) override { arr_->push_back(val); }

    /** Return a copy of the object; nullptr is considered an error */
    StringColumn *clone() override { return new StringColumn(*this); }

    /** Compute the hash code (subclass responsibility) */
    size_t hash_me() override { return arr_->hash_me(); }
};