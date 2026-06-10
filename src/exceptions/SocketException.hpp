/*
** EPITECH PROJECT, 2026
** Project - Lattice
** File description:
** Header file for
** SocketException class
*/

#pragma once

#include "BaseException.hpp"

namespace Lattice::Exceptions {
    class SocketException : public BaseException {
        public:
            using BaseException::BaseException;
    };
}
