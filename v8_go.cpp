#include "v8_c_bridge.h"
#include "v8_go.h"

extern "C" ValueTuple goCallbackHandler(String id, CallerInfo info, int argc, ValueTuple* argv);

extern "C" void initGoCallbackHanlder() {
     v8_Init(goCallbackHandler);
}