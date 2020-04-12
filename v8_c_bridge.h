// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the V8CBRIDGE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// V8CBRIDGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef V8CBRIDGE_EXPORTS
#define V8CBRIDGE_API __declspec(dllexport)
#elif defined(_MSC_VER)
#define V8CBRIDGE_API __declspec(dllimport)
#else // for it to be possible to use v8_c_bridge.h in cgo define it as blank
#define V8CBRIDGE_API
#endif

/*
// This class is exported from the dll
class V8CBRIDGE_API Cv8cbridge {
public:
	Cv8cbridge(void);
	// TODO: add your methods here.
};

extern V8CBRIDGE_API int nv8cbridge;

V8CBRIDGE_API int fnv8cbridge(void);
*/

#include <stddef.h>
#include <stdint.h>

#ifndef V8_C_BRIDGE_H
#define V8_C_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

	V8CBRIDGE_API typedef void* IsolatePtr;
	V8CBRIDGE_API typedef void* ContextPtr;
	V8CBRIDGE_API typedef void* PersistentValuePtr;

	V8CBRIDGE_API void v8_Free(void* ptr);

	V8CBRIDGE_API typedef struct {
		const char* ptr;
		int len;
	} String;

	V8CBRIDGE_API typedef String Error;
	V8CBRIDGE_API typedef String StartupData;
	V8CBRIDGE_API typedef String ByteArray;

	V8CBRIDGE_API typedef struct {
		size_t total_heap_size;
		size_t total_heap_size_executable;
		size_t total_physical_size;
		size_t total_available_size;
		size_t used_heap_size;
		size_t heap_size_limit;
		size_t malloced_memory;
		size_t peak_malloced_memory;
		size_t does_zap_garbage;
	} HeapStatistics;

	// NOTE! These values must exactly match the values in kinds.go. Any mismatch
	// will cause kinds to be misreported.
	V8CBRIDGE_API typedef enum {
		kUndefined = 0,
		kNull,
		kName,
		kString,
		kSymbol,
		kFunction,
		kArray,
		kObject,
		kBoolean,
		kNumber,
		kExternal,
		kInt32,
		kUint32,
		kDate,
		kArgumentsObject,
		kBooleanObject,
		kNumberObject,
		kStringObject,
		kSymbolObject,
		kNativeError,
		kRegExp,
		kAsyncFunction,
		kGeneratorFunction,
		kGeneratorObject,
		kPromise,
		kMap,
		kSet,
		kMapIterator,
		kSetIterator,
		kWeakMap,
		kWeakSet,
		kArrayBuffer,
		kArrayBufferView,
		kTypedArray,
		kUint8Array,
		kUint8ClampedArray,
		kInt8Array,
		kUint16Array,
		kInt16Array,
		kUint32Array,
		kInt32Array,
		kFloat32Array,
		kFloat64Array,
		kDataView,
		kSharedArrayBuffer,
		kProxy,
		kWebAssemblyCompiledModule,
		kNumKinds,
	} Kind;

	// Each kind can be represent using only single 64 bit bitmask since there
	// are less than 64 kinds so far.  If this grows beyond 64 kinds, we can switch
	// to multiple bitmasks or a dynamically-allocated array.
	V8CBRIDGE_API typedef uint64_t KindMask;

	V8CBRIDGE_API typedef struct {
		PersistentValuePtr Value;
		KindMask Kinds;
		Error error_msg;
	} ValueTuple;

	V8CBRIDGE_API typedef struct {
		String Funcname;
		String Filename;
		int Line;
		int Column;
	} CallerInfo;

	V8CBRIDGE_API typedef struct { int Major, Minor, Build, Patch; } Version;
	V8CBRIDGE_API extern Version version;

	// pointer to callback function
	V8CBRIDGE_API typedef ValueTuple(*GoCallbackHandlerPtr)(String id, CallerInfo info, int argc, ValueTuple* argv);

	// v8_Init must be called once before anything else.
	V8CBRIDGE_API void v8_Init(GoCallbackHandlerPtr callback_handler, const char* icu_data_file);

	// typedef unsigned int uint32_t;

	// isolate_ptr is optional
	V8CBRIDGE_API StartupData v8_CreateSnapshotDataBlob(const char* js, int includeCompiledFnCode, IsolatePtr isolate_ptr);

	// Pass NULL as startup_data to use the default snapshot.
	V8CBRIDGE_API extern IsolatePtr v8_Isolate_New(StartupData* startup_data);

	V8CBRIDGE_API extern ContextPtr v8_Isolate_NewContext(IsolatePtr isolate);
	V8CBRIDGE_API extern void       v8_Isolate_Terminate(IsolatePtr isolate);
	V8CBRIDGE_API extern void       v8_Isolate_Release(IsolatePtr isolate);

	V8CBRIDGE_API extern HeapStatistics       v8_Isolate_GetHeapStatistics(IsolatePtr isolate);
	V8CBRIDGE_API extern void                 v8_Isolate_LowMemoryNotification(IsolatePtr isolate);

	V8CBRIDGE_API extern ValueTuple     v8_Context_Run(ContextPtr ctx,
		const char* code, const char* filename);
	V8CBRIDGE_API extern PersistentValuePtr v8_Context_RegisterCallback(ContextPtr ctx,
		const char* name, const char* id);
	V8CBRIDGE_API extern PersistentValuePtr v8_Context_Global(ContextPtr ctx);
	V8CBRIDGE_API extern void               v8_Context_Release(ContextPtr ctx);

	V8CBRIDGE_API typedef enum {
		tSTRING,
		tBOOL,
		tFLOAT64,
		tINT64,
		tOBJECT,
		tARRAY,
		tARRAYBUFFER,
		tUNDEFINED,
		tDATE, // uses Float64 for msec since Unix epoch
	} ImmediateValueType;

	V8CBRIDGE_API typedef struct {
		ImmediateValueType Type;
		// Mem is used for String, ArrayBuffer, or Array. For Array, only len is
		// used -- ptr is ignored.
		ByteArray Mem;
		int Bool;
		double Float64;
		int64_t Int64;
	} ImmediateValue;

	V8CBRIDGE_API extern PersistentValuePtr v8_Context_Create(ContextPtr ctx, ImmediateValue val);

	V8CBRIDGE_API extern ValueTuple  v8_Value_Get(ContextPtr ctx, PersistentValuePtr value, const char* field);
	V8CBRIDGE_API extern Error       v8_Value_Set(ContextPtr ctx, PersistentValuePtr value,
		const char* field, PersistentValuePtr new_value);
	V8CBRIDGE_API extern ValueTuple  v8_Value_GetIdx(ContextPtr ctx, PersistentValuePtr value, int idx);
	V8CBRIDGE_API extern Error       v8_Value_SetIdx(ContextPtr ctx, PersistentValuePtr value,
		int idx, PersistentValuePtr new_value);
	V8CBRIDGE_API extern ValueTuple  v8_Value_Call(ContextPtr ctx,
		PersistentValuePtr func,
		PersistentValuePtr self,
		int argc, PersistentValuePtr* argv);
	V8CBRIDGE_API extern ValueTuple  v8_Value_New(ContextPtr ctx,
		PersistentValuePtr func,
		int argc, PersistentValuePtr* argv);
	V8CBRIDGE_API extern void   v8_Value_Release(ContextPtr ctx, PersistentValuePtr value);
	V8CBRIDGE_API extern String v8_Value_String(ContextPtr ctx, PersistentValuePtr value);

	V8CBRIDGE_API extern double    v8_Value_Float64(ContextPtr ctx, PersistentValuePtr value);
	V8CBRIDGE_API extern int64_t   v8_Value_Int64(ContextPtr ctx, PersistentValuePtr value);
	V8CBRIDGE_API extern int       v8_Value_Bool(ContextPtr ctx, PersistentValuePtr value);
	V8CBRIDGE_API extern ByteArray v8_Value_Bytes(ContextPtr ctx, PersistentValuePtr value);

	V8CBRIDGE_API extern ValueTuple v8_Value_PromiseInfo(ContextPtr ctx, PersistentValuePtr value,
		int* promise_state);

#ifdef __cplusplus
}
#endif

#endif  // !defined(V8_C_BRIDGE_H)
