#pragma once

#include "string.h"
#include <stdarg.h>
#include <iostream>

using namespace std;

/**
 * @brief A primitive "Fast" Array, i.e. an Array that holds values of
 * primitive type (i.e. int, bool, size_t, float, double, etc.)
 * "Fast" Array meaning it does not copy the entire payload when
 * doubling the size
 * 
 * FastArrays own their contents
 * 
 * @tparam T a primitive type
 */
template <class T>
class FastArray : public Object
{
public:
    T **arr_;         // array of array
    size_t memory_;   // memory for new arrays (i.e. memory/total space of outer array)
    size_t arr_size_; // size of each int array pointer is pointing to
    size_t size_;     // total number of ints stored
    size_t arr_fill_; // current number of pointers to primitive arrays stored (i.e. how full the outer array is)

    // constructor for empty
    FastArray() : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        arr_fill_ = 0;
        size_ = 0;
        // intialize everything with nullptrs
        arr_ = new T *[memory_];
        for (size_t i = 0; i < memory_; i++)
            arr_[i] = nullptr;
    }

    FastArray(FastArray &f)
    {
        memory_ = 16;
        arr_size_ = f.arr_size_;
        size_ = 0;
        arr_fill_ = 0;
        arr_ = new T *[memory_];
        // get values from the array we're copying from and copy them here
        for (size_t i = 0; i < f.size(); i++)
            push_back(f.get(i));
        // fill the remainder of the memory_ with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
            arr_[i] = nullptr;
    }

    /**
     * @brief Destroy the FastArray object
     */
    ~FastArray() { delete[] arr_; }

    /**
     * @brief Returns the integer from a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     */
    T get(size_t idx)
    {
        assert(!outOfBounds(idx));
        return arr_[idx / arr_size_][idx % arr_size_];
    }

    /**
     * @brief Sets the integer at a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     */
    void set(size_t idx, T t)
    {
        assert(!outOfBounds(idx));
        arr_[idx / arr_size_][idx % arr_size_] = t;
    }

    /**
     * @brief Adds a T at the end of the "array" (as if it is a linear array).
     */
    void push_back(T t)
    {
        // checking to see if when we add an element, we will run out of space
        if (size_ + 1 > arr_fill_ * arr_size_)
            grow();
        arr_[size_ / arr_size_][size_ % arr_size_] = t;
        size_ = size_ + 1;
    }

    /**
     * @brief Returns the number of elements/values that the array holds. 
     */
    size_t size() { return size_; }

    /**
     * @brief Helper function to grow the size of the array. If there is still room for a new pointer, then it will create an int*
     * for the next available "slot". If there is not room, then the array of arrays will double (effectively doubling number
     * of pointers that are available).
     */
    void grow()
    {
        // we have filled up all of the int arrays in the memory space we have allocated completely
        // need to expand the memory of the "outer" array
        if (arr_fill_ == memory_)
        {
            memory_ = memory_ * 2;            // double the memory
            T **temp_arr_ = new T *[memory_]; // initalize new int** with the new mem size
            // copy over pointers from filled array
            for (size_t i = 0; i < arr_fill_; i++)
                temp_arr_[i] = arr_[i];
            // fill in the rest of new array with nullptr
            for (size_t i = arr_fill_; i < memory_; i++)
                temp_arr_[i] = nullptr;
            temp_arr_[arr_fill_] = new T[arr_size_]; // initialize new int* pointer in temp_arr_ in the new space
            arr_fill_ = arr_fill_ + 1;               // increase this count
            delete arr_;                             // delete the old arr_ and its pointers
            arr_ = temp_arr_;                        // set arr_ to the new expanded array
        }
        // we only need to create a new int array in place of the next nullptr
        else
        {
            arr_[arr_fill_] = new T[arr_size_];
            arr_fill_ = arr_fill_ + 1;
        }
    }

    /**
     * @brief Helper function to determine if a given index is out of bounds. 
     * 
     * @return true if the given idx is out of bounds (>= total num of elements)
     * @return false if the given idx is not out of bounds (< total num of elements)
     */
    bool outOfBounds(size_t idx) { return idx > size_ - 1; }

    // /**
    //  * @brief Calculates and returns the hash
    //  *
    //  * @return size_t the hash_value
    //  */
    // size_t hash_me() override
    // {
    //     size_t res = 0;
    //     for (size_t i = 0; i < size_; i++)
    //         res += get(i);
    //     return res;
    // }

    /**
     * @brief Returns whether this is equal (in terms of elements/values) compared to the given Object o.
     * 
     * @param o - object this is being compared to
     * @return true - if o is an FastArray and all elements are within o and this are the same
     * @return false - if o is not an FastArray or if elements in o and this are not the same
     */
    bool equals(Object *o) override
    {
        if (o == this)
            return true;
        FastArray *other = dynamic_cast<FastArray *>(o);
        if (other == nullptr)
            return false;
        if (size_ == other->size())
        {
            for (size_t i = 0; i < size_; i++)
                // check if the element at each index is the same
                if (this->get(i) != other->get(i))
                    return false; // if not, then the o is not equal
            return true;          // all elements are the same between this and o
        }
        // not the same number of elements, so we should return false
        else
            return false;
    }

    /**
     * @brief Cloning this as a new Object (that is a FastArray)
     * 
     * @return FastArray* the cloned FastArray
     */
    FastArray *clone() override { return new FastArray(*this); }
};

/**
 * @brief Array class provided by Jan Vitek
 * 
 */
class Array : public Object
{
public:
    size_t capacity_;
    size_t size_;
    Object **objs;

    Array(size_t i)
    {
        objs = new Object *[i];
        capacity_ = i;
        size_ = 0;
    }
    void push_back(Object *o)
    {
        assert(size_ + 1 <= capacity_);
        objs[size_] = o;
        size_++;
    }
    Object *get_(size_t idx)
    {
        assert(idx < size_);
        return objs[idx];
    }
    size_t length() { return size_; }

    void put(size_t idx, Object *o) { objs[idx] = o; }

    // shifts all elements after erased downwards
    void erase_(size_t idx)
    {
        assert(idx < size_);
        Object **temp = new Object *[capacity_];
        bool removed = false;
        for (size_t i = 0; i < size_; i++)
        {
            if (i == idx)
                removed = true;
            else if (removed)
                temp[i - 1] = objs[i];
            else
                temp[i] = objs[i];
        }
        delete[] objs;
        objs = temp;
    }
};