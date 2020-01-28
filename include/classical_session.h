#ifndef CLASSICAL_SESSION_H
#define CLASSICAL_SESSION_H

#include <stdint.h>

namespace Cascade {

class ClassicalSession
{
public:
    virtual ~ClassicalSession() = 0;
    virtual void start_iteration(unsigned iteration_nr, uint64_t shuffle_seed) = 0;
    virtual void ask_correct_parities() = 0;
};

} /* namespace Cascade */

#endif /* ifndef CLASSICAL_SESSION_H */