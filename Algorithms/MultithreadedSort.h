#ifndef MULTITHREADED_SORT_H
#define MULTITHREADED_SORT_H
#include <DataStructure.h>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <iterator>
#include <fstream>
namespace parallel {

	template<typename Iter>
	class MergeSortNoCopy{
		struct Merge{

			signed char level;

			Iter _begin;
			Iter _mid;
			Iter _end;
			int id;

			bool operator <(const Merge &_m)const {
				if (level<_m.level) {
					return true;
				}
				else if (level == _m.level) {
					return id<_m.id;
				}
				return false;
			}

			bool is_next(const Merge &_m)const {
				if (level == _m.level) {
					return (id + 1) == _m.id;
				}
				return false;
			};

			void set_end(const Iter &__end) {
				_mid = _end;
				_end = __end;
			}

			Merge(const Iter &__begin,
				const Iter &__end,
				signed char _level,
				int _id) :_begin(__begin),
				_end(__end),
				level(_level),
				id(_id) {
			};

			void set(const Iter &__begin,
				const Iter &__mid,
				const Iter &__end,
				signed char _level,
				int _id) {

				_begin = __begin;
				_mid = __mid;
				_end = __end;
				level = _level;
				id = _id;
			}

			~Merge() {
			}

			Merge() {
			};

		private:
			Merge& operator=(const Merge&);

			Merge(const Merge&);

		};
		typedef BlockingQueue<Merge> Buffer;

		struct MergeCompare{
			bool operator ()(const Merge *_m1,
			const Merge *_m2)const {
				if (_m1->level<_m2->level) {
					return true;
				}
				else if (_m1->level == _m2->level) {
					return _m1->id<_m2->id;
				}
				return false;
			}
		};

		void generate_merge_steps(int _size,
			int _count,
			Buffer *_merge_steps_to_run,
			Buffer *_merge_steps_complete) {



			std::vector<int>level_count;

			auto e = _size;
			std::size_t x = std::ceil((double)_size / (double)_count);
			e = x;
			level_count.reserve(x + 1);
			while (x != 1) {
				level_count.push_back(x);
				x = (e + 1) / 2;
				e = x;

			}
			level_count.push_back(1);


			std::set<Merge*, MergeCompare>current_steps;
			Merge *m;
			Merge *next_merge;
			Merge *neighbor;
			while (_merge_steps_complete->Remove(&m)) {
				if (m) {
					if (std::distance(m->_begin, m->_end) == _size) {
						delete m;
						_merge_steps_complete->ShutDown();
						_merge_steps_to_run->ShutDown();
						return;
					}
					int total_ids = level_count[m->level];
					if (total_ids % 2 == 1) {
						if (total_ids == m->id) {
							//promote the last one of the odd numbers
							m->id = (m->id + 1) / 2;
							++m->level;
							_merge_steps_complete->Insert(m);
							continue;

						}
					}
					current_steps.insert(m);
				}

				for (auto iter = current_steps.begin(); iter != current_steps.end();) {
					auto innerIter = iter;
					++innerIter;
					next_merge = *iter;
					if (innerIter != current_steps.end()) {
						neighbor = *innerIter;
						if (next_merge->is_next(*neighbor) &&
							(next_merge->id % 2 == 1)) {

							next_merge->set_end(neighbor->_end);
							++next_merge->level;
							next_merge->id = (neighbor->id + 1) / 2;
							_merge_steps_to_run->Insert(next_merge);
							delete *innerIter;
							iter = current_steps.erase(iter, ++innerIter);
						}
						else
							++iter;
					}
					else
						break;
				}
			}
		}
		void thread_sort(Buffer *_merge_steps_to_run,
			Buffer *_merge_steps_complete) {
			Merge *m;

			while (_merge_steps_to_run->Remove(&m)) {
				if (m->level == -1) {
					std::sort(m->_begin, m->_end);
					m->level = 0;
				}
				else
					std::inplace_merge(m->_begin, m->_mid, m->_end);
				_merge_steps_complete->Insert(m);
			}
		}
		void threaded_sort(Iter _begin, Iter _end, int _thread_count) {

			boost::thread_group sortingThreads;
			Buffer merge_steps_to_run, merge_steps_complete;
			int thread_count = _thread_count;
			auto total_size = std::distance(_begin, _end);
			int min_count = (total_size / thread_count);
			if ((1 + ((32 * 1024) / sizeof(typename Iter::value_type)))>min_count) {
				min_count = (1 + ((32 * 1024) / sizeof(typename Iter::value_type)));
				thread_count = max(total_size / min_count, 1);
			}

			for (int i = 0; i<thread_count; i++) {

				sortingThreads.create_thread(boost::bind(boost::mem_fn(&MergeSortNoCopy::thread_sort), boost::ref(this), &merge_steps_to_run, &merge_steps_complete));

			}

			sortingThreads.create_thread(boost::bind(boost::mem_fn(&MergeSortNoCopy::generate_merge_steps), boost::ref(this), total_size, min_count, &merge_steps_to_run, &merge_steps_complete));

			std::size_t i_count = 0;

			int index = 1;

			std::size_t array_size = min(min_count, total_size - i_count);
			while (i_count != total_size) {

				array_size = min(min_count, total_size - i_count);
				Merge *m = new Merge(std::next(_begin, i_count), std::next(_begin, i_count + array_size), -1, index);
				i_count += array_size;

				merge_steps_to_run.Insert(m);
				index++;

			}


			sortingThreads.join_all();
		}

	public:
		void sort(Iter _begin, Iter _end, int _thread_count) {

			threaded_sort(_begin, _end, _thread_count);
		}
	};

	template<typename Iter, typename Pred>
	void sort(Iter begin, Iter end, Pred _p,
		unsigned int N = std::thread::hardware_concurrency() / 2) {
		auto len = std::distance(begin, end);
		if (len <= 1024 || N < 2) {
			std::sort(begin, end, _p);
			return;
		}

		Iter mid = std::next(begin, len / 2);
		auto fn = std::async(sort<Iter, Pred>, begin, mid, _p, N - 2);
		sort<Iter, Pred>(mid, end, _p, N - 2);
		fn.wait();
		std::inplace_merge(begin, mid, end, _p);
	}


	// parallel merge sort
	template<typename Iter>
	void sort(Iter begin, Iter end,
		unsigned int N = std::thread::hardware_concurrency() / 2) {
		// find the number of elements being sorted
		auto len = std::distance(begin, end);
		if (len <= 1024 || N < 2) {
			std::sort(begin, end);
			return;
		}

		// find the mid
		Iter mid = std::next(begin, len / 2);

		// start the next stage of the divide step asynchronously
		auto fn = std::async(sort<Iter>, begin, mid, N - 2);
		auto fn2 = std::async(sort<Iter>, mid, end, N - 2);

		// wait for the divide steps to finish
		fn.wait();
		fn2.wait();
		// merge the result
		std::inplace_merge(begin, mid, end);
	}
};

#endif