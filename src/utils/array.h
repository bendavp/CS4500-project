#pragma once

#include "string.h"
#include <stdarg.h>

/**
 * @brief A class that represents an array of pointers to arrays of integers. This allows us to grow the array 
 * more efficiently and without copying the entire payload. Instead, we copy pointers to the arrays containing the payload 
 * to grow which is more efficient. Additionally, this still allows for O(1) time accessing of the elements.
 * 
 * 
 * @author Amy Tang - tang.amy@husky.neu.edu
 * @author Benjamin Kosiborod - kosiborod.b@husky.neu.edu
 */
class IntFastArray : public Object
{
public:
    int **arr_;       // array of array
    size_t memory_;   // memory for new arrays (i.e. memory/total space of outer array)
    size_t arr_size_; // size of each int array pointer is pointing to
    size_t size_;     // total number of ints stored
    size_t arr_fill_; // current number of pointers to int-arrays stored (i.e. how full the outer array is)

    // constructor for empty
    IntFastArray() : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        arr_fill_ = 0;
        size_ = 0;
        // intialize everything with nullptrs
        arr_ = new int *[memory_];
        for (size_t i = 0; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    IntFastArray(IntFastArray &i)
    {
        memory_ = 16;
        arr_size_ = i.arr_size_;
        size_ = 0;
        arr_fill_ = 0;
        arr_ = new int *[memory_];
        // get values from the array we're copying from and copy them here
        for (size_t j = 0; j < i.size(); j++)
        {
            push_back(i.get(j));
        }
        // fill the remainder of the memory_ with nullptr
        for (size_t j = arr_fill_; j < memory_; j++)
        {
            arr_[j] = nullptr;
        }
    }

    // constructor taking in variable number of arguments
    IntFastArray(int n, ...) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new int *[memory_]; // initializing int**
        arr_fill_ = 0;             // initialize as 0, will iterate through later
        // adding var arg ints
        va_list v1;
        va_start(v1, n);
        for (size_t i = 0; i < n; i++)
        {
            // create new int array/int array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new int[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            arr_[i / arr_size_][i % arr_size_] = va_arg(v1, int);
        }
        va_end(v1);
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    IntFastArray(int n, va_list v1) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new int *[memory_]; // initializing int**
        arr_fill_ = 0;             // initialize as 0, will iterate through later
        // adding var arg ints
        for (size_t i = 0; i < n; i++)
        {
            // create new int array/int array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new int[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            arr_[i / arr_size_][i % arr_size_] = va_arg(v1, int);
        }
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    /**
     * @brief Destroy the Int Fast Array object
     * 
     */
    ~IntFastArray()
    {
        delete[] arr_;
    }

    /**
     * @brief Returns the integer from a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_)
     * @return int the value at that index
     */
    int get(size_t idx)
    {
        assert(!outOfBounds(idx));
        return arr_[idx / arr_size_][idx % arr_size_];
    }

    /**
     * @brief Sets the integer at a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_) at which the value is changed
     * @param i the new value to be used at idx
     */
    void set(size_t idx, int i)
    {
        assert(!outOfBounds(idx));
        arr_[idx / arr_size_][idx % arr_size_] = i;
    }

    /**
     * @brief Adds an integer at the end of the "array" (as if it is a linear array).
     * 
     * @param i the integer to be added
     */
    void push_back(int i)
    {
        // checking to see if when we add an element, we will run out of space
        if (size_ + 1 > arr_fill_ * arr_size_)
        {
            grow(); // expands appropriately
        }
        size_t new_idx = size_; // this is the idx of the value to be added
        arr_[new_idx / arr_size_][new_idx % arr_size_] = i;
        size_ = size_ + 1;
    }

    /**
     * @brief Returns the number of elements/values that the array holds. 
     * 
     * @return size_t the total number of values that the array holds
     */
    size_t size()
    {
        return size_;
    }

    /**
     * @brief Helper function to grow the size of the array. If there is still room for a new pointer, then it will create an int*
     * for the next available "slot". If there is not room, then the array of arrays will double (effectively doubling number
     * of pointers that are available). 
     * 
     */
    void grow()
    {
        // we have filled up all of the int arrays in the memory space we have allocated completely
        // need to expand the memory of the "outer" array
        if (arr_fill_ == memory_)
        {

            memory_ = memory_ * 2;                // double the memory
            int **temp_arr_ = new int *[memory_]; // initalize new int** with the new mem size
            // copy over pointers from filled array
            for (size_t i = 0; i < arr_fill_; i++)
            {
                temp_arr_[i] = arr_[i];
            }
            // fill in the rest of new array with nullptr
            for (size_t i = arr_fill_; i < memory_; i++)
            {
                temp_arr_[i] = nullptr;
            }
            temp_arr_[arr_fill_] = new int[arr_size_]; // initialize new int* pointer in temp_arr_ in the new space
            arr_fill_ = arr_fill_ + 1;                 // increase this count
            delete arr_;                               // delete the old arr_ and its pointers
            arr_ = temp_arr_;                          // set arr_ to the new expanded array
        }
        // we only need to create a new int array in place of the next nullptr
        else
        {
            arr_[arr_fill_] = new int[arr_size_];
            arr_fill_ = arr_fill_ + 1;
        }
    }

    /**
     * @brief Helper function to determine if a given index is out of bounds. 
     * 
     * @return true if the given idx is out of bounds (>= total num of elements)
     * @return false if the given idx is not out of bounds (< total num of elements)
     */
    bool outOfBounds(size_t idx)
    {
        return idx > size_ - 1;
    }

    /**
     * @brief Calculates and returns the hash
     * 
     * @return size_t the hash_value 
     */
    size_t hash_me()
    {
        size_t res = 0;
        for (size_t i = 0; i < size_; i++)
        {
            res += get(i);
        }
        return res;
    }

    /**
     * @brief Returns whether this is equal (in terms of elements/values) compared to the given Object o.
     * 
     * @param o - object this is being compared to
     * @return true - if o is an IntFastArray and all elements are within o and this are the same
     * @return false - if o is not an IntFastArray or if elements in o and this are not the same
     */
    bool equals(Object *o)
    {
        if (o == this)
        {
            return true;
        }
        IntFastArray *other = dynamic_cast<IntFastArray *>(o);
        if (other == nullptr)
        {
            return false;
        }
        if (size_ == other->size())
        {
            for (size_t i = 0; i < size_; i++)
            {
                // check if the element at each index is the same
                if (this->get(i) != other->get(i))
                {
                    return false; // if not, then the o is not equal
                }
            }
            return true; // all elements are the same between this and o
        }
        // not the same number of elements, so we should return false
        else
        {
            return false;
        }
    }

    /**
     * @brief Cloning this as a new Object (that is a IntFastArray)
     * 
     * @return IntFastArray* the cloned IntFastArray
     */
    IntFastArray *clone()
    {
        return new IntFastArray(*this);
    }
};

/**
 * @brief A class that represents an array of pointers to arrays of booleans. This allows us to grow the array 
 * more efficiently and without copying the entire payload. Instead, we copy pointers to the arrays containing the payload 
 * to grow which is more efficient. Additionally, this still allows for O(1) time accessing of the elements.
 * 
 * 
 * @author Amy Tang - tang.amy@husky.neu.edu
 * @author Benjamin Kosiborod - kosiborod.b@husky.neu.edu
 */
class BoolFastArray : public Object
{
public:
    bool **arr_;      // array of array
    size_t memory_;   // memory for new arrays (i.e. memory/total space of outer array)
    size_t arr_size_; // size of each bool array pointer is pointing to
    size_t size_;     // total number of bools stored
    size_t arr_fill_; // current number of pointers to bool-arrays stored (i.e. how full the outer array is)

