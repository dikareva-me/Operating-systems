#include "set_tester.h"

#include <iostream>
#include <chrono>
#include <random>
#include <pthread.h>
#include <unistd.h>


SetTester* SetTester::create(ISet* set) {
    if (set == nullptr) {
        return nullptr;
    }

    return new SetTester(set);
}

void* SetTester::write(void* ptr_arg) {
    Arg_t* arg = (Arg_t*)ptr_arg;
    ISet* set = arg->set;
    
    for (auto item : arg->vec) {
        if (!set->add(item)) {
            std::cout << "Can not add item to set" << std::endl;
        }
    }

    return nullptr;
}

void* SetTester::read(void* ptr_arg) {
    Arg_t* arg = (Arg_t*)ptr_arg;
    ISet* set = arg->set;
    std::vector<int>* check_vector = arg->check_vec;

    for (auto item : arg->vec) {
        auto start_time = std::chrono::high_resolution_clock::now();
        auto end_time = start_time;

        while(std::chrono::duration<double>(end_time - start_time).count() < time_between_read_attempts) {
            if (set->contains(item)) {
                if (!set->remove(item)) {
                    std::cout << "Can not remove item from set" << std::endl;
                }

                if (check_vector) {
                    ++(*check_vector)[item];
                }

                break;
            }

            end_time = std::chrono::high_resolution_clock::now();
        }
    }

    return nullptr;
}

SetTester::SetTester(ISet* set) : set{ set } {}

SetTester::~SetTester() {
    delete set;
}

void SetTester::test(TestInfo_t const& test_info) {
    std::cout << "Fix data" << std::endl;
    test(test_info, false);

    std::cout << "\n################################\n" << std::endl;

    std::cout << "Random data" << std::endl;
    test(test_info, true);
}

void SetTester::test(TestInfo_t const& test_info, bool update_data) {
    data_t data = generate_fix_data(test_info.threads_num, test_info.numbers_num);

    TestResult_t writing_tr = {true, 0};
    TestResult_t reading_tr = {true, 0};
    TestResult_t mix_tr = {true, 0};

    size_t d = tests_repeat_num / progress_info_times;
    size_t progress_counter = 1;

    for (size_t i = 0; i < tests_repeat_num; ++i) {
        if (!set->is_empty()) {
            std::cout << "TEST ERROR: set must be empty" << std::endl;
        }
        if (update_data) {
            data = generate_random_data(test_info.threads_num, test_info.numbers_num);
        }

        update_results(writing_tr, writing_test(data));
        update_results(reading_tr, reading_test(data));
        update_results(mix_tr, mix_test(data));

        if (i + 1 >= d * progress_counter) {
            std::cout << "Progress: " << progress_counter * progress_info_times << "%" << std::endl;
            ++progress_counter;
        }
    }

    std::cout << "Test results (test num = " <<  tests_repeat_num << std::endl;

    std::cout << "Writing test:" << std::endl;
    print_test_result(writing_tr);

    std::cout << "Reading test:" << std::endl;
    print_test_result(reading_tr);

    std::cout << "Mix test:" << std::endl; 
    print_test_result(mix_tr);
}

void SetTester::print_test_result(TestResult_t const& tr) const {
    std::cout << (tr.is_passed ? "Test is passed" : "Test is failed")
              << " | "
              << "Average time (milliseconds): "
              << tr.time / tests_repeat_num
              << std::endl;          
}

