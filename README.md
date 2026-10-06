# cpp-ulid

C++20 ULID generator. 48-bit millisecond timestamp plus 80 bits of randomness, Crockford base32, monotonic inside the same millisecond.

```cpp
std::string id = kit::Ulid().str();
kit::Ulid parsed = kit::Ulid::parse(id);
```

Lexicographic order matches creation order across millisecond boundaries. MIT