    // constructor for empty
    BoolFastArray() : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        arr_fill_ = 0;
        size_ = 0;
        // intialize everything with nullptrs
        arr_ = new bool *[memory_];
        for (size_t i = 0; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    BoolFastArray(BoolFastArray &b)
    {
        memory_ = 16;
        arr_size_ = b.arr_size_;
        size_ = 0;
        arr_fill_ = 0;
        arr_ = new bool *[memory_];
        // get values from the array we're copying from and copy them here
        for (size_t j = 0; j < b.size(); j++)
        {
            push_back(b.get(j));
        }
        // fill the remainder of the memory_ with nullptr
        for (size_t j = arr_fill_; j < memory_; j++)
        {
            arr_[j] = nullptr;
        }
    }

    // constructor taking in variable number of arguments
    BoolFastArray(int n, ...) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new bool *[memory_]; // initializing int**
        arr_fill_ = 0;              // initialize as 0, will iterate through later
        // adding var arg ints
        va_list v1;
        va_start(v1, n);
        for (size_t i = 0; i < n; i++)
        {
            // create new bool array/bool array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new bool[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            arr_[i / arr_size_][i % arr_size_] = va_arg(v1, int);
        }
        va_end(v1);
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    BoolFastArray(int n, va_list v1) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new bool *[memory_]; // initializing int**
        arr_fill_ = 0;              // initialize as 0, will iterate through later
        // adding var arg ints
        for (size_t i = 0; i < n; i++)
        {
            // create new bool array/bool array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new bool[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            arr_[i / arr_size_][i % arr_size_] = va_arg(v1, int);
        }
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    /**
     * @brief Destroy the Bool Fast Array object
     * 
     */
    ~BoolFastArray()
    {
        delete[] arr_;
    }

    /**
     * @brief Returns the boolean from a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_)
     * @return bool the value at that index
     */
    bool get(size_t idx)
    {
        assert(!outOfBounds(idx));
        return arr_[idx / arr_size_][idx % arr_size_];
    }

    /**
     * @brief Sets the boolean at a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_) at which the value is changed
     * @param b the new value to be used at idx
     */
    void set(size_t idx, bool b)
    {
        assert(!outOfBounds(idx));
        arr_[idx / arr_size_][idx % arr_size_] = b;
    }

    /**
     * @brief Adds an boolean at the end of the "array" (as if it is a linear array).
     * 
     * @param b the boolean to be added
     */
    void push_back(bool b)
    {
        // checking to see if when we add an element, we will run out of space
        if (size_ + 1 > arr_fill_ * arr_size_)
        {
            grow(); // expands appropriately
        }
        size_t new_idx = size_; // this is the idx of the value to be added
        arr_[new_idx / arr_size_][new_idx % arr_size_] = b;
        size_ = size_ + 1;
    }

    /**
     * @brief Returns the number of elements/values that the array holds. 
     * 
     * @return size_t the total number of values that the array holds
     */
    size_t size()
    {
        return size_;
    }

    /**
     * @brief Helper function to grow the size of the array. If there is still room for a new pointer, then it will create an bool*
     * for the next available "slot". If there is not room, then the array of arrays will double (effectively doubling number
     * of pointers that are available). 
     * 
     */
    void grow()
    {
        // we have filled up all of the int arrays in the memory space we have allocated completely
        // need to expand the memory of the "outer" array
        if (arr_fill_ == memory_)
        {

            memory_ = memory_ * 2;                  // double the memory
            bool **temp_arr_ = new bool *[memory_]; // initalize new int** with the new mem size
            // copy over pointers from filled array
            for (size_t i = 0; i < arr_fill_; i++)
            {
                temp_arr_[i] = arr_[i];
            }
            // fill in the rest of new array with nullptr
            for (size_t i = arr_fill_; i < memory_; i++)
            {
                temp_arr_[i] = nullptr;
            }
            temp_arr_[arr_fill_] = new bool[arr_size_]; // initialize new int* pointer in temp_arr_ in the new space
            arr_fill_ = arr_fill_ + 1;                  // increase this count
            delete arr_;                                // delete the old arr_ and its pointers
            arr_ = temp_arr_;                           // set arr_ to the new expanded array
        }
        // we only need to create a new int array in place of the next nullptr
        else
        {
            arr_[arr_fill_] = new bool[arr_size_];
            arr_fill_ = arr_fill_ + 1;
        }
    }

    /**
     * @brief Helper function to determine if a given index is out of bounds. 
     * 
     * @return true if the given idx is out of bounds (>= total num of elements)
     * @return false if the given idx is not out of bounds (< total num of elements)
     */
    bool outOfBounds(size_t idx)
    {
        return idx > size_ - 1;
    }

    /**
     * @brief Calculates and returns the hash
     * 
     * @return size_t the hash_value 
     */
    size_t hash_me()
    {
        size_t res = 0;
        for (size_t i = 0; i < size_; i++)
        {
            res += get(i);
        }
        return res;
    }

    /**
     * @brief Returns whether this is equal (in terms of elements/values) compared to the given Object o.
     * 
     * @param o - object this is being compared to
     * @return true - if o is an BoolFastArray and all elements are within o and this are the same
     * @return false - if o is not an BoolFastArray or if elements in o and this are not the same
     */
    bool equals(Object *o)
    {
        if (o == this)
        {
            return true;
        }
        BoolFastArray *other = dynamic_cast<BoolFastArray *>(o);
        if (other == nullptr)
        {
            return false;
        }
        if (size_ == other->size())
        {
            for (size_t i = 0; i < size_; i++)
            {
                // check if the element at each index is the same
                if (this->get(i) != other->get(i))
                {
                    return false; // if not, then the o is not equal
                }
            }
            return true; // all elements are the same between this and o
        }
        // not the same number of elements, so we should return false
        else
        {
            return false;
        }
    }

    /**
     * @brief Cloning this as a new Object (that is a BoolFastArray)
     * 
     * @return BoolFastArray* the cloned BoolFastArray
     */
    BoolFastArray *clone()
    {
        return new BoolFastArray(*this);
    }
};

/**
 * @brief A class that represents an array of pointers to arrays of float. This allows us to grow the array 
 * more efficiently and without copying the entire payload. Instead, we copy pointers to the arrays containing the payload 
 * to grow which is more efficient. Additionally, this still allows for O(1) time accessing of the elements.
 * 
 * 
 * @author Amy Tang - tang.amy@husky.neu.edu
 * @author Benjamin Kosiborod - kosiborod.b@husky.neu.edu
 */
class FloatFastArray : public Object
{
public:
    float **arr_;     // array of array
    size_t memory_;   // memory for new arrays (i.e. memory/total space of outer array)
    size_t arr_size_; // size of each float array pointer is pointing to
    size_t size_;     // total number of float stored
    size_t arr_fill_; // current number of pointers to float-arrays stored (i.e. how full the outer array is)

