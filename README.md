# lru-cache-cpp - a header only implementation of a templatized LRU cache data structure in C++

## Description

This project contains a configurable implementation of a cache data structure with Least Recently Used eviction algorithm. 
If the cache is full while adding a new element, the least recently used element is replaced by the new one.
The implementation is typical, using a combination of a doubly linked list and a user configured map/hashmap data structure.

## Requirements

C++17 compliant compiler and standard library are required. To compile tests, Catch2 header only test library is needed.

## Usage

The library user may include either `lrucache.h` or the `lrucache_alt.h` alternative implementation.
They implementations differ in how the template is being instatiated, the interface is the same.
The user may pass either `std::map` or `std::unordered_map` to be used as the backing storage. 
Only node based data structures are currently compatible due to the usage of the `extract` C++17 mechanism.

Apart from `std::map`/`std::unordered_map` the users may independently choose how the map stores the extra information needed for maintaining the list:
+ each node stores an unique pointer to a custom list node structure
+ each node stores the value and two indexes into a vector of iterators 

## Examples

```
// Standard implementation, showing how to independently declare underlying representation and map data structure

lrucache::LRUCache<lrucache::BaseUniqPtr<unsigned long, unsigned long, std::map>> cache1(1000000); 

lrucache::LRUCache<lrucache::BaseVal<unsigned long, unsigned long, std::unordered_map>> cache2(1000000); 

// Alternative implementation, showing how to independently declare underlying representation and map data structure

lrucache::LRUCacheVal<std::string, std::string, std::unordered_map> cache3(12345);

lrucache::LRUCacheUniqPtr<std::string, std::string, std::map> cache4(12345);

// Operations

cache.add("a", "alpha");    

auto opt = cache4.get("a");

// Get most recently used item(s)

auto mru = cache4.getMRU(4);

```

## FAQ

### Why the map iterators cannot be stored in the map's value?

`std::unordered_map` does not support such circular definition. `std::map` does, but it is not guaranteed by the C++ Standard.

### How to get Catch2 header?

For example
```
wget https://github.com/catchorg/Catch2/releases/download/v2.13.7/catch.hpp
```
