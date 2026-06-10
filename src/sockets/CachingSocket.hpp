/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Header file for
** ChachingSocket class
*/

#pragma once

#include "NonBlockingSocket/NonBlockingSocket.hpp"
#include "../utils.hpp"
#include <algorithm>
#include <array>
#include <span>


namespace Lattice {
    template <Serializable T, std::size_t S>
    class CachingSocket : public NonBlockingSocket {
        using Type = T;
        static constexpr auto CACHE_ELEMENTS = S;
        static constexpr auto CACHE_SIZE = CACHE_ELEMENTS * sizeof(Type);

        std::array<char, CACHE_SIZE> m_Cache;
        std::size_t m_FilledBytes;

        public:
            CachingSocket()
                : NonBlockingSocket()
                , m_Cache()
                , m_FilledBytes(0)
            {}

            CachingSocket(CachingSocket&& other) : CachingSocket() { swap(other); }
            CachingSocket(NonBlockingSocket&& other) : CachingSocket() { swap(other); }
            CachingSocket(Socket&& other) : CachingSocket() { swap(other); }

            inline void clearCache() { m_FilledBytes = 0; }

            std::optional<Type> read()
            {
                if (m_FilledBytes >= sizeof(Type))
                    return popFromCache();

                std::span<char> toFill = freeCache();
                ssize_t bytesRead = NonBlockingSocket::read(toFill, toFill.size());
                if (bytesRead <= 0)
                    return {};

                m_FilledBytes += bytesRead;

                if (m_FilledBytes >= sizeof(Type))
                    return popFromCache();

                return {};
            }

            inline void operator=(Socket&& other) { swap(other); }
            inline void operator=(NonBlockingSocket&& other) { swap(other); }
            inline void operator=(CachingSocket&& other) { swap(other); }
            constexpr void swap(CachingSocket& other)
            {
                std::swap(m_Cache, other.m_Cache);
                std::swap(m_FilledBytes, m_FilledBytes);
                NonBlockingSocket::swap(other);
            }

            /**
             * Note:
             * This clears the cache.
             */
            constexpr void swap(NonBlockingSocket& other)
            {
                m_FilledBytes = 0;
                NonBlockingSocket::swap(other);
            }

            /**
             * Note:
             * This clears the cache.
             */
            constexpr void swap(Socket& other)
            {
                m_FilledBytes = 0;
                Socket::swap(other);
            }

        private:
            bool isCacheFull() const { return m_FilledBytes >= CACHE_SIZE; }

            std::span<char> freeCache()   { return std::span<char>(m_Cache.begin() + m_FilledBytes, m_Cache.end()); }
            std::span<char> filledCache() { return std::span<char>(m_Cache.begin(), m_Cache.begin() + m_FilledBytes); }

            /**
             * Note:
             * Assumes m_FilledBytes >= sizeof(T)
             */
            constexpr Type popFromCache() noexcept {
                Type data = cacheCast<Type>(filledCache());

                for (std::size_t i = m_FilledBytes; i < CACHE_SIZE; i++)
                    m_Cache[i - m_FilledBytes] = m_Cache[i];

                m_FilledBytes -= sizeof(Type);
                return data;
            }

            /**
             * Note:
             * Assumes cache.size() >= sizeof(To)
             */
            template <Serializable To>
            static To cacheCast(std::span<char> cache) requires (sizeof(To) <= CACHE_SIZE)
            {
                return *reinterpret_cast<To*>(cache.data());
            }
    };
}
