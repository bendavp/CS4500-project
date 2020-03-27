#include "object.h"
#include "string.h"
#include "helper.h"

#pragma once

/* A queue of Objects, supporting O(1) insertion at the start of the queue and O(1) retrieval / removal from the end
 * of the queue. */
// some ideas for implementation were inspired by https://www.java2novice.com/data-structures-in-java/queue/dynamic-array-impl/
// especially helped with simplification of edge cases
class ObjectQueue : public Object
{
public:
  Object **arr_;
  size_t size_;
  size_t memory_size_;
  size_t firstIndex_;

  ObjectQueue() : Object()
  {
    memory_size_ = 16;
    arr_ = new Object *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  ~ObjectQueue()
  {
    delete arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;

    Object **temp = new Object *[memory_size_];
    for (size_t index = 0; index <= size_; index++)
    {
      temp[index] = arr_[get_index_at(index)];
    }

    delete arr_;
    arr_ = temp;
    firstIndex_ = 0;
  }

  /* Adds the given Object to the start of this Queue */
  void add(Object *o)
  {
    if (size_ == memory_size_)
    {
      incrementSize();
    }

    arr_[get_index_at(size_)] = o;
    size_++;
  }

  /* Adds the given Queue to the start of this Queue */
  void add_all(ObjectQueue *o)
  {
    size_t tempSize = o->size();
    for (size_t i = 0; i < tempSize; i++)
    {
      this->add(o->get_el_at(i));
    }
  }

  /* Returns if given Object is the same as this Queue */
  bool equals(Object *o)
  {
    ObjectQueue *other = dynamic_cast<ObjectQueue *>(o);
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
  Object *remove()
  {
    Object *temp;
    if (size_ == 0)
    {
      println("Nothing to remove!");
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
    arr_ = new Object *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  /* Returns a reference to the Object at the end of this list */
  Object *peek()
  {
    return arr_[firstIndex_];
  }

  /* Returns the number of elements in this Queue */
  size_t size()
  {
    return size_;
  }

  Object *get_el_at(size_t elNum)
  {
    if (elNum < size_)
    {
      return arr_[get_index_at(elNum)];
    }
    else
    {
      println("Element does not exist at given index -- check your bounds!");
      exit(1);
    }
  }

  size_t get_index_at(size_t elNum)
  {
    return (firstIndex_ + elNum) % memory_size_;
  }
};

class StringQueue : public Object
{
public:
  String **arr_;
  size_t size_;
  size_t memory_size_;
  size_t firstIndex_;

  StringQueue() : Object()
  {
    memory_size_ = 16;
    arr_ = new String *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  ~StringQueue()
  {
    delete arr_;
  }

  void incrementSize()
  {
    memory_size_ *= 2;

    String **temp = new String *[memory_size_];
    for (size_t index = 0; index <= size_; index++)
    {
      temp[index] = arr_[get_index_at(index)];
    }

    delete arr_;
    arr_ = temp;
    firstIndex_ = 0;
  }

  /* Adds the given Object to the start of this Queue */
  void add(String *o)
  {
    if (size_ == memory_size_)
    {
      incrementSize();
    }

    arr_[get_index_at(size_)] = o;
    size_++;
  }

  /* Adds the given Queue to the start of this Queue */
  void add_all(StringQueue *o)
  {
    size_t tempSize = o->size();
    for (size_t i = 0; i < tempSize; i++)
    {
      this->add(o->get_el_at(i));
    }
  }

  /* Returns if given Object is the same as this Queue */
  bool equals(Object *o)
  {
    StringQueue *other = dynamic_cast<StringQueue *>(o);
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
  String *remove()
  {
    String *temp;
    if (size_ == 0)
    {
      println("Nothing to remove!");
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
    arr_ = new String *[memory_size_];
    size_ = 0;
    firstIndex_ = 0;
  }

  /* Returns a reference to the Object at the end of this list */
  String *peek()
  {
    return arr_[firstIndex_];
  }

  /* Returns the number of elements in this Queue */
  size_t size()
  {
    return size_;
  }

  String *get_el_at(size_t elNum)
  {
    if (elNum < size_)
    {
      return arr_[get_index_at(elNum)];
    }
    else
    {
      println("Element does not exist at given index -- check your bounds!");
      exit(1);
    }
  }

  size_t get_index_at(size_t elNum)
  {
    return (firstIndex_ + elNum) % memory_size_;
  }
};