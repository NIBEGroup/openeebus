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
 * @brief EV Charger example main function
 */

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "evsrv.h"
#include "src/ship/tls_certificate/tls_certificate.h"

static bool should_terminate = false;

static EvsrvObject* evsrv = NULL;

static void PrintUsage(void) {
  printf("Usage:\n");
  printf("  ev_charger <server_port> <remote_ski> <certificate_file> <private_key_file> [role]\n");
  printf("  role: \"client\", \"server\", or \"auto\" (default: auto)\n");
}

static void GracefulTerminate(int signal) {
  should_terminate = ((signal == SIGTERM) || (signal == SIGINT));
}

static void MainLoop(void) {
  char cmd[200] = "";

  signal(SIGINT, &GracefulTerminate);
  signal(SIGTERM, &GracefulTerminate);

  while (!should_terminate) {
    if (fgets(cmd, sizeof(cmd), stdin)) {
      if (strncmp(cmd, "exit", 4) == 0 && (cmd[4] == '\n' || cmd[4] == '\r' || cmd[4] == '\0')) {
        should_terminate = true;
      } else {
        EvsrvHandleCmd(evsrv, cmd);
      }
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 5 || argc > 6) {
    PrintUsage();
    return -1;
  }

  const int32_t port     = atoi(argv[1]);
  const char* remote_ski = argv[2];
  const char* cert       = argv[3];
  const char* pkey       = argv[4];
  const char* role       = (argc == 6) ? argv[5] : "auto";

  TlsCertificateObject* const tls_cert = TlsCertificateLoadX509KeyPair(cert, pkey);
  if (tls_cert == NULL) {
    printf("Failed to load TLS certificate and private key!\n");
    return -1;
  }

  evsrv = EvsrvOpen(port, role, tls_cert);
  if (evsrv == NULL) {
    printf("Failed to open EV Charger EEBUS service!\n");
    return -1;
  }

  EvsrvRegisterRemoteSki(evsrv, remote_ski);

  MainLoop();

  EvsrvClose(evsrv);

  return 0;
}
