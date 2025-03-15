//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#include "timer_manager.h"
#include "time_utils.h"
#include <iostream>
#include <spdlog/spdlog.h>

void timer_manager::start()
{
    running = true;
    last_run_time = now();
    on_timer_thread = std::make_unique<std::thread>([&]()
        {
            int32_t error_count = 0;
            while (running)
            {
                try
                {
                    for (auto & cb : once_timer_callbacks)
                    {
                        auto n = now();
                        if (n >= cb->run_time)
                        {
                            cb->callback(n);
                            once_timer_callbacks.erase(cb);
                        }
                    }
                    for (auto & cb : recurring_timer_callbacks)
                    {
                        auto n = now();
                        if (n >= cb->run_time)
                        {
                            cb->run_time += cb->interval;
                            cb->callback(n);
                        }
                    }
                    last_run_time = now();
                    std::this_thread::sleep_for(1ms);
                }
                catch (std::exception & ex)
                {
                    spdlog::get("helbreath")->error("on_timer error: {}", ex.what());
                    error_count++;

                    if (error_count > 100)
                    {
                        spdlog::get("helbreath")->error("on_timer error count > 100 - exiting thread");
                        return;
                    }
                }
            }
        });
    spdlog::get("helbreath")->info("Timer manager started");
}

void timer_manager::stop()
{
    running = false;
    if (on_timer_thread)
    {
        if (!on_timer_thread->joinable())
        {
            spdlog::get("helbreath")->error("on_timer_thread not joinable");
        }
        else
        {
            stop();
            on_timer_thread->join();
        }
    }
}

void timer_manager::add_once_timer(std::chrono::milliseconds delay, std::function<void(time_point<system_clock> current_time)> callback)
{
    auto t = std::make_shared<timer_once>();
    t->run_time = now() + delay;
    t->callback = callback;
    once_timer_callbacks.insert(t);
}

void timer_manager::add_recurring_timer(std::chrono::milliseconds interval, std::function<void(time_point<system_clock> current_time)> callback)
{
    auto t = std::make_shared<timer_recurring>();
    t->run_time = now() + interval;
    t->interval = interval;
    t->callback = callback;
    recurring_timer_callbacks.insert(t);
}
