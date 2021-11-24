#ifndef _PoolBuffer_H_
#define _PoolBuffer_H_

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <sstream>
#include <stack>
#include <utility>
#include <list>
#include <cassert>
#include <algorithm>
#include <thread>

#include "trace.h"

using namespace std::string_literals;

namespace emphaticdaq {
static constexpr uint64_t max_buffer_size = 2 * 1024 * 1024 * 1024u;
static constexpr uint64_t redzone_size = sizeof(uint64_t);
static std::array<uint8_t, redzone_size> redzone_bytes = {'r', 'e', 'd', '-', 'z', 'o', 'n', 'e'};

class PoolBuffer {
 private:
  struct DataBlock {
    DataBlock(uint8_t* begin_, uint8_t* end_, std::size_t size_, std::size_t data_size_, std::size_t index_)
        : begin{begin_}, end{end_}, size{size_}, index{index_}, data_size{data_size_} {
      assert(begin != nullptr);
      assert(end != nullptr);
      assert(size != 0);
    }

  void verify_redzone(){
    if (!std::equal(redzone_bytes.begin(), redzone_bytes.end(),end - redzone_bytes.size())){
      TLOG(TLVL_ERROR) << __func__<<  ": Redzone was overwritten; DataBlock.index="<< index;
      throw std::overflow_error("Redzone was overwritten; DataBlock.index="s + std::to_string(index));
    }
  }

	void clear_data(){
//    if(data_size)  std::generate_n(begin,size,[](){return 0;});
		data_size=0;
	}

    DataBlock() = delete;
    DataBlock(const DataBlock&) = delete;
    DataBlock& operator=(const DataBlock&) = delete;
    DataBlock(DataBlock&&) = delete;
    DataBlock& operator=(DataBlock&&) = delete;
    ~DataBlock() = default;

    uint8_t* const begin;
    uint8_t* const end;
    const std::size_t size;
    const std::size_t index;
    std::size_t data_size;
  };

    enum {
      DEBUGINFO=11,
    };

 public:
  template <typename T>
  struct DataRange {
    using value_type = typename T::value_type;
    typename T::iterator begin;
    typename T::iterator end;
  };

 public:
  bool isEmpty() const {
    std::lock_guard<std::mutex> lock(_freeDataBlocks_mutex);
    return _freeDataBlocks.empty();
  }

  bool isFull() const {
    std::lock_guard<std::mutex> lock(_freeDataBlocks_mutex);
    return _freeDataBlocks.size() == _blockCount;
  }

  std::size_t freeBlockCount() const {
    std::lock_guard<std::mutex> lock(_freeDataBlocks_mutex);
    return _freeDataBlocks.size();
  }

  std::size_t activeBlockCount() const {
    std::lock_guard<std::mutex> lock(_activeDataBlocks_mutex);
    return _activeDataBlocks.size();
  }

  std::size_t blockSize() const noexcept { return _blockSize; }
  std::size_t blockCount() const noexcept { return _blockCount; }
  std::size_t totalBytesRead() const noexcept { return _totalBytesRead; }
  std::size_t totalBytesWritten() const noexcept { return _totalBytesWritten; }
  std::size_t poolBufferSize() const noexcept { return _poolBufferSize; }
  std::size_t totalBlocksRead() const noexcept { return _totalBlocksRead; }
  std::size_t totalBlocksWritten() const noexcept { return _totalBlocksWritten; }
  std::size_t lowWaterMark() const noexcept { return _lowWaterMark; }
  std::size_t fullyDrainedCount() const noexcept { return _fullyDrainedCount; }
  std::size_t setTimeout(std::size_t timeout_usec) noexcept {
    std::swap(_timeout_usec, timeout_usec);
    return timeout_usec;
  }

  template <typename T>
  std::size_t write(const DataRange<T>&);

  template <typename T>
  std::size_t read(DataRange<T>&);

  template <typename T>
  std::size_t write(const std::vector<DataRange<T>>& ranges) {
    auto size = std::size_t{0};
    for (const auto& range : ranges) {
      size += write(range);
    }
    return size;
  }

  template <typename T>
  std::size_t read(std::vector<DataRange<T>>& ranges) {
    auto size = std::size_t{0};
    for (auto& range : ranges) {
      size += read(range);
    }
    return size;
  }

	std::string debugInfo() const;
  std::size_t allocate(std::size_t, std::size_t, bool);

  template <typename T>
  static DataRange<T> make_DataRange(T& t) {
    return DataRange<T>{t.begin(), t.end()};
  }