SetTester::TestResult_t SetTester::writing_test(data_t const& data) {
    std::vector<pthread_t> threads(data.size());

    auto start_time = std::chrono::high_resolution_clock::now(); 
    std::vector<Arg_t*> args = setup_threads(threads, data, write);

    bool success = true;

    {
        for (auto& thread : threads) {
            if (pthread_join(thread, nullptr) != 0) {
                std::cout << "Can not join thread" << std::endl;
            }
        }

        for (auto const& array : data) {
            for (auto item : array) {
                if (!set->contains(item)) {
                    success = false;
                    std::cout << "Error: Set does not contain key" << std::endl;
                }
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    for (auto& arg : args) {
        delete arg;
    }

    return { success, std::chrono::duration<double, std::milli>(end_time - start_time).count() };
}

SetTester::TestResult_t SetTester::reading_test(data_t const& data) {
    std::vector<pthread_t> threads(data.size());

    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<Arg_t*> args = setup_threads(threads, data, read);

    bool success = true;
    
    {
        for (auto& thread : threads) {
            if (pthread_join(thread, nullptr) != 0) {
                std::cout << "Can not join thread" << std::endl;
            }
        }

        success = success && set->is_empty();
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    for (auto& arg : args) {
        delete arg;
    }

    if (!success) {
        std::cout << "Reading test failed" << std::endl;
        delete set;
    }

    return { success, std::chrono::duration<double, std::milli>(end_time - start_time).count() };
}

SetTester::TestResult_t SetTester::mix_test(data_t const& data) {
    size_t size = data.size();
    bool success = true;

    double total_time = 0.0;

    for (size_t writers_num = 1, readers_num = size - 1; readers_num > 0; ++writers_num, --readers_num) {
        std::vector<pthread_t> writers(writers_num);
        std::vector<pthread_t> readers(readers_num);

        data_t writers_data = reshape_data(data, writers_num);
        data_t readers_data = reshape_data(data, readers_num);

        std::vector<int> check_vec(data.size() * data[0].size());

        auto start_time = std::chrono::high_resolution_clock::now();

        std::vector<Arg_t*> writers_args = setup_threads(writers, writers_data, write);
        std::vector<Arg_t*> readers_args = setup_threads(readers, readers_data, read, &check_vec);

        for (auto& thread : writers) {
            if (pthread_join(thread, nullptr) != 0) {
                std::cout << "Writer thread can not join" << std::endl;
            }
        }
        for (auto& thread : readers) {
            if (pthread_join(thread, nullptr) != 0) {
                std::cout << "Reader thread can not join" << std::endl;
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();

        for (auto& arg : writers_args) {
            delete arg;
        }
        for (auto& arg : readers_args) {
            delete arg;
        }

        success = success && check_vector(check_vec);

        total_time += std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }

    return { success, total_time / (data.size() - 1) };
}

SetTester::data_t SetTester::generate_fix_data(size_t arrays_num, size_t items_num) const {
    data_t data(arrays_num);
    int num = 0;

    for (size_t i = 0; i < arrays_num; ++i) {
        data[i] = std::vector<int>(items_num);

        for (size_t j = 0; j < items_num; ++j, ++num) {
            data[i][j] = num;
        }
    }

    return data;
}

SetTester::data_t SetTester::generate_random_data(size_t arrays_num, size_t items_num) const {
    size_t size = arrays_num * items_num;
    std::vector<size_t> numbers(size);

    for (size_t i = 0; i < size; ++i) {
        numbers[i] = i;
    }

    std::random_device rd;
    std::mt19937 mt(rd());

    data_t data(arrays_num);

    for (size_t i = 0; i < arrays_num; ++i) {
        data[i] = std::vector<int>(items_num);

        for (size_t j = 0; j < items_num; ++j) {
            std::uniform_int_distribution<int> uni(0, numbers.size() - 1);
            size_t ind = uni(mt);

            data[i][j] = numbers[ind];

            numbers.erase(numbers.begin() + ind);
        }
    }

    return data;
}


SetTester::data_t SetTester::reshape_data(data_t const& src_data, size_t new_size) const {
    size_t items_per_line = src_data[0].size();
    size_t total_items = src_data.size() * items_per_line;
    data_t data(new_size);

    for (size_t cur_size = 0; cur_size < total_items;) {
        for (size_t i = 0; i < new_size && cur_size < total_items; ++i, ++cur_size) {
            size_t ind_i = cur_size / items_per_line;
            size_t ind_j = cur_size - items_per_line * ind_i;
 
            data[i].push_back(src_data[ind_i][ind_j]);
        }
    }

    return data;
}

bool SetTester::check_vector(std::vector<int> const& check_vec) const {
    for (auto item : check_vec) {
        if (item != 1) {
            return false;
        }
    }

    return true;
}

void SetTester::update_results(TestResult_t& tr, TestResult_t const& new_tr) const {
    tr.is_passed = tr.is_passed && new_tr.is_passed;
    tr.time += new_tr.time;
}

std::vector<SetTester::Arg_t*> SetTester::setup_threads(std::vector<pthread_t>& threads, 
                                                        data_t const& data, 
                                                        void* func(void*),
                                                        std::vector<int>* check_vector) const {
    std::vector<Arg_t*> args(threads.size());
    
    for (size_t i = 0; i < args.size(); ++i) {
        args[i] = new Arg_t{ set, data[i], check_vector };
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        if (pthread_create(&threads[i], nullptr, func, args[i]) != 0) {
            std::cout << "Can not create thread" << std::endl;
        }
    }

    return args;
}
