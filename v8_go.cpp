#ifndef V8_GO
#define V8_GO

#include "v8_c_bridge.h"
#include "v8_go.h"

extern "C" ValueTuple goCallbackHandler(String id, CallerInfo info, int argc, ValueTuple* argv);

extern "C" void initWithGoCallbackHanlder(const char* icu_data_file) {
     v8_Init(goCallbackHandler, icu_data_file);
}
#endif