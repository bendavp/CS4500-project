#include "../dataframe/dataframe.h"

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

/**
 * @brief per each row/field, adds the data inside to a StrBuff
 * 
 */
class StrGeneratorLn : public Fielder
{
public:
    int row_num;
    StrBuff sb_ = StrBuff();
    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    void start(size_t r)
    {
        row_num = r;
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b)
    {
        sb_.c(b).c('\t');
    }

    void accept(float f)
    {
        sb_.c(f).c('\t');
    }

    void accept(int i)
    {
        sb_.c(i).c('\t');
    }

    void accept(String *s)
    {
        sb_.c(s->c_str).c('\t');
    }

    /** Called when all fields have been seen. */
    virtual void done()
    {
        sb_ = StrBuff();
    }
};

/**
 * @brief Generates a string-representation of a DataFrame
 * 
 */
class StrGeneratorDF : public Rower
{
public:
    StrBuff sb_ = StrBuff();
    String *s;
    size_t total_rows = 0;

    bool accept(Row &r)
    {
        StrGeneratorLn strgenln_ = StrGeneratorLn();
        strgenln_.start(r.get_idx());
        r.visit(r.get_idx(), strgenln_);
        sb_.c(strgenln_.current.get()->c_str()).c('\n');
        strgenlen_.done();
        total_rows = total_rows + 1;
        s = sb_.get();
        return true;
    }

    void join_delete(Rower *other)
    {
        StrGeneratorLn *r = static_cast<StrGeneratorLn *>(other);
        sb_.c(r->sb_.get().c_str());
        total_rows += total_rows + r->total_rows;
        delete other;
    }

    void reset()
    {
        total_rows = 0;
        delete sb_.get();
    }

    Rower *clone()
    {
        return new StrGeneratorDF();
    }