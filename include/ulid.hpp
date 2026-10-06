#pragma once
#include <chrono>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <string>

namespace kit {

class Ulid {
 public:
  Ulid() { generate(); }

  static Ulid parse(const std::string& text) {
    if (text.size() != 26) throw std::runtime_error("ulid length");
    Ulid u;
    std::uint64_t ts = 0;
    for (int i = 0; i < 10; ++i) ts = (ts << 5) | decode(text[static_cast<std::size_t>(i)]);
    u.time_ms_ = ts;
    std::uint64_t hi = 0, lo = 0;
    for (int i = 10; i < 18; ++i) hi = (hi << 5) | decode(text[static_cast<std::size_t>(i)]);
    for (int i = 18; i < 26; ++i) lo = (lo << 5) | decode(text[static_cast<std::size_t>(i)]);
    u.rand_hi_ = hi & 0xffffffffffull;
    u.rand_lo_ = lo & 0xffffffffffull;
    return u;
  }

  std::string str() const {
    std::string out(26, '0');
    std::uint64_t ts = time_ms_;
    for (int i = 9; i >= 0; --i) { out[static_cast<std::size_t>(i)] = encode(ts & 31); ts >>= 5; }
    std::uint64_t hi = rand_hi_, lo = rand_lo_;
    for (int i = 17; i >= 10; --i) { out[static_cast<std::size_t>(i)] = encode(hi & 31); hi >>= 5; }
    for (int i = 25; i >= 18; --i) { out[static_cast<std::size_t>(i)] = encode(lo & 31); lo >>= 5; }
    return out;
  }

  std::uint64_t time_ms() const { return time_ms_; }

 private:
  static constexpr char kAlphabet[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
  static char encode(std::uint64_t v) { return kAlphabet[v & 31]; }
  static std::uint64_t decode(char c) {
    if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 32);
    if (c == 'I' || c == 'L') c = '1';
    if (c == 'O') c = '0';
    for (int i = 0; i < 32; ++i) if (kAlphabet[i] == c) return static_cast<std::uint64_t>(i);
    throw std::runtime_error("bad ulid char");
  }
  void generate() {
    const auto ms = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    thread_local std::mt19937_64 rng{std::random_device{}()};
    thread_local std::uint64_t last_ms = 0, last_hi = 0, last_lo = 0;
    if (ms == last_ms) {
      last_lo = (last_lo + 1) & 0xffffffffffull;
      if (last_lo == 0) last_hi = (last_hi + 1) & 0xffffffffffull;
    } else {
      last_ms = ms;
      last_hi = rng() & 0xffffffffffull;
      last_lo = rng() & 0xffffffffffull;
    }
    time_ms_ = ms; rand_hi_ = last_hi; rand_lo_ = last_lo;
  }
  std::uint64_t time_ms_ = 0, rand_hi_ = 0, rand_lo_ = 0;
};

}  // namespace kit