  PoolBuffer() = default;
  PoolBuffer(const PoolBuffer&) = delete;
  PoolBuffer& operator=(const PoolBuffer&) = delete;
  PoolBuffer(PoolBuffer&&) = delete;
  PoolBuffer& operator=(PoolBuffer&&) = delete;
  ~PoolBuffer() { deallocate(); };

// private:
  std::shared_ptr<DataBlock> takeFreeBlock() {
    std::shared_ptr<DataBlock> block{nullptr};
    std::lock_guard<std::mutex> lock(_freeDataBlocks_mutex);
    auto size = _freeDataBlocks.size();
    if (size > 0) {
      block = _freeDataBlocks.top();
      _freeDataBlocks.pop();
      if (size == 1) _fullyDrainedCount++;
      _lowWaterMark = std::min(_lowWaterMark, size - 1);
    }
    return block;
  }

  std::shared_ptr<DataBlock> takeActiveBlock() {
    std::shared_ptr<DataBlock> block{nullptr};
    std::lock_guard<std::mutex> lock(_activeDataBlocks_mutex);
    if (_activeDataBlocks.size() > 0) {
      block = _activeDataBlocks.front();
      _activeDataBlocks.pop();
    }
    return block;
  }

  void returnFreeBlock(std::shared_ptr<DataBlock> block) {
		block->clear_data();

    std::lock_guard<std::mutex> lock(_freeDataBlocks_mutex);
    _freeDataBlocks.push(block);
    _activeDataBlocks_cv.notify_one();
  }

  void returnActiveBlock(std::shared_ptr<DataBlock> block) {
    std::lock_guard<std::mutex> lock(_activeDataBlocks_mutex);
    _activeDataBlocks.push(block);
    _freeDataBlocks_cv.notify_one();
  }

