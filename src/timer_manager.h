//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <set>
#include <cstdint>
#include <chrono>
#include <functional>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

class timer_manager
{
public:
    timer_manager() = default;
    ~timer_manager() = default;

    void start();
    void stop();
    void add_once_timer(std::chrono::milliseconds delay, std::function<void(time_point<system_clock> current_time)> callback);
    void add_recurring_timer(std::chrono::milliseconds interval, std::function<void(time_point<system_clock> current_time)> callback);

    struct timer_once
    {
        time_point<system_clock> run_time{};
        std::function<void(time_point<system_clock> current_time)> callback{};
    };

    struct timer_recurring
    {
        time_point<system_clock> run_time{};
        std::function<void(time_point<system_clock> current_time)> callback{};
        std::chrono::milliseconds interval{};
    };

    std::set<std::shared_ptr<timer_once>> once_timer_callbacks{};
    std::set<std::shared_ptr<timer_recurring>> recurring_timer_callbacks{};
    bool running = false;
    std::unique_ptr<std::thread> on_timer_thread{};
    bool thread_ended = false;
    time_point<system_clock> last_run_time{};
};
