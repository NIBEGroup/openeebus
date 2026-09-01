/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "src/common/json.h"

#ifdef __freertos__
#include <cJSON.h>
#else
#include <cjson/cJSON.h>
#endif  // __freertos__
#include <stdbool.h>

struct JsonObject {
    cJSON impl;
};

JsonObject* JsonCreateObject(void) {
    return (JsonObject*)cJSON_CreateObject();
}

const JsonObject* JsonGetItem(const JsonObject* json_obj, const char* name, bool is_root) {
    if (is_root) {
        return (const JsonObject*)cJSON_GetObjectItem((const cJSON*)json_obj, name);
    } else {
        // Search for named item within sequence
        for (size_t j = 0; j < (size_t)cJSON_GetArraySize((const cJSON*)json_obj); ++j) {
            const cJSON* const json_el   = cJSON_GetArrayItem((const cJSON*)json_obj, (int)j);
            const cJSON* const json_item = cJSON_GetObjectItem(json_el, name);
            if (json_item != NULL) {
                return (const JsonObject*)json_item;
            }
        }

        return NULL;
    }
}

JsonObject* JsonAddObjectToArray(JsonObject* json_ar) {
    if (json_ar == NULL) {
        return NULL;
    }

    JsonObject* const json_obj = JsonCreateObject();
    if (JsonAddItemToArray(json_ar, json_obj)) {
        return json_obj;
    }

    JsonDelete(json_obj);
    return NULL;
}

JsonObject* JsonAddStringToArray(JsonObject* json_ar, const char* s) {
    if ((json_ar == NULL) || (s == NULL)) {
        return NULL;
    }

    JsonObject* const json_str = JsonCreateString(s);
    if (JsonAddItemToArray(json_ar, json_str)) {
        return json_str;
    }

    JsonDelete(json_str);
    return NULL;
}

JsonObject* JsonAddNumberToArray(JsonObject* json_obj, double num) {
    if (json_obj == NULL) {
        return NULL;
    }

    JsonObject* const json_num = JsonCreateNumber(num);
    if (JsonAddItemToArray(json_obj, json_num)) {
        return json_num;
    }

    JsonDelete(json_num);
    return NULL;
}

bool JsonAddItem(JsonObject* json_obj, const char* name, JsonObject* json_item, bool is_root) {
    if (is_root) {
        return cJSON_AddItemToObject((cJSON*)json_obj, name, (cJSON*)json_item);
    } else {
        JsonObject* const json_ar_item = JsonAddObjectToArray(json_obj);
        if (json_ar_item == NULL) {
            return false;
        }

        return cJSON_AddItemToObject((cJSON*)json_ar_item, name, (cJSON*)json_item);
    }
}

bool JsonIsNumber(const JsonObject* json_obj) {
    return cJSON_IsNumber((const cJSON*)json_obj);
}

double JsonGetNumber(const JsonObject* json_obj) {
    return cJSON_GetNumberValue((const cJSON*)json_obj);
}

JsonObject* JsonCreateNumber(double num) {
    return (JsonObject*)cJSON_CreateNumber(num);
}

bool JsonIsString(const JsonObject* json_object) {
    return cJSON_IsString((const cJSON*)json_object);
}

const char* JsonGetString(const JsonObject* json_object) {
    return cJSON_GetStringValue((const cJSON*)json_object);
}

JsonObject* JsonCreateString(const char* s) {
    return (JsonObject*)cJSON_CreateString(s);
}

bool JsonIsArray(const JsonObject* json_obj) {
    return cJSON_IsArray((const cJSON*)json_obj);
}

JsonObject* JsonCreateArray(void) {
    return (JsonObject*)cJSON_CreateArray();
}

size_t JsonGetArraySize(const JsonObject* json_obj) {
    return cJSON_GetArraySize((const cJSON*)json_obj);
}

bool JsonAddItemToArray(JsonObject* json_obj, JsonObject* json_el) {
    return cJSON_AddItemToArray((cJSON*)json_obj, (cJSON*)json_el);
}

JsonObject* JsonGetArrayItem(const JsonObject* json_obj, size_t idx) {
    return (JsonObject*)cJSON_GetArrayItem((const cJSON*)json_obj, (int)idx);
}

bool JsonIsBool(const JsonObject* json_obj) {
    return cJSON_IsBool((const cJSON*)json_obj);
}

bool JsonGetBool(const JsonObject* json_obj) {
    return cJSON_IsTrue((const cJSON*)json_obj);
}

JsonObject* JsonCreateBool(bool b) {
    return (JsonObject*)cJSON_CreateBool(b);
}

JsonObject* JsonParse(const char* s) {
    return (JsonObject*)cJSON_Parse(s);
}

JsonObject* JsonParseWithLength(const char* s, size_t len) {
    return (JsonObject*)cJSON_ParseWithLength(s, len);
}

char* JsonPrintUnformatted(const JsonObject* json_obj) {
    return cJSON_PrintUnformatted((const cJSON*)json_obj);
}

void JsonDelete(JsonObject* json_obj) {
    cJSON_Delete((cJSON*)json_obj);
}

void JsonFree(void* p) {
    cJSON_free(p);
}
