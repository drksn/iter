#include "basicio/file_io.hpp"
#include <iter/file_keeper.hpp>
#include <iter/file_monitor.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    std::string filename = "file_keeper.test";
    std::string text = "File keeper test.";
    bool write_ret = iter::FileWrite(filename, text);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto file_monitor_ptr =
            std::make_shared <iter::FileMonitor> ();
    auto file_keeper_ptr =
            std::make_shared <iter::FileKeeper <std::string>> (filename, iter::FileRead, file_monitor_ptr);

    if (file_keeper_ptr->Load()) {
        std::cout << "Initial load success." << std::endl;
    }
    else {
        std::cout << "Initial load failed." << std::endl;
    }

    std::shared_ptr <const std::string> ptr;
    ptr = file_keeper_ptr->Get();
    std::cout << "Get result = " << *ptr << std::endl;
    ptr.reset();

    std::string new_text = "File keeper modified.";
    bool new_write_ret = iter::FileWrite(filename, new_text);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ptr = file_keeper_ptr->Get();
    std::cout << "Get result = " << *ptr << std::endl;

    return 0;
}

