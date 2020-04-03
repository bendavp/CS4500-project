#pragma once

#include "counter.h"
#include "kd-store.h"
#include "application.h"

class WordCounter : public Application
{
public:
    kdstore *kdstore_;

    WordCounter(size_t idx, PseudoNetwork *network) : Application(idx, network)
    {
        kdstore_ = new kdstore();
    }

    /**
     * @brief reads in a file and stores it as a distributed dataframe
     * 
     */
    void readFile() {}

    /**
     * @brief 
     * 
     * @param key_
     * @return OPMap<String, int>* 
     */
    OPMap<String, int> *countWords(Key *key_)
    {
        // getting then list of keys that make up the dataframe
        Key *master_key = kdstore_->get(key_);
        Value *s_df_keys = get(master_key);
        FastArray<Key> *df_keys = s_df_keys->decode_keys();

        // total word count (will contain all words and their counts in the entire dataframe)
        OPMap<String, int> *total = new OPMap<String, int>();

        // the
        Value *temp_val;
        DataFrame *temp_df;
        OPMap<String, int> *temp_count;
        for (size_t i = 0; i < df_keys->size(); i++)
        {
            temp_val = get(df_keys->get(i));
            temp_df = temp_val->decode_df();
            temp_count = countDataFrameWords(temp_df);
            total->merge(temp_count);
            delete temp_val;
            delete temp_df;
            delete temp_count;
        }
        return total;
    }

    /**
     * @brief Calculates the word counts in a given DataFrame.
     * 
     * @param df 
     * @return OPMap<String, int>* the word counts from a given DataFrame returned as a map
     */
    OPMap<String, int> *countDataFrameWords(DataFrame *df)
    {
        OPMap<String, int> *map_ = new OPMap<String, int>();
        CounterR wordcounter_ = CounterR(map_);
        df->map(wordcounter_);
        delete df;
        return map_;
    }
};