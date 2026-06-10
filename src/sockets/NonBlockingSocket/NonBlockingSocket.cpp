/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Implementation for
** NonBlockingSocket class
*/

#include "NonBlockingSocket.hpp"
#include <poll.h>


Lattice::NonBlockingSocket::NonBlockingSocket()
    : Socket()
    , m_PollResult(0)
{

}

Lattice::NonBlockingSocket::NonBlockingSocket(Socket&& socket)
    : NonBlockingSocket()
{
    swap(socket);
}

Lattice::NonBlockingSocket::NonBlockingSocket(NonBlockingSocket&& old)
    : NonBlockingSocket()
{
    swap(old);
}

void Lattice::NonBlockingSocket::close() noexcept
{
    Socket::close();
    m_PollResult = 0;
}

/**
 * Attempts to read amount bytes
 * from the socket. If the socket
 * would block, the function fails
 * and a read operations isn't started.
 *
 * Note:
 * This always clears the specified buffer.
 */
ssize_t Lattice::NonBlockingSocket::read(std::span<char> buffer, std::size_t amount)
{
    if (!canRead())
        return -1;

    disableRead();
    ssize_t readBytes = Socket::read(buffer, amount);

    if (!isOpen())
        close();

    return readBytes;
}

/**
 * Attempts to write the specified data
 * to the socket. If the socket would block,
 * a negative value is returned and the
 * write isn't attempted.
 */
ssize_t Lattice::NonBlockingSocket::write(std::span<const char> data)
{
    if (!canWrite())
        return -1;

    disableWrite();
    return Socket::write(data);
}

std::optional<Lattice::Socket> Lattice::NonBlockingSocket::accept()
{
    if (!canRead())
        return {};

    disableRead();
    return Socket::accept();
}

bool Lattice::NonBlockingSocket::canWrite() const noexcept
{
    return m_PollResult & POLLOUT;
}

bool Lattice::NonBlockingSocket::canRead() const noexcept
{
    return m_PollResult & POLLIN;
}

short Lattice::NonBlockingSocket::requiredEvents() const noexcept
{
    return canWrite()
        ? POLLIN  // don't poll for POLLOUT if not required (spams poll)
        : POLLIN | POLLOUT;
}

void Lattice::NonBlockingSocket::registerEvents(short revents) noexcept
{
    // FD is no longer valid, so close
    if (revents & POLLNVAL) {
        close();
        return;
    }

    m_PollResult = requiredEvents() == POLLIN
        ? m_PollResult | (revents & POLLIN)
        : revents;
}

void Lattice::NonBlockingSocket::disableRead()
{
   m_PollResult &= ~POLLIN;
}

void Lattice::NonBlockingSocket::disableWrite()
{
   m_PollResult &= ~POLLOUT;
}
