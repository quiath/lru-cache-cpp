#ifndef LRUCACHE_ALT_H
#define LRUCACHE_ALT_H

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <cassert>

namespace lrucache {

template <typename Key, typename Value, template<class, class...> class MapClass>
struct ListNodeNP {
    typename MapClass<Key, std::unique_ptr<ListNodeNP>>::iterator next_;
    typename MapClass<Key, std::unique_ptr<ListNodeNP>>::iterator prev_;
    Value value_;
};

template <typename Value>
struct ListNodeI {
    size_t start_index_;
    Value value_;
};

template <
    typename Key, 
    typename Value, 
    template<class, class...> class MapClass, 
    typename ListNode, 
    typename MapStruct, 
    typename Impl>
class BaseLRUCache {
protected:
    using Map = MapStruct;
    using MapIter = typename Map::iterator;
    using Pair = std::pair<Key, Value>;
    using NodeType = typename Map::node_type;

    explicit BaseLRUCache(size_t max_size) : max_size_(max_size) {}

    Impl* impl() { return static_cast<Impl*>(this); }
    const Impl* impl() const { return static_cast<const Impl*>(this); }

    void preAdd() { impl()->preAdd(); }

    Value& val(NodeType& node) { return impl()->val(node); }
    Value& val(MapIter it) { return impl()->val(it); }
    const Value& val(MapIter it) const { return impl()->val(it); }

    MapIter getNext(MapIter it) const { return impl()->getNext(it); }
    MapIter getPrev(MapIter it) const { return impl()->getPrev(it); }
    MapIter setNextTo(MapIter it, MapIter target) { return impl()->setNextTo(it, target); }
    MapIter setPrevTo(MapIter it, MapIter target) { return impl()->setPrevTo(it, target); }
    typename Map::value_type makeListNode(const Key& key, const Value& value) { return impl()->makeListNode(key, value); };

    void addToFront(MapIter it) {
        if (first_ == kv_.end()) {
            last_ = it; 
            first_ = it; 
            setPrevTo(it, kv_.end()); 
            setNextTo(it, kv_.end()); 
        } else {
            setPrevTo(it, kv_.end()); 
            setPrevTo(first_, it); 
            setNextTo(it, first_); 
            first_ = it;
        }        
    }

    void moveToFront(MapIter it) {
        assert((getPrev(it) == kv_.end()) == (it == first_));
        if (getPrev(it) != kv_.end()) {    
            if (it == last_) {
                setNextTo(getPrev(it), kv_.end());
                last_ = getPrev(it);
            } else {
                setNextTo(getPrev(it), getNext(it));
                setPrevTo(getNext(it), getPrev(it));
            }
            setPrevTo(first_, it);
            setPrevTo(it, kv_.end());
            setNextTo(it, first_);
            first_ = it;
        } 
    }

    bool add(const Key& key, const Value& value) {
        assert((kv_.size() > 0) == (first_ != kv_.end()));
        assert((last_ == kv_.end()) == (first_ == kv_.end()));
                                                
        auto it = kv_.find(key);
       
        if (it == kv_.end()) {
            if (kv_.size() == max_size_) {

                // extract the last_
                typename Map::node_type extracted = kv_.extract(last_);
                extracted.key() = key;
                val(extracted) = value; 
                kv_.insert(std::move(extracted));

                // insert as the first_
                moveToFront(last_);
                               
                return false;
            }

            preAdd();
            typename Map::value_type v{makeListNode(key, value)};
            auto [it, is_inserted] = kv_.insert(std::move(v));
            addToFront(it);

            return false;
        } 

        val(it) = value; 
        moveToFront(it);

        return true;
    }

    std::optional<Value> get(const Key& key) {
        assert((kv_.size() > 0) == (first_ != kv_.end()));
        assert((last_ == kv_.end()) == (first_ == kv_.end()));        

        auto it = kv_.find(key);
        if (it == kv_.end()) {
            return {};
        }
        moveToFront(it);

       return val(it); 
    }

    size_t size() const { return kv_.size(); }
    size_t maxSize() const { return max_size_; }

    std::vector<Key> getMRUKeys(size_t n) const {
        std::vector<Key> v;
        v.reserve(std::min(n, kv_.size()));
        auto it = first_;
        while (it != kv_.end() && n != 0) {
            v.push_back(it->first);
            --n;
            it = getNext(it);
        }
        return v;
    }

    std::vector<Pair> getMRU(size_t n) const {
        std::vector<Pair> v;
        v.reserve(std::min(n, kv_.size()));
        auto it = first_;
        while (it != kv_.end() && n != 0) {
            assert(getPrev(it) == kv_.end() || getNext(getPrev(it)) == it);
            assert(getNext(it) == kv_.end() || getPrev(getNext(it)) == it);
            assert((getPrev(it) == kv_.end()) == (first_ == it));
            assert((getNext(it) == kv_.end()) == (last_ == it));            
            v.emplace_back(it->first, val(it));  
            --n;
            it = getNext(it);
        }
        return v;        
    }  

    void clear() {
        kv_.clear();
        first_ = kv_.end();
        last_ = kv_.end();
        impl()->clearImpl();
    }

