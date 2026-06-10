/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** common utilities
*/

#pragma once

#include <type_traits>


namespace Lattice {
    template <typename T>
    concept Serializable =
        std::is_standard_layout_v<T> &&
        std::is_trivial_v<T>;
}
