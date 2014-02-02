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
 * Derive from this publicly to support BaseSList.
 */
template <typename Tag=detail::default_tag>
class SListBaseHook
{
public:
    SListBaseHook() : next_(NULL) {}
    ~SListBaseHook() { assert(!isLinked()); }

    SListBaseHook(const SListBaseHook &cpy) : next_(NULL) {}
    SListBaseHook &operator=(const SListBaseHook &cpy) { return *this; }

    bool isLinked() const { return next_ != NULL; }

    SListBaseHook<Tag> *nextOf() const { return next_; }
    void nextOf(SListBaseHook<Tag> *next) { next_ = next; }

private:
    SListBaseHook<Tag> *next_;
};
    
/**
* Intrusive singly linked list supporting constant time size and push_back.
* Safe mode linking.
* See boost::intrusive::slist for details.
*/
template <class T, class hook_type=SListBaseHook<> >
class BaseSList {
public:
    BaseSList() : tail_(NULL), size_(0) {}
    ~BaseSList() { assert(empty()); }

    bool empty() const { return tail_ == NULL; }
    size_t size() const { return size_; }

    void clear() {
        while (!empty()) { popFront(); }
    }

    void pushFront(hook_type &node) {
        assert(!node.isLinked());
        if (empty()) {
            tail_ = &node;
            node.nextOf(&node);
        } else {
            pushTail(node);
        }
        ++size_;
    }

    void pushBack(hook_type &node) {
        assert(!node.isLinked());
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


/**
 * Add this as a public member to support MemberSList.
 */
class SListMemberHook
{
public:
    SListMemberHook() : next_(NULL) {}
    ~SListMemberHook() { assert(!isLinked()); }

    SListMemberHook(const SListMemberHook &cpy) : next_(NULL) {}
    SListMemberHook &operator=(const SListMemberHook &cpy) { return *this; }

    bool isLinked() const { return next_ != NULL; }

    void *nextOf() { return next_; }
    void nextOf(void *next) { next_ = next; }

private:
    void *next_;
};
    
/**
* Intrusive singly linked list supporting constant time size and push_back.
* Safe mode linking.
* See boost::intrusive::slist for details.
*/
template <class T, SListMemberHook T::*hook_member_ptr >
class MemberSList {
public:
    MemberSList() : tail_(NULL), size_(0) {}
    ~MemberSList() { assert(empty()); }

    SListMemberHook &hook(T &node) { return node.*hook_member_ptr; }
    T *nextOf(T &node) {
        SListMemberHook &h = hook(node);
        return reinterpret_cast<T*>(h.nextOf());
    }
    void nextOf(T &node, T *nextVal) {
        SListMemberHook &h = hook(node);
        h.nextOf(nextVal);
    }
    bool isLinked(T &node) { return hook(node).isLinked(); }

    bool empty() const { return tail_ == NULL; }
    size_t size() const { return size_; }

    void clear() {
        while (!empty()) { popFront(); }
    }

    void pushFront(T &node) {
        assert(!isLinked(node));
        if (empty()) {
            tail_ = &node;
            nextOf(node, &node);
        } else {
            pushTail(node);
        }
        ++size_;
    }

    void pushBack(T &node) {
        assert(!isLinked(node));
        if (empty()) {
            tail_ = &node;
            nextOf(node, &node);
        } else {
            pushTail(node);
            tail_ = nextOf(*tail_);
        }
        ++size_;
    }

    T &popFront() {
        assert(!empty());
        T *r = nextOf(*tail_);
        if (tail_ == r) {
            tail_ = NULL;
        } else {
            nextOf(*tail_, nextOf(*r));
        }
        nextOf(*r, NULL);
        --size_;
        return *r;
    }

    T &front() {
        assert(!empty());
        return *nextOf(*tail_);
    }

    T &back() {
        assert(!empty());
        return *tail_;
    }

private:
    T *tail_;
    size_t size_;

    void pushTail(T &node) {
        nextOf(node, nextOf(*tail_));
        nextOf(*tail_, &node);
    }
};


}}

#endif
