#ifndef LIGHTBLAS_TEST_HELPER_H_
#define LIGHTBLAS_TEST_HELPER_H_

#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

#define TIMER_START(timer) \
	timer = std::chrono::high_resolution_clock::now()

#define TIMER_STOP(timer) \
	std::chrono::duration_cast<std::chrono::microseconds> \
		(std::chrono::high_resolution_clock::now() - timer).count() / 1000.

using Shape = std::vector<int>;
using std::vector;
using std::shared_ptr;
using std::string;
using std::ostringstream;
using CPUTimer = std::chrono::high_resolution_clock::time_point;

class Data {
public:
  Data(){}
  Data(const Shape& shape){
    Reshape(shape);
  }

  void Reshape(const Shape& shape){
    shape_ = shape;
    count_ = 1;
    for (int i = 0; i < shape_.size(); ++i){
      assert(shape_[i] > 0);
      count_ *= shape_[i];
    }
  }

  void RandomInit(){
    float* d = data();
    srand(time(0));
    for (int c = 0; c < count_; ++c){
      d[c] = (float)rand() / RAND_MAX * 2 - 1;
    }
  }

  vector<int> shape() const {return shape_;}
  const int shape(const int index) const {
    assert(index < shape_.size());
    return shape_[index];
  }
  string shape_string(){
    ostringstream oss;
    oss << "[";
    for (int i = 0; i < shape_.size(); ++i){
      oss << (i == 0 ? "" : ", ") << shape_[i];
    }
    oss << "]";
    return oss.str();
  }
  const int count(const int start, const int end) const{
    assert(start >= 0);
    assert(start < end);
    assert(end <= shape_.size());
    int c = 1;
    for (int i = start; i < end; ++i){
      assert(shape_[i] > 0);
      c *= shape_[i];
    }
    return c;
  }
  const int count(const int start = 0) const{
    return count(start, shape_.size());
  }

  float* data() {
    if (count_ > capacity_) {
      data_.reset(new float[count_]);
      capacity_ = count_;
    }

    return data_.get();
  }

  bool CheckEqual(Data& other, const bool check_shape = true){
    bool pass = true;
    if (count_ != other.count()) {
      return false;
    }

    if (check_shape){
      if (shape_.size() != other.shape().size()) {
        std::cout << "Check shape equal failed: "
		<< shape_string() << ": " << other.shape_string();
	return false;
      }
      for (int i = 0; i < shape_.size(); ++i){
        if (shape_[i] != other.shape(i)) {
	  std::cout << "Check shape equal failed: "
		  << shape_string() << ": " << other.shape_string();
	  return false;
	}
      }
    }

    const float* src_data = data();
    const float* dest_data = other.data();

    for (int c = 0; c < count_; ++c){
      if (src_data[c] - dest_data[c] <= -0.0001
	|| src_data[c] - dest_data[c] >= 0.0001){
        vector<int> index(shape_.size());
	int v = c;
	for (int i = index.size() - 1; i >= 0; --i){
	  index[i] = v % shape_[i];
	  v = (v - index[i]) / shape_[i];
	}
	std::cout << "Error at index: [";
	for (int i = 0; i < index.size(); ++i){
	  std::cout << (i == 0 ? "" : ", ") << index[i];
	}
	std::cout << "]:";
	std::cout << src_data[c] << "(" << dest_data[c]
		<< ")\n";
	pass = false;
      }
    }

    return pass;
  }
private:
  Shape shape_;
  int count_{0};
  int capacity_{0};

  shared_ptr<float> data_;
};

#endif
