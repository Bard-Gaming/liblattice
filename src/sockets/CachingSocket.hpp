/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Header file for
** ChachingSocket class
*/

#pragma once

#include "NonBlockingSocket/NonBlockingSocket.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <stdexcept>


namespace Lattice {
    template <char D[]>
    class CachingSocket : public NonBlockingSocket {
        static constexpr

        std::array<char, S> m_Cache;
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

            static inline constexpr std::size_t cacheSize() { return S; }
            inline void clearCache() { m_FilledBytes = 0; }

            template <Serializable T>
            std::optional<T> read() requires (sizeof(T) <= S)
            {
                if (m_FilledBytes >= sizeof(T))
                    return dataFromCache<T>();

                std::span<char> toFill = freeCache();
                ssize_t bytesRead = NonBlockingSocket::read(toFill, toFill.size());
                if (bytesRead <= 0)
                    return {};

                m_FilledBytes += bytesRead;

                if (m_FilledBytes >= sizeof(T))
                    return dataFromCache<T>();

                return {};
            }

            inline void operator=(NonBlockingSocket&& other) { swap(other); }
            inline void operator=(CachingSocket&& other) { swap(other); }
            constexpr void swap(CachingSocket& other)
            {
                std::swap(m_Cache, other.m_Cache);
                std::swap(m_FilledBytes, m_FilledBytes);
                NonBlockingSocket::swap(other);
            }

            constexpr void swap(NonBlockingSocket& other)
            {
                m_FilledBytes = 0;
                NonBlockingSocket::swap(other);
            }

            constexpr void swap(Socket& other)
            {
                m_FilledBytes = 0;
                NonBlockingSocket::swap(other);
            }

        private:
            bool isCacheFull() const { return m_FilledBytes == S; }

            std::span<char> freeCache()   { return std::span<char>(m_Cache.begin() + m_FilledBytes, m_Cache.end()); }
            std::span<char> filledCache() { return std::span<char>(m_Cache.begin(), m_Cache.begin() + m_FilledBytes); }

            /**
             * Note:
             * Assumes m_FilledBytes >= sizeof(T)
             */
            template <Serializable T>
            constexpr T dataFromCache() requires (sizeof(T) <= S) {
                if (m_FilledBytes < sizeof(T))
                    throw std::runtime_error("building data type from cache isn't possible");

                T data = cacheCast<T>(filledCache());

                for (std::size_t i = m_FilledBytes; i < S; i++)
                    m_Cache[i - m_FilledBytes] = m_Cache[i];

                m_FilledBytes -= sizeof(T);

                return data;
            }

            template <Serializable T>
            static constexpr T cacheCast(std::span<char> cache) requires (sizeof(T) <= S)
            {
                // the only way to cast the bits of our cache
                // into the desired type is to use std::bit_cast
                // (due to strict aliasing), which requires both
                // types (to and from) to be of the same size.
                struct { char data[sizeof(T)]; } repr;
                std::copy(cache.begin(), cache.end(), repr.data);

                return std::bit_cast<T>(repr);
            }
    };
}
