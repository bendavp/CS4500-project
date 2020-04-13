#include "../src/kv-store/kv-store.h"
#include "../src/utils/string.h"
#include "../src/dataframe/dataframe.h"
#include <assert.h>
#include <math.h>
#include <cstdlib>

// lang::CwC

void test_get()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *value = new Value();

    map->add(key, value);

    Value *v = map->get(key);

    assert(v->equals(value));
}

void test_has()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *value = new Value();

    map->add(key, value);

    assert(map->has(key));
}

void test_size()
{
    kvstore *map = new kvstore();

    assert(map->size() == 0);

    Key *key = new Key();
    Value *value = new Value();

    map->add(key, value);

    assert(map->size() == 1);
}

// make sure setting a key which was already in the map behaves properly
void test_overwrite()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *val = new Value();

    map->add(key, val);

    Value *v = map->get(key);

    assert(v->equals(val));

    Schema *scm = new Schema();
    Value *val2 = new Value(new DataFrame(*scm));

    map->set(key, val2);

    v = map->get(key);

    // the map successfully overwrites values
    assert(v->equals(val2));

    // the map does not delete the key
    assert(map->has(key));
}

void test_rehash()
{
    kvstore *map = new kvstore();

    Key **keys = new Key *[9];
    keys[0] = new Key(new String("key1"), 1);
    keys[1] = new Key(new String("key2"), 2);
    keys[2] = new Key(new String("key3"), 3);
    keys[3] = new Key(new String("key4"), 4);
    keys[4] = new Key(new String("key5"), 5);
    keys[5] = new Key(new String("key6"), 6);
    keys[6] = new Key(new String("key7"), 7);
    keys[7] = new Key(new String("key8"), 8);
    keys[8] = new Key(new String("key9"), 9);

    Value *val = new Value();

    for (int i = 0; i < 9; i++)
    {
        map->add(keys[i], val);
    }

    assert(map->size() == 9);
    assert(map->memory_size_ == 32);
}

void testSerializerInts()
{
    Serializer s_ = Serializer();

    // testing max int
    int n = INT_MAX;
    char *encoded = new char[4];
    s_.serialize_int(n, encoded);
    int decoded_i = s_.deserialize_int(encoded);
    delete[] encoded;
    assert(n == decoded_i);

    // testing min int
    n = INT_MIN;
    encoded = new char[4];
    s_.serialize_int(n, encoded);
    decoded_i = s_.deserialize_int(encoded);
    delete[] encoded;
    assert(n == decoded_i);

    // testing random int
    n = rand();
    encoded = new char[4];
    s_.serialize_int(n, encoded);
    decoded_i = s_.deserialize_int(encoded);
    assert(n == decoded_i);
}

void testSerializerFloats()
{
    Serializer s_ = Serializer();

    // testing max float
    float f = 3.402823466 * pow(10, 38);
    char *encoded3 = new char[4];
    s_.serialize_float(f, encoded3);
    float decoded_f = s_.deserialize_float(encoded3);
    delete[] encoded3;
    assert(f == decoded_f);

    // testing min float
    f = 1.17549 * pow(-10, 38);
    encoded3 = new char[4];
    s_.serialize_float(f, encoded3);
    decoded_f = s_.deserialize_float(encoded3);
    delete[] encoded3;
    assert(f == decoded_f);

    // testing random float
    f = 1.3223 * rand();
    encoded3 = new char[4];
    s_.serialize_float(f, encoded3);
    decoded_f = s_.deserialize_float(encoded3);
    delete[] encoded3;
    assert(f == decoded_f);
}

void testSeralizerBools()
{
    Serializer s_ = Serializer();

    // testing true
    bool b = true;
    char *encoded2 = new char[4];
    s_.serialize_bool(b, encoded2);
    bool decoded_b = s_.deserialize_bool(encoded2);
    delete[] encoded2;
    assert(b == decoded_b);

    // testing false
    b = false;
    encoded2 = new char[4];
    s_.serialize_bool(b, encoded2);
    decoded_b = s_.deserialize_bool(encoded2);
    delete[] encoded2;
    assert(b == decoded_b);
}

void testSerializerString()
{
    Serializer s_ = Serializer();

    String *str = new String("str");
    char *encodedStr = new char[4];
    s_.serialize_String(str, encodedStr);
    String *decodedStr = s_.deserialize_String(encodedStr);
    delete[] encodedStr;

    assert(decodedStr->equals(str));
}

void testSerializerStringArray()
{
    Serializer s_ = Serializer();

    // string array with 10 values
    size_t sz = 10;
    String **strArray = new String *[sz];
    strArray[0] = new String("hello");
    strArray[1] = new String("world");
    strArray[2] = new String("two");
    strArray[3] = new String("tree");
    strArray[4] = new String("four and");
    strArray[5] = new String("five");
    strArray[6] = new String("six;");
    strArray[7] = new String("seven7");
    strArray[8] = new String("eight ate");
    strArray[9] = new String("nein 999");

    size_t buff_sz = 0;
    for (int i = 0; i < sz; i++)
    {
        buff_sz += strArray[i]->size() + 1;
    }
    char *encoded4 = new char[buff_sz];
    s_.serialize_StringArray(strArray, encoded4, sz);
    String **strArray_decoded = s_.deserialize_StringArray(encoded4, sz);

    for (int i = 0; i < sz; i++)
    {
        assert(strArray[i]->equals(strArray_decoded[i]));
    }
}

