#pragma once
#include <cstddef>
#include <utility>

namespace mystl {

class FixedSizeMemoryPool {
    
public:
    // FixedSizeMemoryPool(std::size_t block_size, std::size_t blocks_per_page):
    //     block_size_(align_up(block_size)),blocks_per_page_(blocks_per_page),free_list_(nullptr), page_list_(nullptr) {}

    FixedSizeMemoryPool(std::size_t block_size, std::size_t blocks_per_page):
        block_size_(align_up(block_size)),blocks_per_page_(blocks_per_page), page_list_(nullptr) {
            const std::size_t ALIGNED_PAGE_HDR = align_up(sizeof(Page));
            page_size_ = block_size_ * blocks_per_page_ + ALIGNED_PAGE_HDR;
    }

    ~FixedSizeMemoryPool() {
        for (Page* p = page_list_; p != nullptr;) { 
            Page* next = p->next;
            ::operator delete(p);
            p = next;
        }
    }

    std::size_t align_up(std::size_t block_size) {
        std::size_t align = alignof(std::max_align_t); // 对齐数设置为该平台对齐数最大的基本类型的对齐数
        return (block_size + align - 1) & ~(align - 1);
    }

    void* allocate(std::size_t n) {

        if (page_list_) {
            char* free_list_ = page_list_->block_list_;
            if (free_list_) {
                char* block = free_list_;
                page_list_->block_list_ = *reinterpret_cast<char**>(free_list_); // 这里左值千万别写成free_list_!!!
                *reinterpret_cast<char**>(block) = nullptr;            
                return block;
            }
        }
        /* 创建page页链表page_list,以及block空闲链表free_list
        **    注意page地址前8字节不要和第一个block前8字节重叠，否则第二个page对应的第一个block被分出去后，
        **    该block前8字节清0，把第二个页next指针内容清空，导致第一个页的指针丢失了，从而导致内存泄漏!!!
        **    因此，这里page_size不该直接设置为 块大小*页内块个数，前面添加一小块内存专门用于存储页的管理结构！！！
        */

        void* newspace = ::operator new(page_size_);
        Page* page = reinterpret_cast<Page*>(newspace);
        page->next = page_list_; // 新页入页表！！！
        page_list_ = page;    

        char* first_block = reinterpret_cast<char*>((char*)page+align_up(sizeof(Page)));
        std::size_t block_xth = 0;
        while(block_xth < blocks_per_page_-1) {
            char* curr_block = first_block + block_size_ * block_xth;
            char* next_block = curr_block + block_size_;
            *reinterpret_cast<char**>(curr_block) = next_block;
            ++block_xth;
        }
        *reinterpret_cast<char**>(first_block + block_xth * block_size_) = nullptr;
        page->block_list_ = first_block + block_size_;
        *reinterpret_cast<char**>(first_block) = nullptr;

        return first_block;
    }


    void deallocate(void* p) {
        Page *page = get_page(p);
        if (!page) return;
        char* curr_block = static_cast<char *>(p);
        *reinterpret_cast<char**>(curr_block) = page->block_list_;
        page->block_list_ = curr_block;
    }

    template<typename T, typename... Args>
    void construct(void* p, Args&&... args) {
        new (p) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void destroy(void* p) {
        T* pp = static_cast<T*>(p);
        pp->~T();
    }
    
    const void* get_page_list() {
        return page_list_;
    }

    // const void* get_free_list() {
    //     return free_list_;
    // }

private:
    struct Page { 
        Page* next; 
        char* block_list_; // 新加的，专门管理该页的块链表
    };
    std::size_t block_size_;
    std::size_t blocks_per_page_;
    std::size_t page_size_; // // 新加的, 记录page大小
    // char* free_list_; // 修改下代码，页的block链表放到页的管理结构体中去
    Page* page_list_;

    Page* get_page(void* block) {
        for (Page* p = page_list_; p != nullptr; p = p->next) {
            Page* pageStart = p;
            Page* pageEnd = reinterpret_cast<Page*>((char*)p + page_size_);
            if (block > pageStart && block < pageEnd) {
                return p;
            }
        }
        return nullptr;
    }
};

template<typename T>
class LMemoryPoolWrapper {
public:
    using value_type = T;
    LMemoryPoolWrapper() = default;

    FixedSizeMemoryPool& get_pool() {
        static FixedSizeMemoryPool pool_(sizeof(T), 64);
        return pool_;   // 静态局部变量，可以返回引用，因为这个变量存到全局静态数据区，不是栈！！！
    }

    T* allocate(std::size_t n) {
        return static_cast<T*>(get_pool().allocate(n));
    }

    void deallocate(T* p, std::size_t n) {
        get_pool().deallocate(p);
    }

    template<typename... Args>
    void construct(T* p, Args&&... args) {
        get_pool().construct<T>(p, std::forward<Args>(args)...);
    }

    void destroy(T* p) {
        get_pool().destroy<T>(p);
    }
};


}