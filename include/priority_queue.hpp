#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include <iostream>
#include <map>

namespace data {

/**
 * PriorityQueue implements a queue where data can be queued
 * based on external priorities that can be any type which operator< is defined for.
 * Internally a std::multimap is used for managing the tuples of priority and connected data.
 * Thus, the lower a priority value as defined by the operator< relation is the higher
 * it is considered in terms of priority.
 */
template< class T, class U > class PriorityQueue {
	public:
		/**
		 * Constant iterator type for PriorityQueue.
		 */
		typedef typename std::multimap< T, U >::const_iterator const_iterator;
		/**
		 * Iterator type for PriorityQueue.
		 */
		typedef typename std::multimap< T, U >::iterator iterator;
		/**
		 * Type of size values used and returned by various methods.
		 */
		typedef typename std::multimap< T, U >::size_type size_type;

		/**
		 * Constructor
		 */
		PriorityQueue() {}
		/**
		 * Destructor
		 */
		virtual ~PriorityQueue() {}

		/**
		 * Returns constant iterator to the beginning (i. e. top) of the queue.
		 * @returns Iterator referencing the top.
		 */
		const_iterator begin() const {
			return values.begin();
		}

		/**
		 * Returns iterator to the beginning (i. e. top) of the queue.
		 * @returns Iterator referencing the top.
		 */
		iterator begin() {
			return values.begin();
		}

		/**
		 * Returns constant iterator to the end (i. e. back) of the queue.
		 * @returns Iterator referencing the end.
		 */
		const_iterator end() const {
			return values.end();
		}

		/**
		 * Returns iterator to the end (i. e. back) of the queue.
		 * @returns Iterator referencing the top.
		 */
		iterator end() {
			return values.end();
		}

		/**
		 * Inserts an element into the queue with the given priority.
		 * @param _priority The priority to be used for the element.
		 * @param _item The element to insert.
		 * @returns Iterator referencing the inserted element.
		 */
		iterator insert(T _priority, U _item) {
			return values.insert(std::pair< T, U >(_priority, _item));
		}

		/**
		 * Removes the element referenced by the given iterator from the queue.
		 * @param _it Iterator referencing element to be removed.
		 */
		void erase(iterator &_it) {
			values.erase(_it);
		}

		/**
		 * Removes top element of the queue.
		 */
		void pop() {
			values.erase(values.begin());
		}

		/**
		 * Gives the stored priority of the top element of the queue.
		 * @returns Priority of top element.
		 */
		const T &top_priority() const {
			return begin()->first;
		}

		/**
		 * Returns reference to the top element.
		 * @returns Reference to constant top element of the queue.
		 */
		const U &top() const {
			return begin()->second;
		}

		/**
		 * Returns reference to the top element.
		 * @returns Reference to top element of the queue.
		 */
		U &top() {
			return begin()->second;
		}

		/**
		 * Entirely remove all stored elements.
		 */
		void clear() {
			values.clear();
		}

		/**
		 * Tells whether the queue is empty or not.
		 * @returns True if queue is empty, false otherwise.
		 */
		bool empty() const {
			return values.empty();
		}

		/**
		 * Tells the current size of the queue, i. e. the number of stored elements.
		 * @returns Current size of the queue.
		 */
		size_type size() const {
			return values.size();
		}

	protected:
		std::multimap< T, U > values;
};

/**
 * Overloaded operator<< printing out a PriorityQueue.
 * @param _out Reference to std::ostream to be used for printing.
 * @param _q PriorityQueue to print.
 * @returns Given std::ostream reference for concatenating operator<<.
 * @see PriorityQueue
 */
template< class T, class U > std::ostream &operator<<(std::ostream &_out, const PriorityQueue< T, U > &_q) {
	_out << "Priority queue:" << std::endl;
	for(typename PriorityQueue< T, U >::const_iterator it = _q.begin(); it != _q.end(); it++) {
		_out << "- " << it->first << " -> " << it->second << std::endl;
	}
	return _out;
}

}

#endif

