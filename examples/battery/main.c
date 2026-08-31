/*
 * Copyright 2026 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "examples/battery/battery.h"
#include "src/ship/tls_certificate/tls_certificate.h"

static bool should_terminate = false;

static void PrintUsage(void) {
  printf("Usage: battery <server_port> <remote_ski> <certificate_file> <private_key_file> [role]\n");
}

static void GracefulTerminate(int signal) {
  should_terminate = (signal == SIGTERM) || (signal == SIGINT);
}

int main(int argc, char** argv) {
  if ((argc < 5) || (argc > 6)) {
    PrintUsage();
    return -1;
  }

  const int32_t port     = atoi(argv[1]);
  const char* const role = (argc == 6) ? argv[5] : "auto";

  TlsCertificateObject* const tls_cert = TlsCertificateLoadX509KeyPair(argv[3], argv[4]);
  if (tls_cert == NULL) {
    printf("Failed to load TLS certificate and private key!\n");
    return -1;
  }

  BatteryObject* const battery = BatteryOpen(port, role, tls_cert);
  if (battery == NULL) {
    printf("Failed to open battery EEBUS service!\n");
    return -1;
  }

  BatteryRegisterRemoteSki(battery, argv[2]);

  signal(SIGINT, GracefulTerminate);
  signal(SIGTERM, GracefulTerminate);

  char cmd[200] = "";
  while (!should_terminate && fgets(cmd, sizeof(cmd), stdin)) {
    if (strncmp(cmd, "exit", 4) == 0 && ((cmd[4] == '\n') || (cmd[4] == '\r') || (cmd[4] == '\0'))) {
      should_terminate = true;
    } else {
      BatteryHandleCmd(battery, cmd);
    }
  }

  BatteryClose(battery);
  return 0;
}
