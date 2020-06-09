#ifndef __U_MYDEQ__
#define __U_MYDEQ__

#include <memory.h>
#include <memory>
#include <vector>

template <typename T>
class mydeq {
public:
	mydeq(int size);
	void push(const T* buf, int num);
	int get_array(T* dst);
	int get_num_data();
	std::shared_ptr<std::vector<T>> pop(int num);
	void clear();

private:
	int size_;
	std::vector<T> buf_;
	int idx_start_;
	int idx_end_;
};

template<class T>
mydeq<T>::mydeq(int size)
{
	size_ = size+1;
	buf_.resize(size_);
	idx_start_ = 0;
	idx_end_ = 0;
}

template<class T>
void mydeq<T>::push(const T* buf, int num)
{
	if(num <= 0) return;
	if(num > size_-1){
		buf += (num-(size_-1));
		num = size_-1;
	}
	int idst = idx_end_;
	int w0 = num;
	if(idst + num > size_ - 1){
		w0 = size_ - idst;
		if(idx_start_ > idx_end_ || idx_start_ == 0){
			idx_start_ = 1;
		}
		idx_end_ = 0;
	}else{
		if(idx_start_ > idx_end_){
			idx_end_ += w0;
			if(idx_end_ >= idx_start_){
				idx_start_ = idx_end_+1;
			}
		}else{
			idx_end_ += w0;
		}
	}
	memcpy(&buf_[idst], buf+0, sizeof(T) * w0);
	int w1 = num - w0;
	if(w1 == 0){
		return;
	}
	idx_end_ = w1;
	if(idx_end_ >= idx_start_){
		idx_start_ = idx_end_ + 1;
	}
	memcpy(&buf_[0], buf+w0, sizeof(T) * w1);
}

template<class T>
int mydeq<T>::get_array(T* dst)
{
	if(idx_start_ == idx_end_) return 0;
	if(idx_end_ > idx_start_){
		int num = idx_end_ - idx_start_;
		memcpy(dst+0, &buf_[idx_start_], num * sizeof(T));
		return num;
	}else{
		int w0 = size_ - idx_start_;
		memcpy(dst, &buf_[idx_start_], w0 * sizeof(T));
		int w1 = idx_end_;
		if(w1 == 0) return w0;
		memcpy(dst+w0, &buf_[0], w1 * sizeof(T));
		return w0+w1;
	}
}

template<class T>
int mydeq<T>::get_num_data()
{
	if(idx_end_ >= idx_start_){
		return idx_end_ - idx_start_;
	}else{
		return size_ - (idx_start_ - idx_end_);
	}
}

template<class T>
std::shared_ptr<std::vector<T>> mydeq<T>::pop(int num)
{
	int num0 = get_num_data();
	if(num > num0) num = num0;
	auto ret = std::make_shared<std::vector<T>>();
	if(num == 0) return ret;
	ret->resize(num);
	if(idx_end_ >= idx_start_){
		memcpy(&(*ret)[0], &buf_[idx_start_], num * sizeof(T));
		idx_start_ += num;
		if(idx_start_ > size_){
			idx_start_ -= size_;
		}
	}else{
		if(idx_start_ + num > size_){
			int num0 = size_ - idx_start_;
			memcpy(&(*ret)[0], &buf_[idx_start_], num0 * sizeof(T));
			int num1 = num - num0;
			memcpy(&(*ret)[num0], &buf_[0], num1 * sizeof(T));
			idx_start_ = num1;
		}else{
			memcpy(&(*ret)[0], &buf_[idx_start_], num * sizeof(T));
			idx_start_ += num;
			if(idx_start_ > size_){
				idx_start_ = 0;
			}
		}
	}
	
	return ret;
}

template<class T>
void mydeq<T>::clear()
{
	idx_end_ = idx_start_;
}

#endif
