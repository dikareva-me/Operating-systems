#pragma once

#include <vector>
#include <pthread.h>

#include "iset.h"


class SetTester {
public:
    struct TestInfo_t {
    public:
        size_t threads_num;
        size_t numbers_num;
        
    public:
        TestInfo_t(TestInfo_t const&) = default;
        ~TestInfo_t() {}
    };

private:
    using data_t = std::vector<std::vector<int>>;

    struct Arg_t {
        ISet* set;
        std::vector<int> vec;
        std::vector<int>* check_vec;

        ~Arg_t() {}
    };

    struct TestResult_t {
        bool is_passed;
        double time;
    };

    enum class DataType {
        Fix = 0,
        Random
    };

private:
    static size_t const read_attemps_num = 7;
    static double constexpr time_between_read_attempts = 1.0;
    static size_t const tests_repeat_num = 1000;

    static size_t const progress_info_times = 10;

    ISet* set;
    
    std::vector<int> nums;

public:
    static SetTester* create(ISet* set);

    void test(TestInfo_t const& test_info);

    TestResult_t writing_test(data_t const& data);
    TestResult_t reading_test(data_t const& data);
    TestResult_t mix_test(data_t const& data);

    ~SetTester();

private:
    static void* write(void* ptr_arg);
    static void* read(void* ptr_arg);

    SetTester(ISet* set);

    void test(TestInfo_t const& test_info, bool update_data);
    void print_test_result(TestResult_t const& tr) const;

    data_t generate_fix_data(size_t arrays_num, size_t items_num) const;
    data_t generate_random_data(size_t arrays_num, size_t items_num) const;
    data_t reshape_data(data_t const& src_data, size_t new_size) const;

    bool check_vector(std::vector<int> const& check_vec) const;

    void update_results(TestResult_t& tr, TestResult_t const& new_tr) const;

    std::vector<Arg_t*> setup_threads(std::vector<pthread_t>& threads, 
                                      data_t const& data, 
                                      void* func(void*),
                                      std::vector<int>* check_vector = nullptr) const;

};
