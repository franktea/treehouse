#include <list>
#include <unordered_map>
#include <optional>
#include <cassert>

using namespace std;


// hashmap的value，除了存储value，还存储该对应的key在链表中的iterator
struct MapValue {
    int value;
    list<int>::iterator node_list_iter;
};

class LRU {
public:
    LRU(size_t capacity):capacity_(capacity) {}
    optional<int> Get(int key) {
        auto it = map_.find(key);
        if(it == map_.end()) {
            return {};
        }

        // 找到了，因为本次访问，该key对应的数据变为最新
        list_.erase(it->second.node_list_iter);
        it->second.node_list_iter = list_.insert(list_.end(), key);
        return it->second.value;
    }

    void Set(int key, int value) {
        // 先查找该节点是否存在
        auto it = map_.find(key);
        if(it == map_.end()) { // 不存在，插入数据
            // 插入数据之前，要看看数据是否已满，如果已满，需要淘汰最老数据
            if(map_.size() >= capacity_) {
                map_.erase(list_.front());
                list_.erase(list_.begin());
            }

            auto&& [it2, b] = map_.insert(make_pair(key, MapValue{value}));
            assert(b);
            it = it2;
        } else { // 存在
            list_.erase(it->second.node_list_iter);
        }

        // 将该key对应的value设置为新的值，并将该key设置为最新(既放入链表末尾)
        it->second.node_list_iter = list_.insert(list_.end(), key);
        it->second.value = value;
    }
private:
    const size_t capacity_;
    unordered_map<int, MapValue> map_;
    list<int> list_; // 链表，存储每个节点的key。头节点为最老的节点，尾节点为最新
};

int main() {
    {
        LRU lru(2);
        lru.Set(2, 1);
        lru.Set(2, 2);
        assert(2 == lru.Get(2));
        lru.Set(1, 1);
        lru.Set(4, 1);
        assert(! lru.Get(2));
    }
}