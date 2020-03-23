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

    map->put(key, value);

    Value *v = map->get(key);

    assert(v->equals(value));

    Key *wrongKey1 = new Key(2);
    Key *wrongKey2 = new Key(new String("wrong"));
    Key *wrongKey3 = new Key(new String("wrong"), 2);

    Value *result1 = map->get(wrongKey1);
    Value *result2 = map->get(wrongKey2);
    Value *result3 = map->get(wrongKey3);

    assert(result1 == nullptr);
    assert(result2 == nullptr);
    assert(result3 == nullptr);
}

void test_remove()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *value = new Value();

    map->put(key, value);

    map->remove(key);

    assert(map->size() == 0);
}

void test_has()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *value = new Value();

    map->put(key, value);

    assert(map->has(key));

    Key *wrongKey1 = new Key(2);
    Key *wrongKey2 = new Key(new String("hello"));
    Key *wrongKey3 = new Key(new String("hello"), 2);

    assert(!map->has(wrongKey1));
    assert(!map->has(wrongKey2));
    assert(!map->has(wrongKey3));
}

void test_clear()
{
    kvstore *map = new kvstore();

    Key *key1 = new Key(new String("Hello"));
    Key *key2 = new Key(new String("World"));
    Value *value1 = new Value();
    Schema *scm = new Schema();
    Value *value2 = new Value(new DataFrame(*scm));

    map->put(key1, value1);
    map->put(key2, value2);

    map->clear();

    Value *r1 = map->get(key1);
    Value *r2 = map->get(key2);

    // successfully clears all entries
    assert(r1 == nullptr);
    assert(r2 == nullptr);
}

void test_size()
{
    kvstore *map = new kvstore();

    assert(map->size() == 0);

    Key *key = new Key();
    Value *value = new Value();

    map->put(key, value);

    assert(map->size() == 1);

    map->remove(key);

    assert(map->size() == 0);
}

void test_keys()
{
    kvstore *map = new kvstore();

    Key *key1 = new Key();
    Value *val1 = new Value();
    Key *key2 = new Key(new String("2"));
    Schema *scm = new Schema();
    Value *val2 = new Value(new DataFrame(*scm));

    map->put(key1, val1);
    map->put(key2, val2);

    Key **keys = new Key *[2];

    map->keys(keys);

    // the keys array contains both keys in the map, in some order
    assert((keys[0]->equals(key1) && keys[1]->equals(key2)) || (keys[0]->equals(key2) && keys[1]->equals(key1)));
}

// make sure setting a key which was already in the map behaves properly
void test_overwrite()
{
    kvstore *map = new kvstore();

    Key *key = new Key();
    Value *val = new Value();

    map->put(key, val);

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
    keys[0] = new Key(1);
    keys[1] = new Key(2);
    keys[2] = new Key(3);
    keys[3] = new Key(4);
    keys[4] = new Key(5);
    keys[5] = new Key(6);
    keys[6] = new Key(7);
    keys[7] = new Key(8);
    keys[8] = new Key(9);

    Value *val = new Value();

    for (int i = 0; i < 9; i++)
    {
        map->put(keys[i], val);
    }

    assert(map->size() == 9);
    assert(map->memory_size_ == 32);

    for (int i = 0; i < 9; i++)
    {
        map->remove(keys[i]);
    }

    assert(map->size() == 0);
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
    Column *c1 = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
    Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
    df->add_column(c1, nullptr);
    df->add_column(c2, nullptr);
    df->add_column(c3, nullptr);
    df->add_column(c4, nullptr);
    // printing original DataFrame
    std::cout << "original dataframe" << std::endl;
    df->print();
    Value *v = new Value();
    v->encode(df);

    DataFrame *df2 = v->decode();
    // printing deserialized DataFrame
    std::cout << "deserialized dataframe" << std::endl;
    df2->print();
}

void testSimpleDataFrameIBFSBS()
{
    Schema *s = new Schema();
    DataFrame *df = new DataFrame(*s);
    Column *c1 = new IntColumn(8, 1, 2, 3, 4, 5, 6, 7, 8);
    Column *c2 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    Column *c3 = new FloatColumn(8, 1.22, 2.22, 3.22, 4.22, 5.22, 6.22, 7.22, 8.22);
    Column *c4 = new StringColumn(8, new String("h"), new String("e"), new String("l"), new String("l"), new String("o"), new String("w"), new String("o"), new String("r"));
    Column *c5 = new BoolColumn(8, true, false, true, false, true, false, true, false);
    Column *c6 = new StringColumn(8, new String("hello"), new String("world"), new String("blah1"), new String("swdev"), new String("gogogog"), new String("wwww"), new String("no"), new String("REEEE"));
    df->add_column(c1, nullptr);
    df->add_column(c2, nullptr);
    df->add_column(c3, nullptr);
    df->add_column(c4, nullptr);
    df->add_column(c5, nullptr);
    df->add_column(c6, nullptr);
    // printing original DataFrame
    std::cout << "original dataframe" << std::endl;
    df->print();

    Value *v = new Value();
    v->encode(df);

    DataFrame *df2 = v->decode();
    // printing deserialized DataFrame
    std::cout << "deserialized dataframe" << std::endl;
    df2->print();
}

void testSerializeKey()
{
    Key *before = new Key(new String("hello"), 1);
    char *encoded_key1 = before->encode();
    Key *after = new Key(encoded_key1);

    std::cout << "before: home: " << before->home << " name: " << before->name->c_str() << std::endl;
    std::cout << "after: home: " << after->home << " name: " << after->name->c_str() << std::endl;

    assert(before->equals(after));
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
    DataFrame *df2 = kv->get(key)->decode();
    for (size_t i = 0; i < SZ; ++i)
        sum -= df2->get_float(0, i);
    assert(sum == 0);
}

int main()
{
    test_get();
    test_remove();
    test_has();
    test_clear();
    test_size();
    test_keys();
    test_overwrite();
    test_rehash();

    testSeralizerBools();
    testSerializerInts();
    testSerializerFloats();
    testSerializerString();
    testSerializerStringArray();
    testSimpleDataFrameIBFS();
    testSimpleDataFrameIBFSBS();
    testSerializeKey();

    testTrivialApplication();

    std::cout << "All kvstore tests passed!" << std::endl;

    return 0;
}
