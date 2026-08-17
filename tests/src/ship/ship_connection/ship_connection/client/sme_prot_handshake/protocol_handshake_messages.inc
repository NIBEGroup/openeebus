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
#include <string_view>

using std::literals::string_view_literals::operator""sv;

static constexpr std::string_view kProtHandshakeAbortUnexpectedMessage =
R"({
  "messageProtocolHandshakeError": [
    {
      "error": 2
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeAbortDataMismatchMsg =
R"({
  "messageProtocolHandshakeError": [
    {
      "error": 3
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeAnnounceMaxVersionMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "announceMax"
    },
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeSelectVersionMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "select"
    },
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeInvalidAnnounceMaxMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "announceMax"
    },
    {
      "version": [
        {
          "major": 0
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeUnsupportedMajorVersionMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "select"
    },
    {
      "version": [
        {
          "major": 0
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeUnsupportedMinorVersionMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "select"
    },
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 255
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeMissingFormatMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "select"
    },
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": null
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeUnsupportedFormatMsg =
R"({
  "messageProtocolHandshake": [
    {
      "handshakeType": "select"
    },
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF16"
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kProtHandshakeAnnounceMaxMsg =
R"({
  "announceMax": [
    {
      "version": [
        {
          "major": 1
        }, 
        {
          "minor": 0
        }
      ]
    },
    {
      "formats": [
        {
          "format": [
            "JSON-UTF8"
          ]
        }
      ]
    }
  ]
})"sv;


static constexpr std::string_view kProtHandshakeNoValueMsg =
R"({
  "messageProtocolHandshake": null
})"sv;
