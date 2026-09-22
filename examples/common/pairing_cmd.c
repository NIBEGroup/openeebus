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
 * @brief Console commands for SHIP Pairing Service implementation
 */

#include "examples/common/pairing_cmd.h"

#include <stdio.h>
#include <string.h>

#include "src/common/string_util.h"
#include "src/ship/tls_certificate/tls_certificate.h"

static const char* PairingCmdOwnFingerprint(const PairingCmd* self) {
  return TlsCertificateCalcFingerprintSha256(
      (const uint8_t*)TLS_CERTIFICATE_GET_CERTIFICATE(self->tls_certificate),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(self->tls_certificate)
  );
}

void PairingCmdConstruct(
    PairingCmd* self,
    EebusServiceObject* service,
    const TlsCertificateObject* tls_certificate,
    const char* ship_id
) {
  self->service         = service;
  self->tls_certificate = tls_certificate;
  self->ship_id         = ship_id;
  self->request         = NULL;
}

void PairingCmdDestruct(PairingCmd* self) {
  ShipPairingRequestDelete(self->request);
  self->request = NULL;
}

static void PairingCmdPrintUsage(void) {
  printf("SHIP Pairing Service commands:\n");
  printf("  pairing info                                  what an administrator collects about this node\n");
  printf("  pairing secret <32 hex digits>                the secret a request to this node is authenticated with\n");
  printf("  pairing announce <shipId> <fingerprint> <secret>   ask that node to trust this one\n");
  printf("  pairing stop                                  withdraw the announced request\n");
  printf("  pairing status                                what this node is doing\n");
}

/**
 * @brief Prints what an administrator has to carry to the other node
 *
 * The same values a QR code would carry (section 12), printed because these
 * examples have no display.
 */
static void PairingCmdInfo(const PairingCmd* self) {
  const char* const fingerprint = PairingCmdOwnFingerprint(self);
  const char* const curve       = TlsCertificateGetCurveName(
      (const uint8_t*)TLS_CERTIFICATE_GET_CERTIFICATE(self->tls_certificate),
      TLS_CERTIFICATE_GET_CERTIFICATE_SIZE(self->tls_certificate)
  );

  printf("SHIP ID:     %s\n", (self->ship_id != NULL) ? self->ship_id : "(none)");
  printf("Fingerprint: %s\n", (fingerprint != NULL) ? fingerprint : "(none)");
  printf("Curve:       %s\n", (curve != NULL) ? curve : "(unsupported)");
  printf("Secret:      set it with \"pairing secret <32 hex digits>\"\n");

  StringDelete((char*)fingerprint);
}

static void PairingCmdSetSecret(const PairingCmd* self, const char* secret_hex) {
  ShipPairingObject* const pairing = ShipNodeGetShipPairing(EebusServiceGetShipNode(self->service));
  if (pairing == NULL) {
    printf("This node has no shippairing evaluator\n");
    return;
  }

  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
  if (!StringHexToBytes(secret_hex, secret, sizeof(secret))) {
    printf("The secret has to be exactly %d hexadecimal digits\n", SHIP_PAIRING_SECRET_SIZE * 2);
    return;
  }

  SHIP_PAIRING_SET_SECRET(pairing, secret, sizeof(secret));
  memset(secret, 0, sizeof(secret));

  printf("Secret set. This node now evaluates requests addressed to it.\n");
}

static void PairingCmdAnnounce(PairingCmd* self, const char* for_id, const char* for_par, const char* secret_hex) {
  uint8_t secret[SHIP_PAIRING_SECRET_SIZE];
  if (!StringHexToBytes(secret_hex, secret, sizeof(secret))) {
    printf("The secret has to be exactly %d hexadecimal digits\n", SHIP_PAIRING_SECRET_SIZE * 2);
    return;
  }

  const ShipPairingRequestConfig config = {
      .instance_name   = self->ship_id,
      .for_id          = for_id,
      .for_par         = for_par,
      .secret          = secret,
      .secret_size     = sizeof(secret),
      .trust_id        = self->ship_id,
      .tls_certificate = self->tls_certificate,
  };

  ShipPairingRequest* const request = ShipPairingRequestCreate(&config);
  memset(secret, 0, sizeof(secret));

  if (request == NULL) {
    printf("Could not build the request. Check the SHIP ID, fingerprint and secret.\n");
    return;
  }

  // Section 5.5: announcing again replaces what was there, which is withdrawn
  // first.
  ShipPairingRequestDelete(self->request);
  self->request = request;

  const ShipPairingEntry* const entry = ShipPairingRequestGetEntry(request);
  if (ShipNodeAnnounceShipPairingRequest(EebusServiceGetShipNode(self->service), entry) != kEebusErrorOk) {
    printf("Could not announce the request\n");
    return;
  }

  printf("Announcing a request for %s\n", for_id);
  printf("  nonce:  %s\n", ShipPairingEntryGetTrustNonce(entry));
  printf("  digest: %s\n", ShipPairingEntryGetDigest(entry));
}

static void PairingCmdStop(PairingCmd* self) {
  if (self->request != NULL) {
    ShipPairingRequestStop(self->request);
  }

  ShipNodeAnnounceShipPairingRequest(EebusServiceGetShipNode(self->service), NULL);
  printf("Request withdrawn\n");
}

static void PairingCmdStatus(const PairingCmd* self) {
  ShipPairingObject* const pairing = ShipNodeGetShipPairing(EebusServiceGetShipNode(self->service));

  if (pairing != NULL) {
    printf("Processing addCu-requests: %s\n", SHIP_PAIRING_IS_ADD_CU_ACTIVATED(pairing) ? "yes" : "no");
    printf("Paired by a request:       %s\n", SHIP_PAIRING_HAS_TRUSTED_PEER(pairing) ? "yes" : "no");
  }

  if (self->request != NULL) {
    printf("Announcing a request:      %s\n", ShipPairingRequestIsAnnouncing(self->request) ? "yes" : "no");
  }
}

bool PairingCmdHandle(PairingCmd* self, const char* cmd) {
  if (strncmp(cmd, "pairing", strlen("pairing")) != 0) {
    return false;
  }

  char verb[32]        = "";
  char arg1[128]       = "";
  char arg2[128]       = "";
  char arg3[128]       = "";
  const int parsed_num = sscanf(cmd, "pairing %31s %127s %127s %127s", verb, arg1, arg2, arg3);

  if (parsed_num < 1) {
    PairingCmdPrintUsage();
    return true;
  }

  if (strcmp(verb, "info") == 0) {
    PairingCmdInfo(self);
  } else if ((strcmp(verb, "secret") == 0) && (parsed_num >= 2)) {
    PairingCmdSetSecret(self, arg1);
  } else if ((strcmp(verb, "announce") == 0) && (parsed_num >= 4)) {
    PairingCmdAnnounce(self, arg1, arg2, arg3);
  } else if (strcmp(verb, "stop") == 0) {
    PairingCmdStop(self);
  } else if (strcmp(verb, "status") == 0) {
    PairingCmdStatus(self);
  } else {
    PairingCmdPrintUsage();
  }

  return true;
}
