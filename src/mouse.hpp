#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <map>

#include <GLFW/glfw3.h>

namespace Mouse {
enum class Action {
    LeftPress,
    LeftRelease,
    RightPress,
    RightRelease,
    ScrollUp,
    ScrollDown,
    Movement
};
enum class State { Default, LeftPressed, RightPressed };
class StateMachine {
  public:
    void add(
        State start,
        Action action,
        State end,
        std::function<void()> callback = [] {}) {
        config[start][action] = {end, callback};
    }

    template <typename... Args>
    void add(State start, Action action, State end, std::function<void(Args...)> callback) {
        config[start][action] = {end, callback};
    }


    template <typename... Args>
    void execute(Action action, Args... args) {
        if (auto found = config.find(state); found != std::cend(config)) {
            auto actions = std::get<1>(*found);
            if (auto found = actions.find(action); found != std::end(actions)) {
                auto [end, fn] = std::get<1>(*found);
                state          = end;
                std::any_cast<std::function<void(Args...)>>(fn)(args...);
            }
        }
    }

  private:
    std::map<State, std::map<Action, std::pair<State, std::any>>> config{};
    State state{State::Default};
};

} // namespace Mouse