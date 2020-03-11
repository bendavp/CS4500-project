#include "dataframe.h"

class FibCalc : public Fielder
{
public:
    int row_num;
    int total = 0;
    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    void start(size_t r)
    {
        row_num = r;
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b)
    {
        total += calcFib(b);
    }

    void accept(float f)
    {
        total -= calcFib(static_cast<int>(f));
    }

    void accept(int i)
    {
        total += calcFib(i);
    }

    void accept(String *s)
    {
        total -= calcFib(s->size());
    }

    /** Called when all fields have been seen. */
    void done()
    {
        total = 0;
    }

    int calcFib(int i)
    {
        int first = 0;
        int second = 1;
        int total = 0;
        if (i == 0)
        {
            total = first;
        }
        else if (i == 2)
        {
            total = second;
        }
        else
        {
            int next;
            for (int j = 2; j < i; j++)
            {
                next = first + second;
                first = second;
                second = next;
            }
            total = next;
        }
        return total;
    }
};

class FibonnacciRower : public Rower
{
public:
    int total = 0;
    size_t total_rows = 0;

    bool accept(Row &r)
    {
        FibCalc fibcalc_ = FibCalc();
        fibcalc_.start(r.get_idx());
        r.visit(r.get_idx(), fibcalc_);
        total_rows = total_rows + 1;
        return true;
    }

    void join_delete(Rower *other)
    {
        FibonnacciRower *r = static_cast<FibonnacciRower *>(other);
        total += r->total;
        total_rows = total_rows + r->total_rows;
        delete other;
    }

    Rower *clone()
    {
        return new FibonnacciRower();
    }

    void reset()
    {
        total_rows = 0;
        total = 0;
    }
};

class AddInts : public Fielder
{
public:
    int row_num;
    int total = 0;
    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    void start(size_t r)
    {
        row_num = r;
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b)
    {
        total += b;
    }

    void accept(float f)
    {
        total += static_cast<int>(f);
    }

    void accept(int i)
    {
        total += i;
    }

    void accept(String *s)
    {
        total += s->size();
    }

    /** Called when all fields have been seen. */
    virtual void done()
    {
        total = 0;
    }
};

class AddAllInts : public Rower
{
public:
    int total = 0;
    size_t total_rows = 0;

    bool accept(Row &r)
    {
        AddInts addints_ = AddInts();
        addints_.start(r.get_idx());
        r.visit(r.get_idx(), addints_);
        total += addints_.total;
        addints_.done();
        total_rows = total_rows + 1;
        return true;
    }

    void join_delete(Rower *other)
    {
        AddAllInts *r = static_cast<AddAllInts *>(other);
        total += r->total;
        total_rows += total_rows + r->total_rows;
        delete other;
    }

    void reset()
    {
        total_rows = 0;
        total = 0;
    }

    Rower *clone()
    {
        return new AddAllInts();
    }
};