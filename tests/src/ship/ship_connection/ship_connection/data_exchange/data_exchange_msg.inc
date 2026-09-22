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

static constexpr std::string_view kDataExchangeWebsocketReceivedMsg =
R"({
  "data": [
    {
      "header": [
        {
          "protocolId" : "ee1.0"
        }
      ]
    },
    {
      "payload": {
        "datagram": [
          {
            "header": [
              {
                "specificationVersion": "1.3.0"
              },
              {
                "addressSource": [
                  {
                    "device": "d:_i:Demo_EVSE-234567890"
                  },
                  {
                    "entity": [0]
                  },
                  {
                    "feature": 0
                  }
                ]
              },
              {
                "addressDestination": [
                  {
                    "entity": [0]
                  },
                  {
                    "feature": 0
                  }
                ]
              },
              {
                "msgCounter": 1
              },
              {
                "cmdClassifier": "read"
              }
            ]
          },
          {
            "payload": [
              {
                "cmd": [
                  [
                    {
                      "nodeManagementDetailedDiscoveryData": []
                    }
                  ]
                ]
              }
            ]
          }
        ]
      }
    }
  ]
})"sv;

static constexpr std::string_view kDataExchangeSpineDataReceived =
R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_i:Demo_EVSE-234567890"
            },
            {
              "entity": [0]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "addressDestination": [
            {
              "entity": [0]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 1
        },
        {
          "cmdClassifier": "read"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "nodeManagementDetailedDiscoveryData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})"sv;

static constexpr std::string_view kDataExchangeWebsocketWriteMsg =
R"({
  "data": [
    {
      "header": [
        {
          "protocolId": "ee1.0"
        }
      ]
    },
    {
      "payload": {
        "datagram": [
          {
            "header": [
              {
                "specificationVersion": "1.3.0"
              },
              {
                "addressSource": [
                  {
                    "device": "HeatGenerationSystem"
                  },
                  {
                    "entity": [0]
                  },
                  {
                    "feature": 0
                  }
                ]
              },
              {
                "addressDestination": [
                  {
                    "device": "HeatGenerationSystem"
                  },
                  {
                    "entity": [0]
                  },
                  {
                    "feature": 0
                  }
                ]
              },
              {
                "msgCounter": 1
              },
              {
                "cmdClassifier": "read"
              }
            ]
          },
          {
            "payload": [
              {
                "cmd": [
                  [
                    {
                      "nodeManagementDetailedDiscoveryData": []
                    }
                  ]
                ]
              }
            ]
          }
        ]
      }
    }
  ]
})"sv;

static constexpr std::string_view kDataExchangeSpineDataToSend =
R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "HeatGenerationSystem"
            },
            {
              "entity": [0]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "HeatGenerationSystem"
            },
            {
              "entity": [0]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 1
        },
        {
          "cmdClassifier": "read"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "nodeManagementDetailedDiscoveryData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})"sv;
