#include <iostream>
//#include <map>
//#include <unordered_map>
//#include <optional>
//#include <list>
#include <algorithm>
#include <cassert>


#define CATCH_CONFIG_ENABLE_BENCHMARKING 1
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "lrucache.h"
#include "lrucache_alt.h"

template <class T>
void testCacheOps(T& cache) {
    REQUIRE( cache.size() == 0 );
    bool existed = cache.add("one", "jeden");
    REQUIRE( cache.size() == 1 );
    REQUIRE( !existed );
    auto opt{cache.get("two")};
    REQUIRE( !opt.has_value() );
    opt = cache.get("one");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value() == "jeden" );
    existed = cache.add("two", "dwa");
    REQUIRE( !existed );
    existed = cache.add("three", "trzy");
    REQUIRE( !existed );
    REQUIRE( cache.size() == 3 );
    existed = cache.add("two", "dwa");
    REQUIRE( existed );
    REQUIRE( cache.size() == 3 );
    opt = cache.get("two");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value() == "dwa" );
    opt = cache.get("three");
    REQUIRE( opt.has_value() );
    REQUIRE( opt.value() == "trzy" );
    existed = cache.add("four", "cztery");
    REQUIRE( !existed );
    opt = cache.get("one");
    REQUIRE( !opt.has_value() );   
}

TEST_CASE( "lrucache::LRUCache Ptr U ops", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<std::string, std::string, std::unordered_map>> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCache Ptr M ops", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<std::string, std::string, std::map>> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCache Val U ops", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseVal<std::string, std::string, std::unordered_map>> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCache Val M ops", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseVal<std::string, std::string, std::map>> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCacheUniqPtr U ops", "[lru]" ) {
    lrucache::LRUCacheUniqPtr<std::string, std::string, std::unordered_map> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCacheUniqPtr M ops", "[lru]" ) {
    lrucache::LRUCacheUniqPtr<std::string, std::string, std::map> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCacheVal U ops", "[lru]" ) {
    lrucache::LRUCacheVal<std::string, std::string, std::unordered_map> cache(3);

    testCacheOps(cache);
}

TEST_CASE( "lrucache::LRUCacheVal M ops", "[lru]" ) {
    lrucache::LRUCacheVal<std::string, std::string, std::map> cache(3);

    testCacheOps(cache);
}

template <class T>
void testCacheLRUStringToString(T& cache) {
    using PV = std::vector<std::pair<std::string, std::string>>;  

    REQUIRE( cache.size() == 0 );
    cache.add("one", "jeden");    
    REQUIRE( cache.getMRU(5) == PV{{"one", "jeden"}} );
    cache.add("two", "dwa");
    REQUIRE( cache.getMRU(5) == PV{{"two", "dwa"}, {"one", "jeden"}} );
    cache.add("three", "trzy");
    REQUIRE( cache.size() == 3 );
    PV v{cache.getMRU(5)};
    REQUIRE( v == PV{{"three", "trzy"}, {"two", "dwa"}, {"one", "jeden"}} );
    auto x = cache.get("one");
    v = cache.getMRU(3);
    REQUIRE( v == PV{{"one", "jeden"}, {"three", "trzy"}, {"two", "dwa"}} );
    v = cache.getMRU(2);
    REQUIRE( v == PV{{"one", "jeden"}, {"three", "trzy"}} );    
    x = cache.get("two");
    v = cache.getMRU(4);
    REQUIRE( v == PV{{"two", "dwa"}, {"one", "jeden"}, {"three", "trzy"}} );    
    cache.add("four", "cztery");    
    v = cache.getMRU(4);
    REQUIRE( v == PV{{"four", "cztery"}, {"two", "dwa"}, {"one", "jeden"}} );    
}

TEST_CASE( "lrucache::LRUCache Ptr U order getMRU", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<std::string, std::string, std::unordered_map>> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCache Ptr M order getMRU", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<std::string, std::string, std::map>> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCache Val U order getMRU", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseVal<std::string, std::string, std::unordered_map>> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCache Val M order getMRU", "[lru]" ) {
    lrucache::LRUCache<lrucache::BaseVal<std::string, std::string, std::map>> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCacheUniqPtr U order getMRU", "[lru]" ) {
    lrucache::LRUCacheUniqPtr<std::string, std::string, std::unordered_map> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCacheUniqPtr M order getMRU", "[lru]" ) {
    lrucache::LRUCacheUniqPtr<std::string, std::string, std::map> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCacheVal U order getMRU", "[lru]" ) {
    lrucache::LRUCacheVal<std::string, std::string, std::unordered_map> cache(3);
    testCacheLRUStringToString(cache);
}

