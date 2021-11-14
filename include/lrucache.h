#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <cassert>

namespace lrucache {

// Pass to LRUCache to represent the list by iterators inside a node pointed to by unique_ptr
template <typename Key, typename Value, template<class, class...> class MapClass>
class BaseUniqPtr {
protected:
   
    struct ListNode {
        typename MapClass<Key, std::unique_ptr<ListNode>>::iterator next_;
        typename MapClass<Key, std::unique_ptr<ListNode>>::iterator prev_;
        Value value_;
    };

    using Map = MapClass<Key, std::unique_ptr<ListNode>>;
    using MapIter = typename Map::iterator;
    using Pair = std::pair<Key, Value>;
    using NodeType = typename Map::node_type;
    using TKey = Key;
    using TValue = Value;

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

    void clear() {
        kv_.clear();
        first_ = kv_.end();
        last_ = kv_.end();
    }

    void preAdd() {

    }

    size_t max_size_{0};
    Map kv_;
    MapIter first_;
    MapIter last_;

    explicit BaseUniqPtr(size_t max_size) 
        : max_size_(max_size)
        , first_(kv_.end())
        , last_(kv_.end()) 
    {

    }

};


// Pass to LRUCache to represent the list by a vector of iterators
template <typename Key, typename Value, template<class, class...> class MapClass>
class BaseVal {
protected:

    struct ListNode {
        size_t start_index_;
        Value value_;
    };

    using Map = MapClass<Key, ListNode>;
    using MapIter = typename Map::iterator;
    using Pair = std::pair<Key, Value>;
    using NodeType = typename Map::node_type;
    using TKey = Key;
    using TValue = Value;

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

    void clear() {
        kv_.clear();
        iters_.clear();
        first_ = kv_.end();
        last_ = kv_.end();
    }

    void preAdd() {
        iters_.push_back(kv_.end());
        iters_.push_back(kv_.end());
    }

    size_t max_size_{0};
    Map kv_;
    MapIter first_;
    MapIter last_;
    std::vector<MapIter> iters_;

    explicit BaseVal(size_t max_size) 
        : max_size_(max_size)
        , first_(kv_.end())
        , last_(kv_.end()) 
    {
        iters_.reserve(max_size * 2);
    }

};


// LRU Cache parametrized class (use BaseVal or BaseUniqPtr)
template <typename Base>
class LRUCache : public Base {
    using typename Base::Map;
    using typename Base::MapIter;
    using typename Base::Pair;
    using typename Base::NodeType;
    using Key = typename Base::TKey;
    using Value = typename Base::TValue;

    using Base::val;
    using Base::getNext;
    using Base::getPrev;
    using Base::setNextTo;
    using Base::setPrevTo;
    using Base::makeListNode;
    using Base::clear;
    using Base::preAdd;

    using Base::max_size_;
    using Base::kv_;
    using Base::first_;
    using Base::last_;

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

public:
    explicit LRUCache(size_t max_size) :
        Base(max_size)
    {

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

};

} // namespace lrucache

#endif
