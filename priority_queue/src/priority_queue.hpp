#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */

template<typename T>
struct node	{
	T data;
	int order;
	node* child;
	node* parent;
  node* sibling;
};

template<typename T>
node<T>* make_node() {
  node<T>* temp;
	temp = (node<T>*)malloc(sizeof(node<T>));
	temp->order = 0;
  temp->child = nullptr;
	temp->parent = nullptr;
	temp->sibling = nullptr;
	return temp;
};

template<typename T>
node<T>* make_node(const T &value) {
  node<T>* temp;
	temp = (node<T>*)malloc(sizeof(node<T>));
	new (&temp->data) T(value);
	temp->order = 0;
  temp->child = nullptr;
	temp->parent = nullptr;
	temp->sibling = nullptr;
	return temp;
}

//把两棵度数均为n的二项树连接成一棵度数为(n+1)的二项树，heap2的根节点作为根节点
template<typename T>
void Link(node<T>* heap1, node<T>* heap2) {
  heap1->parent = heap2;
	heap1->sibling = heap2->child;
	heap2->child = heap1;
	heap2->order = heap2->order + 1;
}

//把两个森林的所有二项树按照根节点的度数链接起来
template<typename T>
node<T>* Merge(node<T>* heap1, node<T>* heap2) {
  if (heap1 == nullptr) return heap2;
	if (heap2 == nullptr) return heap1;
	node<T>* new_heap = nullptr;
  if (heap1->order < heap2->order) {
		new_heap = heap1;
		new_heap->sibling = Merge(heap1->sibling, heap2);
	} else {
		new_heap = heap2;
		new_heap->sibling = Merge(heap2->sibling, heap1);
	}
	return new_heap;
}

template<typename T, class Compare = std::less<T>>
class priority_queue {
public:
	/**
	 * @brief default constructor
	 */
	priority_queue() {
		head = nullptr;
		length = 0;
	}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) {
		this->head = nullptr;
		if (other.head != nullptr) {
		  this->length = other.length;
			head = copy_node(other.head, nullptr);
		}
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear(head);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
    if (this != &other) {
			clear(head);
			head = nullptr;
			length = other.length;
			if (other.head != nullptr) head = copy_node(other.head, nullptr);
	  }
	  return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if ((*this).empty()) {
			throw container_is_empty();
		}
		Compare comp;
		node<T>* ans = head;
    node<T>* cur = head;
		while (cur != nullptr) {
			try {
				comp(ans->data, cur->data);
			} catch(...) {
				throw;
			}
			if (comp(ans->data, cur->data)) {
				ans = cur;
			}
		  cur = cur->sibling;
		}
		return ans->data;
  }

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		Compare comp;
    node<T>* temp = make_node(e);
    temp->order = 0;
		int init_length = length;
    if (head == nullptr) {
      head = temp;
    } else {
      priority_queue temp_queue;
      temp_queue.head = temp;
      temp_queue.length = 1;
			try {
				comp(head->data, e);
			} catch (...) {
				throw;
			}
			try {
        merge(temp_queue);
			} catch (...) {
				temp->data.~T();
				free(temp);
				throw;
			}
			temp_queue.head = nullptr;
    }
    length = init_length + 1;
}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		Compare comp;
		//获取node
    if ((*this).empty()) {
			throw container_is_empty();
		}
		int f_length = length;
		node<T>* top = head;
    node<T>* cur = head;
		try {
		  while (cur != nullptr) {
		  	if (comp(top->data, cur->data)) {
		  		top = cur;
		  	}
		    cur = cur->sibling;
		  }
		} catch(...) {
			throw;
		}

		node<T> *pre, *pos;
		pre = nullptr;
		pos = head;
		if (top == head) {
			head = top->sibling;
		} else {
			cur = head;
			while (cur->sibling != top) {
				cur = cur->sibling;
			} 
			cur->sibling = top->sibling;
		}

		//把删去根节点的子树按照度数从小到大排列
		node<T>* temp = top->child;
    node<T> *next, *tail;
		tail = nullptr;
		if (temp == nullptr) {
			temp = nullptr;
		} else {
			temp->parent = nullptr;
			while (temp->sibling != nullptr) {
				next = temp->sibling;
				temp->sibling = tail;
				tail = temp;
				temp = next;
				temp->parent = nullptr;
			}
			temp->sibling = tail;
		  length--;
		}
    
		//重新merge
		priority_queue tmp;
		tmp.head = temp;
		try {
		  if (temp != nullptr) (*this).merge(tmp);
		  this->length = f_length - 1;
		} catch (...) {
		  this->head = Merge(head, temp);
		  this->length = f_length;
		  throw;
		}
		top->data.~T();
		free(top);
		tmp.head = nullptr;
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
    return length;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return (head == nullptr);
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		Compare comp;
		if (other.empty() || this == &other) return; 
    priority_queue temp;
		node<T>* Head = nullptr;
		Head = Merge((*this).head, other.head);
		temp.head = Head;
		if (Head == nullptr) { (*this).head = nullptr; return; }   
    node<T> *pre, *cur, *nxt;
		pre = nullptr;
		cur = Head;
		nxt = Head->sibling;
		try {
		  while (nxt != nullptr) {
		  	//度数不同无需合并，有且只有这两棵度数相同需要合并
		  	if ((cur->order != nxt->order)
		  			||((nxt->sibling != nullptr)
		  			   && (nxt->order == nxt->sibling->order))) {
          pre = cur;
		  		cur = nxt;
		  	} else if (comp(nxt->data, cur->data)) {//把nxt作为cur的子树合并
		  		cur->sibling = nxt->sibling;
		  		nxt->parent = cur;
		  		Link(nxt, cur);
		  	} else {//把cur作为nxt的子树合并
		  		if (pre == nullptr) Head = nxt;
		  		else pre->sibling = nxt;
		  		Link(cur, nxt);
		  		cur = nxt;
		  	}
			  nxt = cur->sibling;
		  }
		} catch(...) {
			Head = nullptr;
      throw;
		}
	  head = Head;
		temp.head = nullptr;
		other.head = nullptr;
  }
 private:
  node<T>* head;
  size_t length;

	void clear(node<T>* heap) {
		if (heap == nullptr) {
		  return;
		} else {
		  if (heap->child != nullptr) clear(heap->child);
		  if (heap->sibling != nullptr) clear(heap->sibling);
		  heap->data.~T();
		  free(heap);
		  heap = nullptr;
		}
	}

	node<T>* copy_node(const node<T>* src, node<T>* parent) {
    if (!src) return nullptr;
    node<T>* newNode = (node<T>*)malloc(sizeof(node<T>));
    new (&newNode->data) T(src->data);
    newNode->order = src->order;
    newNode->parent = parent;
    newNode->child = copy_node(src->child, newNode);
    newNode->sibling = copy_node(src->sibling, parent);
    return newNode;
}
};

}

#endif