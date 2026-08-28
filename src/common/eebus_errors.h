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
/**
 * @file errors.h
 * @brief EEBUS error codes
 */

#ifndef SRC_COMMON_ERRORS_H_
#define SRC_COMMON_ERRORS_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum EebusError {
    /* Common error codes */
    kEebusErrorOk,         /**< Ok */
    kEebusErrorNoChange,   /**< No change */
    kEebusErrorInit,       /**< Functionality has not been initialized */
    kEebusErrorDeinit,     /**< Functionality has not been deinitialized */
    kEebusErrorActivate,   /**< Functionality has not been activated */
    kEebusErrorDeactivate, /**< Functionality has not been deactivated */
    kEebusErrorThread,     /**< Thread management failed */

    /* Input data error codes */
    kEebusErrorInput,                   /**< Generic Input data error */
    kEebusErrorInputType,               /**< Invalid type / type error */
    kEebusErrorInputSize,               /**< Invalid size / size error */
    kEebusErrorInputArgument,           /**< Invalid argument */
    kEebusErrorInputArgumentNull,       /**< Invalid argument */
    kEebusErrorInputArgumentOutOfRange, /**< Invalid argument */

    /* Communication error codes */
    kEebusErrorCommunicationBegin,  /**< Communication start error */
    kEebusErrorCommunication,       /**< Generic Communication error */
    kEebusErrorCommunicationEnd,    /**< Communication end error */
    kEebusErrorCommunicationBusy,   /**< Busy Communication error */
    kEebusErrorCommunicationUpload, /**< Upload file to Uri error */

    /* Memory error codes */
    kEebusErrorMemory,         /**< Generic Memory error */
    kEebusErrorMemoryAllocate, /**< Memory error, unable to allocate */

    /* Filesystem error codes */
    kEebusErrorFileSystem,       /**< Generic File system error */
    kEebusErrorFileSystemAccess, /**< File system access error */
    kEebusErrorFileSystemNoFile, /**< No such a file exists */
    kEebusErrorFileSystemRemove, /**< Remove file error */
    kEebusErrorFileSystemCopy,   /**< Copy file error */

    /* Time error codes */
    kEebusErrorTime, /**< Generic Time error */

    /* Other error codes */
    kEebusErrorOther,          /**< Other error */
    kEebusErrorUnknown,        /**< Unknown error */
    kEebusErrorParse,          /**< Parsing error */
    kEebusErrorNotImplemented, /**< Functionality not implemented */
    kEebusErrorNotSupported,   /**< Functionality not supported */
    kEebusErrorNotAvailable,   /**< Data not available */
    kEebusErrorCompressFile,   /**< Compress files error */
    kEebusErrorOtherEnd,       /**< Other errors code end */

    kEebusErrorInvalid = 255 /**< Invalid error code */
};

typedef enum EebusError EebusError;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_COMMON_ERRORS_H_
