/* 
 * File:   Handler.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef TCAM_HANDLER_H
#define TCAM_HANDLER_H

namespace TCAM {
    class Handler {
        public:
            virtual void callback() = 0;
    };
}

#endif