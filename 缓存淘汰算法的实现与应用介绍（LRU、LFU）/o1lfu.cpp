#include <list>
#include <unordered_map>
#include <optional>
#include <cassert>

using namespace std;

struct ListNode;

struct CountListNode {
    int count;
    list<ListNode> node_list;
    //CountListNode(int c):count(c) {}
};

struct ListNode {
    int key;
    list<CountListNode>::iterator count_list_iter;
};

struct MapValue {
    int value;
    list<ListNode>::iterator node_list_iter;
};

class LFU {
private:
    unordered_map<int, MapValue> map_;
    list<CountListNode> count_list_;
    const size_t max_size_;

    // 将某个数据的使用次数加1，需要将它从当前计数的链表中移除，然后放入当前计数+1的链表中去
    void IncCount(decltype(map_)::iterator it) {
        auto node_list_it = it->second.node_list_iter;
        auto count_list_it = node_list_it->count_list_iter;
        auto& node_list = count_list_it->node_list;
        
        auto next_count_list_it = count_list_it;
        ++next_count_list_it;
        if(next_count_list_it == count_list_.end() || next_count_list_it->count != count_list_it->count+1) {
            next_count_list_it = count_list_.insert(next_count_list_it, 
                CountListNode{ count_list_it->count+1 });
        }
        node_list_it->count_list_iter = next_count_list_it;
        auto new_node_list_it = next_count_list_it->node_list.insert(next_count_list_it->node_list.end(), *node_list_it);
        it->second.node_list_iter = new_node_list_it;

        node_list.erase(node_list_it);

        // 链表中移除节点以后，如果链表变为了空链表，需要将整个链表删除
        if(node_list.empty()) {
            count_list_.erase(count_list_it);
        }
    }
public:
    LFU(size_t size):max_size_(size) { }

    void Put(int k, int v) {
        auto it = map_.find(k);
        if(it == map_.end()) { // 不存在，应该插入新数据
            // 插入新数据之前要看缓存条数是否已满
            if(map_.size() >= max_size_) { // 已满，需要淘汰一个
                auto& node_list = count_list_.begin()->node_list;
                map_.erase(node_list.begin()->key);
                node_list.erase(node_list.begin());
                if(node_list.empty()) {
                    count_list_.erase(count_list_.begin());
                }
                
            }

            // 向count=1的链表中插入新数据
            auto first = count_list_.begin();
            if(first == count_list_.end() || first->count != 1) {
                first = count_list_.insert(count_list_.begin(), CountListNode{1});
            }

            auto node_list_it = first->node_list.insert(first->node_list.end(), ListNode{k, first});
            map_.insert(std::make_pair(k, MapValue{v, node_list_it}));
        } else { // 存在，替换value，并将访问次数加1
            it->second.value = v;
            IncCount(it);
        }
    }

    optional<int> Get(int k) {
        auto it = map_.find(k);
        if(it == map_.end()) {
            return {};
        }

        // 找到了，访问次数加1
        IncCount(it);

        return it->second.value;
    }
};

int main() {
    {
        LFU lfu(2);
        lfu.Put(1, 1);
        lfu.Put(2, 2);
        lfu.Put(3, 3);
        assert(!lfu.Get(1));
    }

    {
        LFU lfu(2);
        lfu.Put(1, 1);
        lfu.Put(2, 2);
        assert(1 == lfu.Get(1));
        lfu.Put(3, 3);
        assert(! lfu.Get(2));
        assert(3 == lfu.Get(3));
        lfu.Put(4, 4);
        assert(! lfu.Get(1));
        assert(3 == lfu.Get(3));
        assert(4 == lfu.Get(4));
    }

    {
        LFU lfu(2);
        lfu.Put(3, 1);
        lfu.Put(2, 1);
        lfu.Put(2, 2);
        lfu.Put(4, 4);
        assert(2 == lfu.Get(2));       
    }

    {
        LFU lfu(2);
        lfu.Put(2, 1);
        lfu.Put(3, 2);
        assert(2 == lfu.Get(3));
        assert(1 == lfu.Get(2));
        lfu.Put(4, 3);
        assert(1 == lfu.Get(2));
        assert(! lfu.Get(3));
        assert(3 == lfu.Get(4));         
    }
}
