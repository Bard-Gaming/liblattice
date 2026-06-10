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
    inline constexpr bool is_serializable_v = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

    template <typename T>
    inline constexpr bool is_serializable_v<T[]> = is_serializable_v<T>;

    template <typename T>
    concept Serializable = is_serializable_v<T>;
}
