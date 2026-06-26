/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** NonBlockingSocket class
*/

#pragma once

#include "../Socket/Socket.hpp"


namespace Lattice {
    class NonBlockingSocket : public Socket {
        short m_PollResult;

        public:
            NonBlockingSocket();
            NonBlockingSocket(Socket&& socket);
            NonBlockingSocket(NonBlockingSocket&& old);

            void close() noexcept;

            ssize_t read(std::span<char> buffer, std::size_t amount);
            ssize_t write(std::span<const char> data);

            std::optional<Socket> accept();

            bool canWrite() const noexcept;
            bool canRead() const noexcept;
            inline void refresh() noexcept { disableRead(); disableWrite(); }

            short requiredEvents() const noexcept;
            void registerEvents(short revents) noexcept;

            inline void operator=(NonBlockingSocket&& other) { swap(other); }
            inline void operator=(Socket&& other) { swap(other); }
            constexpr void swap(NonBlockingSocket& other)
            {
                Socket::swap(other);
                std::swap(m_PollResult, other.m_PollResult);
            }

            constexpr void swap(Socket& other)
            {
                m_PollResult = 0;
                Socket::swap(other);
            }

        private:
            void disableRead();
            void disableWrite();
    };
}

template <>
constexpr void std::swap(Lattice::NonBlockingSocket& a, Lattice::NonBlockingSocket& b)
{
    a.swap(b);
}
