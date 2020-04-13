#pragma once

#include "string.h"
#include <iostream>

using namespace std;

/**
 * @brief A Queue of Objects, supporting O(1) insertion at the start of the queue and O(1) retrieval / removal from the end
 * of the queue.
 * some ideas for implementation were inspired by https://www.java2novice.com/data-structures-in-java/queue/dynamic-array-impl/
 * especially helped with simplification of edge cases
 * 
 * @tparam T a typename that the Queue holds (non-primitive Object or subclass thereof)
 */
template <class T>
class Queue : public Object
{
public:
  T **arr_;
  size_t size_;
  size_t memory_size_;
  size_t firstIndex_;

  Queue() : Object()
  {
    memory_size_ = 16;
    arr_ = new T *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  ~Queue()
  {
    delete arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;

    T **temp = new T *[memory_size_];
    for (size_t index = 0; index <= size_; index++)
      temp[index] = arr_[get_index_at(index)];

    delete arr_;
    arr_ = temp;
    firstIndex_ = 0;
  }

  /* Adds the given Object to the start of this Queue */
  void add(T *m)
  {
    if (size_ == memory_size_)
      incrementSize();

    arr_[get_index_at(size_)] = m;
    size_++;
  }

  /* Returns if given Object is the same as this Queue */
  bool equals(Object *o) override
  {
    Queue<T> *other = dynamic_cast<Queue<T> *>(o);
    if (other == nullptr)
      return false;
    if (other->size_ != size_)
      return false;
    for (size_t i = 0; i < size_; i++)
    {
      if (!get_el_at(i)->equals(other->get_el_at(i)))
        return false;
    }

    return true;
  }

  size_t hash_me() override
  {
    size_t res = 0;
    for (size_t i = 0; i < size_; i++)
      res += get_el_at(i)->hash();
    return res;
  }

  /* Removes and returns the Object at the end of this Queue */
  T *remove()
  {
    assert(size_ == 0);
    T *temp = arr_[firstIndex_];
    if (firstIndex_ == memory_size_ - 1)
      firstIndex_ = 0;
    else
      firstIndex_++;
    size_--;

    return temp;
  }

  /* Returns the number of elements in this Queue */
  size_t size() { return size_; }

  T *get_el_at(size_t elNum)
  {
    assert(elNum < size_);
    return arr_[get_index_at(elNum)];
  }

  size_t get_index_at(size_t elNum)
  {
    return (firstIndex_ + elNum) % memory_size_;
  }

  Queue<T> *clone() override
  {
    Queue<T> *res = new Queue<T>();
    for (size_t i = 0; i < size_; i++)
      res->add(get_el_at(i)->clone());
    return res;
  }
};