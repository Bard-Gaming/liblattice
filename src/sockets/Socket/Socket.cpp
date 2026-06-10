/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Implementation for
** Socket class
*/

#include "Socket.hpp"
#include <lattice/exceptions.hpp>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


Lattice::Socket::Socket() noexcept
    : m_FD(-1)
    , m_Address{ 0 }
{

}

Lattice::Socket::Socket(Socket&& old) noexcept
    : Socket()
{
    swap(old);
}

Lattice::Socket::~Socket()
{
    close();
}

std::string_view Lattice::Socket::ip() const
{
    char* repr = ::inet_ntoa(m_Address.sin_addr);

    return repr ? repr : "<no assigned ip>";
}

std::uint16_t Lattice::Socket::port() const
{
    return ::ntohs(m_Address.sin_port);
}

void Lattice::Socket::open()
{
    m_FD = ::socket(AF_INET, SOCK_STREAM, 0);

    if (m_FD >= 0)
        return;

    throw Exceptions::SocketException("failed to open TCP socket");
}

void Lattice::Socket::close() noexcept
{
    if (m_FD < 0)
        return;

    ::close(m_FD);
    m_FD = -1;
}

void Lattice::Socket::bind(std::string_view ip, std::uint16_t port)
{
    setAddress(ip, port);

    int success = ::bind(
        m_FD,
        reinterpret_cast<sockaddr*>(&m_Address),
        sizeof(sockaddr_in)
    );

    if (success >= 0)
        return;

    m_Address = { 0 };

    std::string msg = "failed to bind to ";

    msg.append(ip)
       .append(":")
       .append(std::to_string(port));

    throw Exceptions::SocketException(msg);
}

void Lattice::Socket::listen()
{
    int success = ::listen(m_FD, CONNECTION_BACKLOG);

    if (success >= 0)
        return;

    throw Exceptions::SocketException("failed to make socket listen");
}

std::optional<Lattice::Socket> Lattice::Socket::accept() const
{
    sockaddr_in peerAddr;
    socklen_t peerAddrLen = sizeof(sockaddr_in);

    int peerFD = ::accept(
        m_FD,
        reinterpret_cast<sockaddr*>(&peerAddr),
        &peerAddrLen
    );

    if (peerFD < 0)
        return {};

    Socket peer;
    peer.m_FD = peerFD;
    peer.m_Address = peerAddr;
    return peer;
}

void Lattice::Socket::connect(std::string_view ip, std::uint16_t port)
{
    setAddress(ip, port);

    int success = ::connect(
        m_FD,
        reinterpret_cast<sockaddr*>(&m_Address),
        sizeof(sockaddr_in)
    );

    if (success >= 0)
        return;

    m_Address = { 0 };

    std::string msg = "failed to connect to ";

    msg.append(ip)
       .append(":")
       .append(std::to_string(port));

    throw Exceptions::SocketException(msg);
}

/**
 * Attempts to read data from the socket
 * and into the specified buffer. The amount
 * of bytes this will attempt to read depends
 * on the size of the given buffer.
 *
 * The actual number of written bytes can be
 * retrieved using the returned value.
 *
 * Note:
 * Regardless of the outcome of this function,
 * the specified buffer will be changed.
 */
ssize_t Lattice::Socket::read(std::span<char> buffer, std::size_t amount)
{
    std::size_t possibleAmnt = std::min(buffer.size(), amount);

    std::ranges::fill(buffer, '\0');

    ssize_t readBytes = ::read(m_FD, buffer.data(), possibleAmnt);

    if (possibleAmnt != 0 && readBytes == 0)
        close();  // read of 0 means socket has disconnected

    return readBytes;
}

/**
 * Writes the specified data
 * to the socket.
 *
 * Returns the actual number of bytes
 * written.
 */
ssize_t Lattice::Socket::write(std::span<const char> data)
{
    return ::write(m_FD, data.data(), data.size());
}

Lattice::Socket::operator std::string() const
{
    if (!isOpen())
        return "<unopened socket>";

    if (!isBound())
        return "<unbound socket>";

    std::string repr;

    repr.append(ip())
        .append(":")
        .append(std::to_string(port()));

    return repr;
}

void Lattice::Socket::setAddress(std::string_view ip, std::uint16_t port)
{
    bool success = ::inet_aton(ip.data(), &m_Address.sin_addr);

    if (!success) {
        m_Address.sin_addr = { 0 };
        throw Exceptions::SocketException(std::string(ip) + " is not a valid ip");
    }

    m_Address.sin_port = ::htons(port);
    m_Address.sin_family = AF_INET;
}
