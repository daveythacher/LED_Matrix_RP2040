/* 
 * File:   Frame.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_FRAME_H
#define CONCURRENT_FRAME_H

namespace Concurrent {
    // Tool to create in order from out of order context
    template <typename T> class Frame {
        public:
            virtual ~Frame();

            // Superscalar will join before return
            //  Syncing should provide full conherency
            T *run();

        protected:
            Frame();

            virtual T *func() = 0;
            void sync_cpu();
            virtual void sync_cache();
    };

    // TODO: Thread Pool and Frame Manager?
}

#endif
