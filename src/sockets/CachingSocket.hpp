/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Header file for
** ChachingSocket class
*/

#pragma once

#include "NonBlockingSocket/NonBlockingSocket.hpp"
#include <string_view>
#include <algorithm>
#include <string>
#include <array>


namespace Lattice {
    template <std::size_t S = 1024>
    class CachingSocket : public NonBlockingSocket {
        static constexpr std::size_t MAX_CACHE_SIZE = S;

        std::array<char, MAX_CACHE_SIZE> m_Cache;
        std::size_t m_FilledBytes;

        public:
            CachingSocket()
                : NonBlockingSocket()
                , m_Cache()
                , m_FilledBytes(0)
            {

            }

            CachingSocket(CachingSocket&& other) { swap(other); }
            CachingSocket(NonBlockingSocket&& other) { swap(other); }
            CachingSocket(Socket&& other) { swap(other); }

            bool readUntil(std::string& output, char delimiter)
            {
                if (popFromCache(output, delimiter))
                    return true;

                std::size_t toFill = MAX_CACHE_SIZE - m_FilledBytes;
                auto start = m_Cache.begin();
                std::advance(start, m_FilledBytes);
                std::span<char> bufferToFill(start, m_Cache.end());

                auto bytesRead = NonBlockingSocket::read(bufferToFill, toFill);
                if (bytesRead <= 0)
                    return false;

                m_FilledBytes += bytesRead;
                return popFromCache(output, delimiter);
            }

            inline void operator=(CachingSocket&& other)     { swap(std::move(other)); }
            inline void operator=(NonBlockingSocket&& other) { swap(std::move(other)); }
            inline void operator=(Socket&& other)            { swap(std::move(other)); }
            void swap(CachingSocket& other)
            {
                std::swap(m_Cache, other.m_Cache);
                NonBlockingSocket::swap(other);
            }

            void swap(NonBlockingSocket& other)
            {
                m_Cache.fill('\0');
                NonBlockingSocket::swap(other);
            }

            void swap(Socket& other)
            {
                m_Cache.fill('\0');
                NonBlockingSocket::swap(other);
            }

        private:
            /**
             * Pops (i.e. retrieves and removes) a string
             * from the cache. The string starts at the beginning
             * of the cache and goes up to the specified delimiter.
             *
             * Note:
             * The given string will inevitably get cleared when
             * the function is called.
             *
             * Furthermore, the returned string being empty does
             * not indicate that an error occurred, since there
             * may just be two consecutive delimiters.
             */
            bool popFromCache(std::string& output, char delimiter)
            {
                output.clear();

                auto start = m_Cache.begin();
                auto end = start;
                std::advance(end, m_FilledBytes);

                auto valueEnd = std::find(start, end, delimiter);
                if (valueEnd == end)
                    return false;

                ++valueEnd;  // delimiter is part of output
                std::string_view value(start, valueEnd);
                output.append(value);
                clearFromCache(value.length());

                return true;
            }

            /**
             * Removes nChars from the
             * cache.
             *
             * Note:
             * Assumes nChars <= m_FilledByes.
             */
            void clearFromCache(std::size_t nChars)
            {
                std::size_t newSize = m_FilledBytes - nChars;

                for (std::size_t i = 0; i < nChars; i++) {
                    m_Cache[i] = m_Cache[i + nChars];
                }

                m_FilledBytes = newSize;
            }
    };
}
