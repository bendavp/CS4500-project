#pragma once

#include <assert.h>
#include "../utils/string.h"
#include "../dataframe/dataframe.h"
#include "../kv-store/key.h"
#include "../utils/map.h"

/**
 * @brief Visit every column in a given row>
 * 
 * @author BK and AT
 */
class Counter : public Fielder
{
public:
    OPMap<String, int> *map_; // String to Num map;  Num holds an int
    size_t row_num_;

    Counter(OPMap<String, int> *map) : Fielder()
    {
        map_ = map;
    }

    void start(size_t r)
    {
        row_num_ = r;
    }

    void accept(bool b)
    {
        assert(false);
    }

    void accept(int i)
    {
        assert(false);
    }

    void accept(float f)
    {
        assert(false);
    }

    void accept(String *word)
    {
        assert(word != nullptr);
        int num;
        if (map_->has(word))
        {
            num = map_->get(word) + 1;
            map_->set(word, num);
        }
        else
        {
            map_->add(word->clone(), 1);
        }
    }
};

class CounterR : public Rower
{
public:
    OPMap<String, int> *map_; // String to Num map;  Num holds an int

    CounterR(OPMap<String, int> *map)
    {
        map_ = map;
    }

    bool accept(Row &r) override
    {
        Counter c_ = Counter(map_);
        c_.start(r.get_idx());
        r.visit(r.get_idx(), c_);
        return true;
    }
};