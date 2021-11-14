# Building tests

Select the build type
```
$ cmake -S. -B build/ -DCMAKE_BUILD_TYPE=Release
```
or
```
$ cmake -S. -B build/ -DCMAKE_BUILD_TYPE=Debug
```

Build:
```
cmake --build build/
```

Run tests including benchmarks if Release mode:
```
build/LRUCacheTests
```
