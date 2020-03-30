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
    {
      temp[index] = arr_[get_index_at(index)];
    }

    delete arr_;
    arr_ = temp;
    firstIndex_ = 0;
  }

  /* Adds the given Object to the start of this Queue */
  void add(T *m)
  {
    if (size_ == memory_size_)
    {
      incrementSize();
    }

    arr_[get_index_at(size_)] = m;
    size_++;
  }

  /* Adds the given Queue to the start of this Queue */
  void add_all(Queue *m)
  {
    size_t tempSize = m->size();
    for (size_t i = 0; i < tempSize; i++)
    {
      this->add(m->get_el_at(i));
    }
  }

  /* Returns if given Object is the same as this Queue */
  bool equals(Object *o)
  {
    Queue<T> *other = dynamic_cast<Queue<T> *>(o);
    if (other == nullptr)
    {
      return false;
    }

    if (other->size_ != size_)
    {
      return false;
    }

    for (size_t i = 0; i < size_; i++)
    {
      if (!get_el_at(i)->equals(other->get_el_at(i)))
      {
        return false;
      }
    }

    return true;
  }

  size_t hash()
  {
    size_t res = 0;
    for (int i = 0; i < size_; i++)
    {
      res += get_el_at(i)->hash();
    }
    return res;
  }

  /* Removes and returns the Object at the end of this Queue */
  T *remove()
  {
    T *temp;
    if (size_ == 0)
    {
      perror("Nothing to remove!");
      exit(1);
    }
    else
    {
      temp = arr_[firstIndex_];
      if (firstIndex_ == memory_size_ - 1)
      {
        firstIndex_ = 0;
      }
      else
      {
        firstIndex_++;
      }
      size_--;
    }

    return temp;
  }

  /* Clears this Queue of all elements */
  void clear()
  {
    delete arr_;
    memory_size_ = 16;
    arr_ = new T *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  /* Returns a reference to the Object at the end of this list */
  T *peek()
  {
    return arr_[firstIndex_];
  }

  /* Returns the number of elements in this Queue */
  size_t size()
  {
    return size_;
  }

  T *get_el_at(size_t elNum)
  {
    if (elNum < size_)
    {
      return arr_[get_index_at(elNum)];
    }
    else
    {
      perror("Element does not exist at given index -- check your bounds!");
      exit(1);
    }
  }

  size_t get_index_at(size_t elNum)
  {
    return (firstIndex_ + elNum) % memory_size_;
  }

  Queue<T> *clone()
  {
    Queue<T> *res = new Queue<T>();
    for (int i = 0; i < size_; i++)
    {
      res->add(get_el_at(i)->clone());
    }
    return res;
  }
};