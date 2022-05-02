/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MULTIPLEX_H
#define MULTIPLEX_H

    class Multiplex {
        public:
            virtual ~Multiplex();
            static Multiplex *getMultiplexer(int num);
            virtual void SetRow(int row) = 0;
            
        protected:
            Multiplex();
            
            static Multiplex *ptr;
    };
    
#endif
