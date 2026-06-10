/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** Socket class
*/

#pragma once

#include <string_view>
#include <optional>
#include <cstdint>
#include <utility>
#include <span>
#include <netinet/in.h>


namespace Lattice {
    /**
     * Zappy Socket class.
     *
     * Note:
     * This class is *only* for TCP
     * sockets, since they are the only
     * sockets required by the project.
     */
    class Socket {
        static constexpr int CONNECTION_BACKLOG = 100;

        int m_FD;
        sockaddr_in m_Address;

        public:
            Socket() noexcept;
            Socket(Socket&& old) noexcept;
            ~Socket();

            inline bool isOpen() const noexcept { return m_FD >= 0; }
            inline bool isBound() const noexcept { return m_Address.sin_family == AF_INET; }
            inline int fileno() const noexcept { return m_FD; }

            std::string_view ip() const;
            std::uint16_t port() const;

            void open();
            void close() noexcept;

            void bind(std::string_view ip, std::uint16_t port);
            void listen();
            std::optional<Socket> accept() const;
            void connect(std::string_view ip, std::uint16_t port);

            ssize_t read(std::span<char> buffer, std::size_t amount);
            ssize_t write(std::span<const char> data);

            inline void operator=(Socket&& other) { swap(other); }
            constexpr void swap(Socket& other)
            {
                std::swap(m_FD, other.m_FD);
                std::swap(m_Address, other.m_Address);
            }

            operator std::string() const;

        private:
            void setAddress(std::string_view ip, std::uint16_t port);
    };
}


template <>
constexpr void std::swap(Lattice::Socket& a, Lattice::Socket& b)
{
    a.swap(b);
}
