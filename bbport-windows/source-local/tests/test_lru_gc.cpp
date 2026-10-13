#include "common/lru_cache.h"

int main() {
    Common::LeastRecentlyUsedCache<int, u64> cache;
    auto first = cache.Insert(1, 0);
    auto second = cache.Insert(2, 0);
    cache.Insert(3, 0);
    int visits = 0;
    cache.ForEachItemBelow(0, [&](int) {
        ++visits;
        return true;
    });
    if (visits != 1) return 1;
    int evictions = 0;
    cache.ForEachItemBelow(0, [&](int value) {
        if (value == 1) return false; // Oldest image cannot be downloaded.
        ++evictions;
        cache.Free(second);
        return true;
    });
    if (evictions != 1) return 2;
    cache.Touch(first, 2);
    visits = 0;
    cache.ForEachItemBelow(0, [&](int value) {
        if (value != 3) visits += 100;
        ++visits;
    });
    if (visits != 1) return 3;
}