TEST_CASE( "lrucache::LRUCacheVal M order getMRU", "[lru]" ) {
    lrucache::LRUCacheVal<std::string, std::string, std::map> cache(3);
    testCacheLRUStringToString(cache);
}

#define BENCHMARKS

#if defined(BENCHMARKS) && defined(NDEBUG)


//const auto Size = 10000UL;

template <typename T>
void benchAddGetExistingKeys(Catch::Benchmark::Chronometer meter, T& cache)
{
    auto Size = cache.maxSize();
    for (auto i = 0UL; i < Size; ++i) {
        cache.add((i + 11) % Size, i);
    }
    volatile unsigned long r{0};
    volatile bool e{false};
    meter.measure([&r, &e, &cache, Size](int j) { 
        for (auto i = 0UL; i < Size; ++i) {            
            e = cache.add((i + j) % Size, i);
            std::optional<unsigned long> opt = cache.get(i);
            r = opt.value_or(0);
        }
        return r;        
    });    
}

template <typename T>
void benchAddGetMixedKeys(Catch::Benchmark::Chronometer meter, T& cache)
{
    auto Size = cache.maxSize();
    for (auto i = 0UL; i < Size; ++i) {
        cache.add((i + 11) % Size, i);
    }
    volatile unsigned long r{0};
    volatile bool e{false};
    meter.measure([&r, &e, &cache, Size](int j) { 
        for (auto i = 0UL; i < Size; ++i) {            
            std::optional<unsigned long> opt = cache.get(Size + i);
            r = opt.value_or(0);
            r = cache.get(i).value_or(0);
            e = cache.add((i + j) % Size + Size, i);
        }
        return r;        
    });    
}

TEST_CASE( "Benchmarks advanced", "[benchmarks]" ) {

#define BENCHMARK_ADVANCED_SIZE(MY_S) BENCHMARK_ADVANCED(((MY_S) + (" " + std::to_string(Size))).c_str())

for (size_t Size = 1000000UL; Size >=100; Size /= 10) {


BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheUniqPtr U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheUniqPtr<unsigned long, unsigned long, std::unordered_map> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheUniqPtr M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheUniqPtr<unsigned long, unsigned long, std::map> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheVal U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheVal<unsigned long, unsigned long, std::unordered_map> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheVal M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheVal<unsigned long, unsigned long, std::map> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};



BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseUniqPtr U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<unsigned long, unsigned long, std::unordered_map>> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseUniqPtr M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<unsigned long, unsigned long, std::map>> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseVal U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseVal<unsigned long, unsigned long, std::unordered_map>> cache(Size);    
    benchAddGetExistingKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseVal M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseVal<unsigned long, unsigned long, std::map>> cache(Size);        
    benchAddGetExistingKeys(meter, cache);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////


BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheUniqPtr U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheUniqPtr<unsigned long, unsigned long, std::unordered_map> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheUniqPtr M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheUniqPtr<unsigned long, unsigned long, std::map> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheVal U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheVal<unsigned long, unsigned long, std::unordered_map> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCacheVal M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCacheVal<unsigned long, unsigned long, std::map> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};


BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseUniqPtr U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<unsigned long, unsigned long, std::unordered_map>> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseUniqPtr M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseUniqPtr<unsigned long, unsigned long, std::map>> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseVal U operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseVal<unsigned long, unsigned long, std::unordered_map>> cache(Size);    
    benchAddGetMixedKeys(meter, cache);
};

BENCHMARK_ADVANCED_SIZE("lrucache::LRUCache BaseVal M operations add/get existing keys")(Catch::Benchmark::Chronometer meter) {
    lrucache::LRUCache<lrucache::BaseVal<unsigned long, unsigned long, std::map>> cache(Size);        
    benchAddGetMixedKeys(meter, cache);
};


}

}

#endif