    // constructor for empty
    FloatFastArray() : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        arr_fill_ = 0;
        size_ = 0;
        // intialize everything with nullptrs
        arr_ = new float *[memory_];
        for (size_t i = 0; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    // constructor taking in variable number of arguments
    FloatFastArray(int n, ...) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new float *[memory_]; // initializing float**
        arr_fill_ = 0;               // initialize as 0, will iterate through later
        // adding var arg ints
        va_list v1;
        va_start(v1, n);
        float val;
        for (size_t i = 0; i < n; i++)
        {
            // create new float array/float array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new float[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            val = va_arg(v1, double);
            arr_[i / arr_size_][i % arr_size_] = val;

            // arr_[i / arr_size_][i % arr_size_] = va_arg(v1, float);
        }
        va_end(v1);
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    FloatFastArray(int n, va_list v1) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new float *[memory_]; // initializing float**
        arr_fill_ = 0;               // initialize as 0, will iterate through later
        // adding var arg ints
        float val;
        for (size_t i = 0; i < n; i++)
        {
            // create new float array/float array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new float[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            val = va_arg(v1, double);
            arr_[i / arr_size_][i % arr_size_] = val;
        }
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    FloatFastArray(FloatFastArray &f)
    {
        memory_ = 16;
        arr_size_ = f.arr_size_;
        size_ = 0;
        arr_fill_ = 0;
        arr_ = new float *[memory_];
        // get values from the array we're copying from and copy them here
        for (size_t j = 0; j < f.size(); j++)
        {
            push_back(f.get(j));
        }
        // fill the remainder of the memory_ with nullptr
        for (size_t j = arr_fill_; j < memory_; j++)
        {
            arr_[j] = nullptr;
        }
    }

    /**
     * @brief Destroy the Float Fast Array object
     * 
     */
    ~FloatFastArray()
    {
        delete[] arr_;
    }

    /**
     * @brief Returns the float from a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_)
     * @return float the value at that index
     */
    float get(size_t idx)
    {
        assert(!outOfBounds(idx));
        return arr_[idx / arr_size_][idx % arr_size_];
    }

    /**
     * @brief Sets the float at a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_) at which the value is changed
     * @param f the new value to be used at idx
     */
    void set(size_t idx, float f)
    {
        assert(!outOfBounds(idx));
        arr_[idx / arr_size_][idx % arr_size_] = f;
    }

    /**
     * @brief Adds a float at the end of the "array" (as if it is a linear array).
     * 
     * @param f the float to be added
     */
    void push_back(float f)
    {
        // checking to see if when we add an element, we will run out of space
        if (size_ + 1 > arr_fill_ * arr_size_)
        {
            grow(); // expands appropriately
        }
        size_t new_idx = size_; // this is the idx of the value to be added
        arr_[new_idx / arr_size_][new_idx % arr_size_] = f;
        size_ = size_ + 1;
    }

    /**
     * @brief Returns the number of elements/values that the array holds. 
     * 
     * @return size_t the total number of values that the array holds
     */
    size_t size()
    {
        return size_;
    }

    /**
     * @brief Helper function to grow the size of the array. If there is still room for a new pointer, then it will create an int*
     * for the next available "slot". If there is not room, then the array of arrays will double (effectively doubling number
     * of pointers that are available). 
     * 
     */
    void grow()
    {
        // we have filled up all of the float arrays in the memory space we have allocated completely
        // need to expand the memory of the "outer" array
        if (arr_fill_ == memory_)
        {

            memory_ = memory_ * 2;                    // double the memory
            float **temp_arr_ = new float *[memory_]; // initalize new float** with the new mem size
            // copy over pointers from filled array
            for (size_t i = 0; i < arr_fill_; i++)
            {
                temp_arr_[i] = arr_[i];
            }
            // fill in the rest of new array with nullptr
            for (size_t i = arr_fill_; i < memory_; i++)
            {
                temp_arr_[i] = nullptr;
            }
            temp_arr_[arr_fill_] = new float[arr_size_]; // initialize new float* pointer in temp_arr_ in the new space
            arr_fill_ = arr_fill_ + 1;                   // increase this count
            delete arr_;                                 // delete the old arr_ and its pointers
            arr_ = temp_arr_;                            // set arr_ to the new expanded array
        }
        // we only need to create a new float array in place of the next nullptr
        else
        {
            arr_[arr_fill_] = new float[arr_size_];
            arr_fill_ = arr_fill_ + 1;
        }
    }

    /**
     * @brief Helper function to determine if a given index is out of bounds. 
     * 
     * @return true if the given idx is out of bounds (>= total num of elements)
     * @return false if the given idx is not out of bounds (< total num of elements)
     */
    bool outOfBounds(size_t idx)
    {
        return idx > size_ - 1;
    }

    /**
     * @brief Calculates and returns the hash
     * 
     * @return size_t the hash_value 
     */
    size_t hash_me()
    {
        size_t res = 0;
        for (size_t i = 0; i < size_; i++)
        {
            res += get(i);
        }
        return res;
    }

    /**
     * @brief Returns whether this is equal (in terms of elements/values) compared to the given Object o.
     * 
     * @param o - object this is being compared to
     * @return true - if o is an FloatFastArray and all elements are within o and this are the same
     * @return false - if o is not an FloatFastArray or if elements in o and this are not the same
     */
    bool equals(Object *o)
    {
        if (o == this)
        {
            return true;
        }
        FloatFastArray *other = dynamic_cast<FloatFastArray *>(o);
        if (other == nullptr)
        {
            return false;
        }
        if (size_ == other->size())
        {
            for (size_t i = 0; i < size_; i++)
            {
                // check if the element at each index is the same
                if (this->get(i) != other->get(i))
                {
                    return false; // if not, then the o is not equal
                }
            }
            return true; // all elements are the same between this and o
        }
        // not the same number of elements, so we should return false
        else
        {
            return false;
        }
    }

    /**
     * @brief Cloning this as a new Object (that is a FloatFastArray)
     * 
     * @return FloatFastArray* the cloned FloatFastArray
     */
    FloatFastArray *clone()
    {
        return new FloatFastArray(*this);
    }
};

/**
 * @brief A class that represents an array of pointers to arrays of String*. This allows us to grow the array 
 * more efficiently and without copying the entire payload. Instead, we copy pointers to the arrays containing the payload 
 * to grow which is more efficient. Additionally, this still allows for O(1) time accessing of the elements.
 * 
 * 
 * @author Amy Tang - tang.amy@husky.neu.edu
 * @author Benjamin Kosiborod - kosiborod.b@husky.neu.edu
 */
class StringFastArray : public Object
{
public:
    String ***arr_;   // array of array
    size_t memory_;   // memory for new arrays (i.e. memory/total space of outer array)
    size_t arr_size_; // size of each string* array pointer is pointing to
    size_t size_;     // total number of string* stored
    size_t arr_fill_; // current number of pointers to string*-arrays stored (i.e. how full the outer array is)

