/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** InterruptHandler class
*/

#pragma once

#include <lattice/exceptions.hpp>
#include <functional>
#include <csignal>


namespace Lattice::Utils {
    /**
     * SignalHandler class.
     *
     * This implementation of
     * a signal handler allows
     * multiple instanciations,
     * where every new instanciation
     * replaces the current handler until
     * the destructor is called (in which
     * case the original handler is restored).
     */
    template <int SIGNAL>
    class SignalHandler {
        using Handler = void(*)(int);
        using Callback = std::function<void(void)>;

        static inline SignalHandler* s_TopLevelHandler = nullptr;

        SignalHandler* m_Parent;
        SignalHandler* m_Child;

        Handler m_PreviousHandler;
        Callback m_Callback;

        public:
            SignalHandler()
                : m_Parent(nullptr)
                , m_Child(nullptr)
                , m_PreviousHandler(nullptr)
                , m_Callback()
            {

            }

            SignalHandler(Callback callback)
                : m_Parent(nullptr)
                , m_Child(nullptr)
                , m_PreviousHandler(nullptr)
                , m_Callback(callback)
            {
                m_Parent = s_TopLevelHandler;
                if (m_Parent != nullptr)
                    m_Parent->m_Child = this;

                s_TopLevelHandler = this;

                m_PreviousHandler = ::signal(SIGNAL, handleSignal);
            }

            SignalHandler(const SignalHandler&) = delete;
            SignalHandler(SignalHandler&& other)
                : SignalHandler()
            {
                swap(other);
            }

            ~SignalHandler()
            {
                if (!m_Callback)
                    return;  // nothing to be done

                if (m_Child != nullptr) {
                    m_Child->m_Parent = m_Parent;
                    return;
                }

                ::signal(SIGNAL, m_PreviousHandler);
                s_TopLevelHandler = m_Parent;

                if (m_Parent != nullptr)
                    m_Parent->m_Child = nullptr;
            }

            /**
             * Note:
             * Since setting a callback function
             * in a cleared signal handler would
             * change the execution order of signal
             * handlers and make these signal handlers
             * more complex than they need to be,
             * the operation is unsupported and will
             * throw a SignalHandlerException.
             */
            void setCallback(Callback callback)
            {
                if (!m_Callback)
                    throw Exceptions::SignalHandlerException("setting the callback function of a cleared signal handler is unsupported");

                m_Callback = callback;
            }

            void clear()
            {
                SignalHandler toBeRemoved;
                swap(toBeRemoved);
            }

            inline void operator=(SignalHandler&& other) { swap(other); }
            constexpr void swap(SignalHandler& other) noexcept
            {
                if (s_TopLevelHandler == &other)
                    s_TopLevelHandler = this;
                else if (s_TopLevelHandler == this)
                    s_TopLevelHandler = &other;

                std::swap(m_Parent, other.m_Parent);
                std::swap(m_Child, other.m_Child);
                std::swap(m_PreviousHandler, other.m_PreviousHandler);
                std::swap(m_Callback, other.m_Callback);
            }

        private:
            static void handleSignal(int)
            {
                s_TopLevelHandler->m_Callback();
            }
    };

    using InterruptHandler = SignalHandler<SIGINT>;
}