 private:
  void deallocate() {
		TLOG(TLVL_INFO) << __func__ << ": Deallocating PoolBuffer";
    auto remainingBlockCount=_blockCount;
    debugInfo();
    while(remainingBlockCount) {
    std::lock(_allDataBlocks_mutex, _freeDataBlocks_mutex, _activeDataBlocks_mutex);
    std::lock_guard<std::mutex> lock1(_allDataBlocks_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(_freeDataBlocks_mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lock3(_activeDataBlocks_mutex, std::adopt_lock);
    while (!_freeDataBlocks.empty()){ _freeDataBlocks.pop();remainingBlockCount--;}
    while (!_activeDataBlocks.empty()){_activeDataBlocks.pop();remainingBlockCount--;}
    std::this_thread::yield();
    TLOG(TLVL_DEBUG) << __func__ << ": Waiting for " << remainingBlockCount << " DataBlocks to be rerurned to PoolBuffer";
    }
    _allDataBlocks.clear();
    _rawPoolBuffer.reset(nullptr);
    _rawPoolBuffer = 0;
    _blockCount = 0;
    _blockSize = 0;
    TLOG(TLVL_INFO) << __func__ <<": Deallocated PoolBuffer";
  }

 private:
  std::size_t _blockCount = 0;
  std::size_t _blockSize = 0;
  std::size_t _poolBufferSize = 0;
  std::size_t _enableRedzones = 0;
  std::size_t _timeout_usec = 100;
  std::size_t _totalBytesRead = 0;
  std::size_t _totalBlocksRead = 0;
  std::size_t _totalBytesWritten = 0;
  std::size_t _totalBlocksWritten = 0;
  std::size_t _fullyDrainedCount = 0;
  std::size_t _lowWaterMark = 0;

  std::queue<std::shared_ptr<DataBlock>> _activeDataBlocks;
  mutable std::mutex _activeDataBlocks_mutex;
  std::mutex _activeDataBlocks_cv_mutex;
  std::condition_variable _activeDataBlocks_cv;
  std::stack<std::shared_ptr<DataBlock>> _freeDataBlocks;
  mutable std::mutex _freeDataBlocks_mutex;
  std::condition_variable _freeDataBlocks_cv;
  std::mutex _freeDataBlocks_cv_mutex;
  std::list<std::shared_ptr<DataBlock>> _allDataBlocks;
  mutable std::mutex _allDataBlocks_mutex;

  std::unique_ptr<uint8_t[]> _rawPoolBuffer = {nullptr};
};

template <typename T>
std::size_t PoolBuffer::write(const DataRange<T>& range) {
  using range_value_type = typename DataRange<T>::value_type;

  auto range_size = sizeof(range_value_type) * std::distance(range.begin, range.end);

  if (range_size > _blockSize){
    TLOG(TLVL_ERROR) << __func__ << ": Not enough space; DataRange.size="<< range_size
      <<  " is larger than DataBlock.size="<< _blockSize;
    throw std::length_error("Not enough space; DataRange.size="s + std::to_string(range_size) +
                            " is larger than DataBlock.size="s + std::to_string(_blockSize));
  }
  std::shared_ptr<DataBlock> block = takeFreeBlock();

  if (!block) {
    std::unique_lock<std::mutex> cvlock(_freeDataBlocks_cv_mutex);
    _freeDataBlocks_cv.wait_for(cvlock, std::chrono::microseconds(_timeout_usec));
    block = takeFreeBlock();
  }

  if (!block) return 0;

  assert(range_size <= block->size);
  auto block_begin = (range_value_type*)block->begin;
  std::copy(range.begin, range.end, block_begin);
  block->data_size = range_size;
  _totalBytesWritten += range_size;
  _totalBlocksWritten++;
  returnActiveBlock(block);

  return range_size;
}

template <typename T>
std::size_t PoolBuffer::read(DataRange<T>& range) {
  using range_value_type = typename DataRange<T>::value_type;
  auto range_length=  std::distance(range.begin, range.end);
  auto range_size = sizeof(range_value_type) * range_length;

  if (range_size > _blockSize){
    TLOG(TLVL_ERROR) << __func__ << ": Not enough space; DataRange.size="
       << range_size <<" is smaller than DataBlock.size=" << _blockSize;

    throw std::length_error("Not enough space; DataRange.size="s + std::to_string(range_size) +
                            " is smaller than DataBlock.size="s + std::to_string(_blockSize));
  }
  std::shared_ptr<DataBlock> block = takeActiveBlock();

  if (!block) {
    std::unique_lock<std::mutex> cvlock(_activeDataBlocks_cv_mutex);
    _activeDataBlocks_cv.wait_for(cvlock, std::chrono::microseconds(_timeout_usec));
    block = takeActiveBlock();
  }

  if (!block) return 0;

  assert(range_size <= block->size);
  assert(block->size >= block->data_size);

  if (_enableRedzones != 0) block->verify_redzone();

  auto block_begin = (range_value_type*)block->begin;
  std::copy(block_begin, block_begin + range_length, range.begin);
	block->clear_data();

  _totalBytesRead += range_size;
  _totalBlocksRead++;
  returnFreeBlock(block);

  return range_size;
}

inline std::size_t PoolBuffer::allocate(std::size_t block_size, std::size_t poolbuffer_size = max_buffer_size,
                                        bool enable_redzones = false) {
  if (_poolBufferSize != 0) throw std::logic_error("Reallocation of PoolBuffer is not allowed.");

  if (poolbuffer_size == 0 || poolbuffer_size > max_buffer_size)
    throw std::length_error("Invalid PoolBuffer size (size="s + std::to_string(poolbuffer_size) +
                            ";  0 < size <= max_buffer_size="s + std::to_string(max_buffer_size));

  auto align_cacheline = [](auto size) { return (size % 64 != 0) ? ((size >> 6) + 1) << 6 : size; };

  std::lock(_allDataBlocks_mutex, _freeDataBlocks_mutex, _activeDataBlocks_mutex);
  std::lock_guard<std::mutex> lock1(_allDataBlocks_mutex, std::adopt_lock);
  std::lock_guard<std::mutex> lock2(_freeDataBlocks_mutex, std::adopt_lock);
  std::lock_guard<std::mutex> lock3(_activeDataBlocks_mutex, std::adopt_lock);
  _enableRedzones = enable_redzones;
  _blockSize = block_size;
  block_size = align_cacheline(block_size + (_enableRedzones == 0 ? 0 : redzone_size));
  _blockCount = poolbuffer_size / block_size;
  _lowWaterMark = _blockCount;
  poolbuffer_size = align_cacheline(block_size * _blockCount);

  assert(poolbuffer_size != 0 && poolbuffer_size <= max_buffer_size);

  _rawPoolBuffer.reset(new uint8_t[poolbuffer_size]);
  if (!_rawPoolBuffer) throw std::length_error("Failed allocating "s + std::to_string(poolbuffer_size) + " bytes."s);
  _poolBufferSize = poolbuffer_size;

  auto buf = _rawPoolBuffer.get();

  for (std::size_t i = 0; i < _poolBufferSize; i += 64) buf[i] = '*';

  for (std::size_t i = 0; i<  _blockCount ;  i++) {
    _allDataBlocks.push_front(
        std::make_shared<DataBlock>(buf + i * block_size, buf + (i + 1) * block_size, _blockSize, i, 0));
    if (_enableRedzones != 0) {
      std::copy(redzone_bytes.begin(), redzone_bytes.end(),
                _allDataBlocks.front()->begin + (block_size - redzone_bytes.size()));
    }
  }
  for (auto& block : _allDataBlocks) _freeDataBlocks.push(block);
  _freeDataBlocks_cv.notify_all();
  return _poolBufferSize;
}
}
#endif  // _PoolBuffer_H_