    // constructor for empty
    StringFastArray() : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        arr_fill_ = 0;
        size_ = 0;
        // intialize everything with nullptrs
        arr_ = new String **[memory_];
        for (size_t i = 0; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    StringFastArray(StringFastArray &s)
    {
        memory_ = s.memory_;
        arr_size_ = s.arr_size_;
        size_ = s.size();
        arr_fill_ = 0;                 // initialize as 0, will iterate through later
        arr_ = new String **[memory_]; // initializing string***
        for (size_t i = 0; i < size_; i++)
        {
            // create new string array/string array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new String *[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            if (s.get(i) == nullptr)
            {
                arr_[i / arr_size_][i % arr_size_] = nullptr;
            }
            else
            {
                String *new_ = s.get(i)->clone();
                arr_[i / arr_size_][i % arr_size_] = new_;
            }
        }
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    // constructor taking in variable number of arguments
    StringFastArray(int n, ...) : Object()
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new String **[memory_]; // initializing string***
        arr_fill_ = 0;                 // initialize as 0, will iterate through later
        // adding var arg ints
        va_list v1;
        va_start(v1, n);
        for (size_t i = 0; i < n; i++)
        {
            // create new string array/string array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new String *[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            String *new_ = va_arg(v1, String *);
            arr_[i / arr_size_][i % arr_size_] = new_;
        }
        va_end(v1);
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    StringFastArray(int n, va_list v1)
    {
        arr_size_ = 16;
        memory_ = 16;
        size_ = n;
        // doubles memory as necessary to ensure all elements will fit with space to grow
        while (n / arr_size_ > memory_)
        {
            memory_ = memory_ * 2;
        }
        arr_ = new String **[memory_]; // initializing string***
        arr_fill_ = 0;                 // initialize as 0, will iterate through later
        // adding var arg ints
        for (size_t i = 0; i < n; i++)
        {
            // create new string array/string array pointer as necessary
            if (i % arr_size_ == 0)
            {
                arr_[i / arr_size_] = new String *[arr_size_];
                arr_fill_ = arr_fill_ + 1; // we are using up +1 array so increment this count
            }
            // filling appropriate location with the value
            String *new_ = va_arg(v1, String *);
            arr_[i / arr_size_][i % arr_size_] = new_;
        }
        // fill the rest of the array with nullptr
        for (size_t i = arr_fill_; i < memory_; i++)
        {
            arr_[i] = nullptr;
        }
    }

    /**
     * @brief Destroy the String Fast Array object
     * 
     */
    ~StringFastArray()
    {
        delete[] arr_;
    }

    /**
     * @brief Returns the String* from a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_)
     * @return bool the value at that index
     */
    String *get(size_t idx)
    {
        assert(!outOfBounds(idx));
        return arr_[idx / arr_size_][idx % arr_size_];
    }

    /**
     * @brief Sets the String* at a certain location. The math works out as if this is a linear array 
     * rather than an array of arrays. 
     * 
     * @param idx the index as if this is a linear array (out of size_) at which the value is changed
     * @param s the new value to be used at idx
     */
    void set(size_t idx, String *s)
    {
        assert(!outOfBounds(idx));
        delete arr_[idx / arr_size_][idx % arr_size_];
        arr_[idx / arr_size_][idx % arr_size_] = s;
    }

    /**
     * @brief Adds a String* at the end of the "array" (as if it is a linear array).
     * 
     * @param s the String* to be added
     */
    void push_back(String *s)
    {
        // checking to see if when we add an element, we will run out of space
        if (size_ + 1 > arr_fill_ * arr_size_)
        {
            grow(); // expands appropriately
        }
        size_t new_idx = size_; // this is the idx of the value to be added
        arr_[new_idx / arr_size_][new_idx % arr_size_] = s;
        size_ = size_ + 1;
    }

    /**
     * @brief Returns the number of elements/values that the array holds. 
     * 
     * @return size_t the total number of values that the array holds
     */
    size_t size()
    {
        return size_;
    }

    /**
     * @brief Helper function to grow the size of the array. If there is still room for a new pointer, then it will create an string**
     * for the next available "slot". If there is not room, then the array of arrays will double (effectively doubling number
     * of pointers that are available). 
     * 
     */
    void grow()
    {
        // we have filled up all of the string* arrays in the memory space we have allocated completely
        // need to expand the memory of the "outer" array
        if (arr_fill_ == memory_)
        {

            memory_ = memory_ * 2;                        // double the memory
            String ***temp_arr_ = new String **[memory_]; // initalize new string*** with the new mem size
            // copy over pointers from filled array
            for (size_t i = 0; i < arr_fill_; i++)
            {
                temp_arr_[i] = arr_[i];
            }
            // fill in the rest of new array with nullptr
            for (size_t i = arr_fill_; i < memory_; i++)
            {
                temp_arr_[i] = nullptr;
            }
            temp_arr_[arr_fill_] = new String *[arr_size_]; // initialize new string** pointer in temp_arr_ in the new space
            arr_fill_ = arr_fill_ + 1;                      // increase this count
            delete arr_;                                    // delete the old arr_ and its pointers
            arr_ = temp_arr_;                               // set arr_ to the new expanded array
        }
        // we only need to create a new string* array in place of the next nullptr
        else
        {
            arr_[arr_fill_] = new String *[arr_size_];
            arr_fill_ = arr_fill_ + 1;
        }
    }

    /**
     * @brief Helper function to determine if a given index is out of bounds. 
     * 
     * @return true if the given idx is out of bounds (>= total num of elements)
     * @return false if the given idx is not out of bounds (< total num of elements)
     */
    bool outOfBounds(size_t idx)
    {
        return idx > size_ - 1;
    }

    /**
     * @brief Calculates and returns the hash
     * 
     * @return size_t the hash_value 
     */
    size_t hash_me()
    {
        size_t res = 0;
        for (size_t i = 0; i < size_; i++)
        {
            res += get(i)->hash_me();
        }
        return res;
    }

    /**
     * @brief Returns whether this is equal (in terms of elements/values) compared to the given Object o.
     * 
     * @param o - object this is being compared to
     * @return true - if o is an StringFastArray and all elements are within o and this are the same
     * @return false - if o is not an StringFastArray or if elements in o and this are not the same
     */
    bool equals(Object *o)
    {
        if (o == this)
        {
            return true;
        }
        StringFastArray *other = dynamic_cast<StringFastArray *>(o);
        if (other == nullptr)
        {
            return false;
        }
        if (size_ == other->size())
        {
            for (size_t i = 0; i < size_; i++)
            {
                // check if the element at each index is the same
                if (!this->get(i)->equals(other->get(i)))
                {
                    return false; // if not, then the o is not equal
                }
            }
            return true; // all elements are the same between this and o
        }
        // not the same number of elements, so we should return false
        else
        {
            return false;
        }
    }

    /**
     * @brief Cloning this as a new Object (that is a FloatFastArray)
     * 
     * @return StringFastArray* the cloned FloatFastArray
     */
    StringFastArray *clone()
    {
        return new StringFastArray(*this);
    }
};