void testSimpleDataFrameIBFS()
{
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);

    IntColumn *c1 = new IntColumn();
    c1->push_back(1);
    c1->push_back(2);
    c1->push_back(3);
    c1->push_back(4);
    c1->push_back(5);
    c1->push_back(6);
    c1->push_back(7);
    c1->push_back(8);

    BoolColumn *c2 = new BoolColumn();
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);

    FloatColumn *c3 = new FloatColumn();
    c3->push_back((float)1.22);
    c3->push_back((float)2.22);
    c3->push_back((float)3.22);
    c3->push_back((float)4.22);
    c3->push_back((float)5.22);
    c3->push_back((float)6.22);
    c3->push_back((float)7.22);
    c3->push_back((float)8.22);

    StringColumn *c4 = new StringColumn();
    c4->push_back(new String("h"));
    c4->push_back(new String("e"));
    c4->push_back(new String("l"));
    c4->push_back(new String("l"));
    c4->push_back(new String("o"));
    c4->push_back(new String("w"));
    c4->push_back(new String("o"));
    c4->push_back(new String("r"));

    std::cout << c1->size();
    df->add_column(c1);
    std::cout << df->nrows();
    df->add_column(c2);
    df->add_column(c3);
    df->add_column(c4);
    // printing original DataFrame
    std::cout << "original dataframe" << std::endl;
    df->print();
    Value *v = new Value();
    v->encode(df);

    DataFrame *df2 = v->decode_df();
    // printing deserialized DataFrame
    std::cout << "deserialized dataframe" << std::endl;
    df2->print();
}

void testSimpleDataFrameIBFSBS()
{
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);

    Column *c1 = new IntColumn();
    c1->push_back(1);
    c1->push_back(2);
    c1->push_back(3);
    c1->push_back(4);
    c1->push_back(5);
    c1->push_back(6);
    c1->push_back(7);
    c1->push_back(8);

    Column *c2 = new BoolColumn();
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);

    Column *c3 = new FloatColumn();
    c3->push_back((float)1.22);
    c3->push_back((float)2.22);
    c3->push_back((float)3.22);
    c3->push_back((float)4.22);
    c3->push_back((float)5.22);
    c3->push_back((float)6.22);
    c3->push_back((float)7.22);
    c3->push_back((float)8.22);

    Column *c4 = new StringColumn();
    c4->push_back(new String("h"));
    c4->push_back(new String("e"));
    c4->push_back(new String("l"));
    c4->push_back(new String("l"));
    c4->push_back(new String("o"));
    c4->push_back(new String("w"));
    c4->push_back(new String("o"));
    c4->push_back(new String("r"));

    Column *c5 = new BoolColumn();
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);
    c1->push_back(true);
    c1->push_back(false);

    Column *c6 = new StringColumn();
    c4->push_back(new String("h"));
    c4->push_back(new String("e"));
    c4->push_back(new String("l"));
    c4->push_back(new String("l"));
    c4->push_back(new String("o"));
    c4->push_back(new String("w"));
    c4->push_back(new String("o"));
    c4->push_back(new String("r"));

    df->add_column(c1);
    df->add_column(c2);
    df->add_column(c3);
    df->add_column(c4);
    df->add_column(c5);
    df->add_column(c6);
    // printing original DataFrame
    std::cout << "original dataframe" << std::endl;
    df->print();

    Value *v = new Value();
    v->encode(df);

    DataFrame *df2 = v->decode_df();
    // printing deserialized DataFrame
    std::cout << "deserialized dataframe" << std::endl;
    df2->print();
}

void testTrivialApplication()
{
    size_t SZ = 10;
    float *vals = new float[SZ];
    float sum = 0;
    for (size_t i = 0; i < SZ; ++i)
        sum += vals[i] = i;
    Key *key = new Key(new String("triv"), 0);
    kvstore *kv = new kvstore();
    DataFrame *df = DataFrame::fromArray(key, kv, SZ, vals);
    assert(df->get_float(0, 1) == 1);
    DataFrame *df2 = kv->get(key)->decode_df();
    for (size_t i = 0; i < SZ; ++i)
        sum -= df2->get_float(0, i);
    assert(sum == 0);
}

int main()
{
    test_get();
    test_has();
    test_size();
    test_overwrite();
    // test_rehash();
    testSeralizerBools();
    testSerializerInts();
    testSerializerFloats();
    testSerializerString();
    testSerializerStringArray();
    std::cout << "here";
    testSimpleDataFrameIBFS();
    std::cout << "here";
    testSimpleDataFrameIBFSBS();
    std::cout << "here";

    testTrivialApplication();

    std::cout << "All kvstore tests passed!" << std::endl;

    return 0;
}
