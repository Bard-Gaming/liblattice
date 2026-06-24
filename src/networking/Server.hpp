/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Implementation for
** Server class
*/

#pragma once

#include <lattice/concepts.hpp>
#include <lattice/sockets.hpp>
#include <lattice/utils.hpp>
#include <vector>
#include <atomic>
#include <list>
#include <poll.h>


namespace Lattice {
    template <ClientSocketType C, SocketType S = NonBlockingSocket, std::size_t CB = 50>
    class Server {
        static constexpr auto CONNECTION_BACKLOG = CB;
        using ClientSocket = C;
        using ServerSocket = S;

        ServerSocket m_Host;
        std::list<ClientSocket> m_Clients;
        std::vector<pollfd> m_PollFds;

        Utils::InterruptHandler m_InterruptHandler;
        std::atomic<bool> m_IsRunning;

        public:
            Server()
                : m_Host()
                , m_Clients()
                , m_PollFds()
                , m_InterruptHandler([&](){ m_IsRunning.store(false); })
                , m_IsRunning(false)
            {

            }

            Server(Server&& other)
                : Server()
            {
                swap(other);
            }

            Server(std::string_view ip, std::uint16_t port)
                : Server()
            {
                m_Host.open();
                m_Host.bind(ip, port);
                m_Host.listen();

                m_PollFds.emplace_back(m_Host.fileno(), POLLIN, 0);
            }

            inline std::string_view hostname() const { return m_Host.ip(); }
            inline std::uint16_t port() const { return m_Host.port(); }

            void run()
            {

                m_IsRunning.store(true);
                onStart();

                while (m_IsRunning.load()) {
                    pollSockets();
                    purgeDisconnectedClients();

                    updateServer();

                    for (auto& client : m_Clients)
                        updateClient(client);

                    acceptClient();
                }

                onShutdown();
            }

            inline void operator=(Server&& other) { swap(other); }
            void swap(Server& other)
            {
                std::swap(m_Host, other.m_Host);
                std::swap(m_Clients, other.m_Clients);
                std::swap(m_PollFds, other.m_PollFds);
                m_InterruptHandler.swap(other.m_InterruptHandler);

                bool intermediate = m_IsRunning.load();
                m_IsRunning.store(other.m_IsRunning.load());
                other.m_IsRunning.store(intermediate);

                // Since the callbacks contain reference
                // and they are swapped in the interrupt handler's
                // swap() function, we need to set them back to what
                // they were originally (otherwise shutting one server
                // down will shut the other one down instead).
                m_InterruptHandler.setCallback([&](){ m_IsRunning.store(false); });
                other.m_InterruptHandler.setCallback([&](){ other.m_IsRunning.store(false); });
            }

        private:
            virtual inline void onStart() {}
            virtual inline void onShutdown() {}

            virtual inline void updateServer() {}
            virtual inline void updateClient(ClientSocket&) {}
            virtual inline void onClientAccepted(const ClientSocket&) {}
            virtual inline void onClientDisconnected(const ClientSocket&) {}

            void pollSockets()
            {
                std::size_t pollIndex;

                pollIndex = 1;
                for (auto& client : m_Clients) {
                    m_PollFds[pollIndex].events = client.requiredEvents();
                    pollIndex++;
                }

                int success = ::poll(m_PollFds.data(), m_PollFds.size(), -1);
                if (success < 0)
                    return;

                m_Host.registerEvents(m_PollFds[0].revents);

                pollIndex = 1;
                for (auto& client : m_Clients) {
                    client.registerEvents(m_PollFds[pollIndex].revents);
                    pollIndex++;
                }
            }

            void purgeDisconnectedClients()
            {
                auto pollfd = ++m_PollFds.cbegin();
                auto it = m_Clients.cbegin();

                while (it != m_Clients.cend()) {
                    if (it->isOpen()) {
                        ++pollfd;
                        ++it;
                        continue;
                    }

                    // disconnected client, so remove
                    onClientDisconnected(*it);
                    it = m_Clients.erase(it);
                    pollfd = m_PollFds.erase(pollfd);
                }
            }

            void acceptClient()
            {
                auto newPeer = m_Host.accept();
                if (!newPeer)
                    return;

                ClientSocket peer = std::move(*newPeer);

                m_PollFds.emplace_back(peer.fileno(), peer.requiredEvents(), 0);
                m_Clients.push_back(std::move(peer));

                onClientAccepted(m_Clients.back());
            }
    };
}