    size_t max_size_{0};    
    Map kv_;
    MapIter first_{kv_.end()};
    MapIter last_{kv_.end()};


};

// LRU Cache using uniq_ptr to nodes containing iterators to represent a list
template <typename Key, typename Value, template<class, class...> class MapClass>
class LRUCacheUniqPtr : public BaseLRUCache<
    Key, 
    Value, 
    MapClass, 
    ListNodeNP<Key, Value, MapClass>, 
    MapClass<Key, std::unique_ptr<ListNodeNP<Key, Value, MapClass>>>,
    LRUCacheUniqPtr<Key, Value, MapClass>> {

    using ListNode = ListNodeNP<Key, Value, MapClass>;
    using Base = BaseLRUCache<
        Key, Value, MapClass, ListNode, 
        MapClass<Key, std::unique_ptr<ListNodeNP<Key, Value, MapClass>>>, 
        LRUCacheUniqPtr<Key, Value, MapClass>>;
    friend class BaseLRUCache<
        Key, Value, MapClass, ListNode, 
        MapClass<Key, std::unique_ptr<ListNodeNP<Key, Value, MapClass>>>, 
        LRUCacheUniqPtr<Key, Value, MapClass>>;
    
    using typename Base::Map;
    using typename Base::MapIter;
    using typename Base::Pair;
    using typename Base::NodeType;

    using Base::kv_; 

    Value& val(NodeType& node) { return node.mapped()->value_; }
    
    Value& val(MapIter it) { return it->second->value_; }
    const Value& val(MapIter it) const { return it->second->value_; }

    MapIter getNext(MapIter it) const { 
        assert(it->second);
        return it->second->next_; 
    }

    MapIter getPrev(MapIter it) const { 
        assert(it->second);
        return it->second->prev_; 
    }

    MapIter setNextTo(MapIter it, MapIter target) { 
        assert(it->second);
        return it->second->next_ = target; 
    }

    MapIter setPrevTo(MapIter it, MapIter target) { 
        assert(it->second);
        return it->second->prev_ = target; 
    }

    typename Map::value_type makeListNode(const Key& key, const Value& value) {
        std::unique_ptr<ListNode> p{new ListNode{kv_.end(), kv_.end(), value}};
        return {key, std::move(p)};
    }

    void preAdd() {

    }

    void clearImpl() {

    }
 
public:

    using Base::clear;
    using Base::add;
    using Base::size;
    using Base::maxSize;
    using Base::get;
    using Base::getMRUKeys;
    using Base::getMRU;

    explicit LRUCacheUniqPtr(size_t max_size) 
        : Base(max_size)
    {

    }

};


// LRU Cache using a separate vector of iterators to represent the LRU list
template <typename Key, typename Value, template<class, class...> class MapClass>
class LRUCacheVal : public BaseLRUCache<
    Key, 
    Value, 
    MapClass, 
    ListNodeI<Value>, 
    MapClass<Key, ListNodeI<Value>>,
    LRUCacheVal<Key, Value, MapClass>> {

    using ListNode = ListNodeI<Value>;
    using Base = BaseLRUCache<
                    Key, Value, MapClass, ListNode, 
                    MapClass<Key, ListNodeI<Value>>, LRUCacheVal<Key, Value, MapClass>>;
    friend class BaseLRUCache<Key, Value, MapClass, ListNode, 
                    MapClass<Key, ListNodeI<Value>>, LRUCacheVal<Key, Value, MapClass>>;
    
    using typename Base::Map;
    using typename Base::MapIter;
    using typename Base::Pair;
    using typename Base::NodeType;

    using Base::kv_; 

    Value& val(NodeType& node) { return node.mapped().value_; }
    
    Value& val(MapIter it) { return it->second.value_; }
    const Value& val(MapIter it) const { return it->second.value_; }

    MapIter getNext(MapIter it) const { 
        assert(it->second.start_index_ + 1 < iters_.size());
        return iters_[it->second.start_index_ + 1]; 
    }

    MapIter getPrev(MapIter it) const { 
        assert(it->second.start_index_ < iters_.size());
        return iters_[it->second.start_index_]; 
    }

    MapIter setNextTo(MapIter it, MapIter target) { 
        assert(it->second.start_index_ + 1 < iters_.size());
        return iters_[it->second.start_index_ + 1] = target; 
    }

    MapIter setPrevTo(MapIter it, MapIter target) { 
        assert(it->second.start_index_ < iters_.size());
        return iters_[it->second.start_index_] = target; 
    }

    typename Map::value_type makeListNode(const Key& key, const Value& value) {
        return {key, ListNode{iters_.size() - 2, value}};
    }

    void clearImpl() {
        iters_.clear();
    }

    void preAdd() {
        iters_.push_back(kv_.end());
        iters_.push_back(kv_.end());
    }
 
    std::vector<MapIter> iters_;

public:

    using Base::clear;
    using Base::add;
    using Base::size;
    using Base::maxSize;
    using Base::get;
    using Base::getMRUKeys;
    using Base::getMRU;

    explicit LRUCacheVal(size_t max_size) 
        : Base(max_size)
    {

    }

};

} // namespace lrucache

#endif
