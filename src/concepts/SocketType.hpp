/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** SocketType concept
*/

#pragma once

#include <span>
#include <concepts>


namespace Lattice {
    class Socket;

    template <typename T>
    concept SocketType = requires (T& socket, std::span<char> buff, std::size_t amnt)
    {
        { socket.open()  };
        { socket.close() };
        { socket.fileno() } -> std::convertible_to<int>;
        { socket.read(buff, amnt) } -> std::convertible_to<long long int>;
        { socket.write(buff)      } -> std::convertible_to<long long int>;
    };

    template <typename T>
    concept ClientSocketType = requires (T clientSocket, Socket&& socket, short revents)
    {
        { T(std::move(socket)) };
        { clientSocket.requiredEvents() } -> std::convertible_to<short>;
        { clientSocket.registerEvents(revents) };

    };
}
