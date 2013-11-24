/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#ifndef stepper_device_SList_hpp
#define stepper_device_SList_hpp

#include <stddef.h>
#include <assert.h>

namespace stepper { namespace device {

namespace detail {
    struct default_tag {};
}

/**
 * Derive from this publicly to support SList.
 */
template <typename Tag=detail::default_tag>
class SListHook
{
public:
    SListHook() : next_(NULL) {}
    ~SListHook() { assert(!is_linked()); }

    SListHook(const SListHook &cpy) : next_(NULL) {}
    SListHook &operator=(const SListHook &cpy) { return *this; }

    bool is_linked() const { return next_ != NULL; }

    SListHook<Tag> *nextOf() const { return next_; }
    void nextOf(SListHook<Tag> *next) { next_ = next; }

private:
    SListHook<Tag> *next_;
};
    
/**
* Intrusive singly linked list supporting constant time size and push_back.
* Safe mode linking.
* See boost::intrusive::slist for details.
*/
template <class T, class hook_type=SListHook<> >
class SList {
public:
    SList() : tail_(NULL), size_(0) {}
    ~SList() { assert(empty()); }

    bool empty() const { return tail_ == NULL; }
    size_t size() const { return size_; }

    void clear() {
        while (!empty()) { popFront(); }
    }

    void pushFront(hook_type &node) {
        assert(!node.is_linked());
        if (empty()) {
            tail_ = &node;
            node.nextOf(&node);
        } else {
            pushTail(node);
        }
        ++size_;
    }

    void pushBack(hook_type &node) {
        assert(!node.is_linked());
        if (empty()) {
            tail_ = &node;
            node.nextOf(&node);
        } else {
            pushTail(node);
            tail_ = tail_->nextOf();
        }
        ++size_;
    }

    T &popFront() {
        assert(!empty());
        hook_type *r = tail_->nextOf();
        if (tail_ == r) {
            tail_ = NULL;
        } else {
            tail_->nextOf(r->nextOf());
        }
        r->nextOf(NULL);
        --size_;
        return *static_cast<T*>(r);
    }

    T &front() {
        assert(!empty());
        return *static_cast<T*>(tail_->nextOf());
    }

    const T &front() const {
        assert(!empty());
        return *static_cast<const T*>(tail_->nextOf());
    }

    T &back() {
        assert(!empty());
        return *static_cast<T*>(tail_);
    }

    const T &back() const {
        assert(!empty());
        return *static_cast<const T*>(tail_);
    }

private:
    hook_type *tail_;
    size_t size_;

    void pushTail(hook_type &node) {
        node.nextOf(tail_->nextOf());
        tail_->nextOf(&node);
    }
};

}}

#endif
