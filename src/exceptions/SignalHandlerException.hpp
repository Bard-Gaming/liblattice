/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** SignalHandlerException class
*/

#pragma once

#include "BaseException.hpp"

namespace Lattice::Exceptions {
    class SignalHandlerException : public BaseException {
        public:
            using BaseException::BaseException;
    };
}
