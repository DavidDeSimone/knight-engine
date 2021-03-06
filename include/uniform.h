#pragma once

#include "common.h"
#include "shader_types.h"

#include <GL/gl3w.h>
#include <memory.h>
#include <gsl.h>
#include <hash.h>

#include <string>

namespace knight {

class UniformBase {
 public:
  UniformBase(foundation::Allocator &alloc, MaterialManager &manager, std::string name)
      : manager_{manager},
        name_{name},
        materials_{alloc} { }

  virtual ~UniformBase() { }

  gsl::czstring<> name() { return name_.c_str(); }

  virtual void push(GLint location) = 0;
  virtual UniformBase *clone(foundation::Allocator &alloc) const = 0;

  void add_material(const Material &mat, GLint location);
  void notify_dirty();

  MaterialManager &manager_;
  std::string name_;
  foundation::Hash<const Material *> materials_; 
 
 private:
  KNIGHT_DISALLOW_COPY_AND_ASSIGN(UniformBase);
  KNIGHT_DISALLOW_MOVE_AND_ASSIGN(UniformBase);
};

template<typename T, size_t row_count, size_t col_count = 1>
class Uniform : public UniformBase {
 public:
  using Type = T;

  using UniformBase::UniformBase;

  virtual void push(GLint location);
  virtual UniformBase *clone(foundation::Allocator &alloc) const;

  void set_value(const T *values);

  static const size_t kElementCount = row_count * col_count;
  static const size_t kElementSize = sizeof(T);
  static const size_t kTotalElementSize = kElementCount * kElementSize;

  T elements_[kElementCount];

 private:
  KNIGHT_DISALLOW_COPY_AND_ASSIGN(Uniform);
  KNIGHT_DISALLOW_MOVE_AND_ASSIGN(Uniform);
};

template<typename T, size_t row_count, size_t col_count>
UniformBase *Uniform<T, row_count, col_count>::clone(foundation::Allocator &alloc) const {
  auto clone = alloc.make_new<Uniform<T, row_count, col_count>>(alloc, manager_, name_);
  std::memcpy(clone->elements_, elements_, kTotalElementSize);
  return clone;
}

template<typename T, size_t row_count, size_t col_count>
void Uniform<T, row_count, col_count>::set_value(const T *values) {
  if (std::memcmp(elements_, values, kTotalElementSize) != 0) {
    std::memcpy(elements_, values, kTotalElementSize);
    notify_dirty();
  }
}

} // namespace knight
