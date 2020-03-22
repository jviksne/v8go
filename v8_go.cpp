#include "v8_c_bridge.h"
#include "v8_go.h"

extern "C" ValueTuple goCallbackHandler(String id, CallerInfo info, int argc, ValueTuple* argv);

extern "C" void initWithGoCallbackHanlder(char* icu_data_file) {
     v8_Init(goCallbackHandler, icu_data_file);
}