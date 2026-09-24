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
 * @file
 * @brief When a SHIP node trusts a foreign SKI
 */

#ifndef SRC_SHIP_API_TRUST_MODE_H_
#define SRC_SHIP_API_TRUST_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief When a SHIP node trusts a foreign SKI, see docs/post_trust.md
 *
 * SHIP 5.2 permits both approaches and mandates neither.
 */
typedef enum {
  /** Trust before data exchange: an unknown SKI is refused at the socket */
  kEebusTrustModePreTrust = 0,
  /**
   * Trust within SME connection state "hello": an unknown SKI is accepted
   * provisionally and its connection held in the PENDING phase until the
   * application approves or refuses it. Required for user verification
   * (SHIP 12.3.1.3), where the SKI has to be shown to a user first.
   */
  kEebusTrustModePostTrust = 1,
} EebusTrustMode;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_API_TRUST_MODE_H_
