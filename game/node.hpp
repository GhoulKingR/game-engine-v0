#pragma once

// #include <cstring>

// namespace script {
// void *_ctx;
// inline void (*set_transform)(void *);
// inline void (*get_transform)(void *);
// } // namespace script
//
// extern "C" inline void __register_func_node(const char *name,
//                                             void (*func)(void *)) {
//     if (strcmp(name, "set_transform") == 0) {
//         script::set_transform = func;
//     } else if (strcmp(name, "get_transform") == 0) {
//         script::get_transform = func;
//     }
// }

#include <print>

extern "C" inline void init() {
    std::println("I'm an init script");
}
