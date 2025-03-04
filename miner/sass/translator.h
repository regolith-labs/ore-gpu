#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <SASS/Tree/Tree.h>
#include <hashx.h>

SASS::Program* translate_hashx_to_sass(const hashx_program& prog);

#endif // TRANSLATOR_H 