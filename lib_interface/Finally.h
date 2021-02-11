//
// Created by Petr on 03.04.2020.
//

#ifndef LIB_TEST_FINALLY_H
#define LIB_TEST_FINALLY_H
/**
 * RAII version of try... finally... construct
 * @tparam F callable
 */
template<typename F>
class Finally final {
 public:
  explicit Finally(F &&callable) : callable(callable) {}
  Finally(const Finally &) = delete;
  Finally(Finally &&) = delete;
  Finally &operator=(const Finally &) = delete;
  Finally &operator=(Finally &&) = delete;
  ~Finally() { callable(); }

 private:
  std::function<void()> callable;
};
#endif// LIB_TEST_FINALLY_H
