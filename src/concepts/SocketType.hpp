/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** SocketType concept
*/

#pragma once

#include <span>


namespace Lattice {
    template <typename T>
    concept SocketType = requires (T& socket, std::span<char> buff)
    {
        { socket.open()  };
        { socket.close() };
        { socket.fileno() } -> std::convertible_to<int>;
        { socket.read(buff, 5) } -> std::convertible_to<int>;
        { socket.write(buff)   } -> std::convertible_to<int>;
    };
}
