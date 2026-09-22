# Compile-Time Configuration

Several library behaviours can be tuned at compile time by defining preprocessor macros before the
relevant header is included, or by passing them as compiler flags (e.g. `-DMACRO=value` in CMake via
`target_compile_definitions`).

Each macro is guarded with `#ifndef`, so defining it on the command line always takes precedence over
the in-source default.

---

## `EEBUS_DATA_LIST_MAX_ELEMENTS_NUM`

**Header:** `src/common/eebus_data/eebus_data_list.h`  
**Default:** `10000`

Upper bound on the number of elements accepted by the data-list APIs and JSON parsing. Both
`EebusDataListDataAppend`, `EebusDataListDataAppendList`, and the JSON `FromJsonObjectItem` path
reject inputs that would push the element count above this cap, returning
`kEebusErrorMemoryAllocate` or a parse error respectively.

The default limit reduces denial-of-service risk from unexpectedly large inputs while staying well
within the capacity of typical host platforms. Embedded builds with tighter heap budgets should
lower it; applications that need to process larger lists can raise it.

**CMake example:**
```cmake
target_compile_definitions(my_target PRIVATE EEBUS_DATA_LIST_MAX_ELEMENTS_NUM=500)
```

---

## `EEBUS_WEBSOCKET_MAX_INPUT_MSG_SIZE`

**Header:** `src/ship/websocket/websocket_internal.h`  
**Default:** `65536` (64 KB)

Maximum total size in bytes of a single reassembled WebSocket input message. WebSocket frames may
arrive in multiple fragments; the library accumulates them until the final fragment is received. If
the running total exceeds this cap at any fragment boundary the connection is closed immediately and
the partial data is discarded.

The default provides a reasonable upper bound for SHIP/SPINE messages on memory-constrained devices.
Builds that exchange larger payloads (e.g. long schedule lists) can raise the limit; embedded builds
with limited heap can lower it.

**CMake example:**
```cmake
target_compile_definitions(my_target PRIVATE EEBUS_WEBSOCKET_MAX_INPUT_MSG_SIZE=131072)
```
