# eau2: A semester-long project for CS4500 (Software Development)

### Authors:
- [Amy Tang](https://github.com/tang-amy)
- [Benjamin Kosiborod](https://github.com/bendavp) (myself)

### Description:
Over the course of the Spring 2020 semester, myself and Amy Tang had completed finger exercises and assignments that contained the necessary building blocks to build a basic distributed data processor (the [utils](https://github.com/bendavp/CS4500-project/tree/master/src/utils) layer). Although it is not in a fully functional status, it is able to process text files in a format called Schema On Read (SOR) into a DataFrame data structure (the [adapter](https://github.com/bendavp/CS4500-project/tree/master/src/adapter), forked from [here](https://github.com/gyroknight/boat-a1p1)) & [dataframe](https://github.com/bendavp/CS4500-project/tree/master/src/dataframe) layer), distribute chunks of the data onto different nodes (in place of a network, a pseudonetwork made up of threads that represent other computers on a network) and store those chunks in a key-value store (the [network](https://github.com/bendavp/CS4500-project/tree/master/src/network) & [kv-store](https://github.com/bendavp/CS4500-project/tree/master/src/kv-store) layer). It can then apply some defined function on that data, and print the final result of that function to the user (the [app](https://github.com/bendavp/CS4500-project/tree/master/src/app) layer). The codebase is written in a mix of **C, C with classes (CwC), and C++** 

- For our source code, see [src](https://github.com/bendavp/CS4500-project/tree/master/src)
- For more thorough documentation, see [report](https://github.com/bendavp/CS4500-project/tree/master/report)
- To see what kind of data we were tasked to deal with, including a couple of examples of SOR-format text files, see [data](https://github.com/bendavp/CS4500-project/tree/master/data)
- For our unit tests, see [tests](https://github.com/bendavp/CS4500-project/tree/master/tests)
- For our Makefile, used to compile and run our code, tests, valgrind, and Docker, see [Makefile](https://github.com/bendavp/CS4500-project/blob/master/Makefile)
- For our Docker settings, see [Dockerfile](https://github.com/bendavp/CS4500-project/blob/master/Dockerfile)
