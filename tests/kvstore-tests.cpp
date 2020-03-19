// lang::CwC

#include "../src/kv-store/kv-store.h"
#include "../src/utils/string.h"
#include "../src/dataframe/dataframe.h"
#include <assert.h>

void test_get()
{
  kvstore *map = new kvstore();

  Key *key = new Key();
  Value *value = new Value();

  map->put(key, value);

  Value *v = map->get(key);

  assert(v->equals(value));

  Key *wrongKey1 = new Key(2);
  Key *wrongKey2 = new Key("wrong");
  Key *wrongKey3 = new Key("wrong", 2);
  Value *wrongValue = new Value(new DataFrame(Schema()));

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
  Key *wrongKey2 = new Key("hello");
  Key *wrongKey3 = new Key("hello", 2);

  assert(!map->has(wrongKey1));
  assert(!map->has(wrongKey2));
  assert(!map->has(wrongKey3));
}

void test_clear()
{
  kvstore *map = new kvstore();

  Key *key1 = new Key("Hello");
  Key *key2 = new Key("World");
  Value *value1 = new Value();
  Value *value2 = new Value(new DataFrame(Schema()));

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
  Key *key2 = new Key("2");
  Value *val2 = new Value(new DataFrame(Schema()));

  map->put(key1, val1);
  map->put(key2, val2);

  Key **keys = new Key *[2];

  map->keys(keys);

  // the keys array contains both keys in the map, in either order
  assert(keys[0] == key1 && keys[1] == key2);
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

  Value *val2 = new Value(new DataFrame(Schema()));

  map->set(key, val2);

  Value *v = map->get(key);

  // the map successfully overwrites values
  assert(v->equals(val2));

  // the map does not delete the key
  assert(map->has(key));
}

void test_rehash()
{
  kvstore *map = new kvstore();

  Key **keys = new Key *[9];
  keys[0] = new Key(0);
  keys[1] = new Key(1);
  keys[2] = new Key(2);
  keys[3] = new Key(3);
  keys[4] = new Key(4);
  keys[5] = new Key(5);
  keys[6] = new Key(6);
  keys[7] = new Key(7);
  keys[8] = new Key(8);

  Value *val = new Value();

  for (int i = 0; i < 9; i++)
  {
    map->put(keys[i], val);
  }

  assert(map->size() == 9);
  assert(map->memory_size_ == 32);

  for (int i = 0; i < 9; i++)
  {
    ooMap->remove(keys[i]);
  }

  assert(map->size() == 0);
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
  return 0;
}
