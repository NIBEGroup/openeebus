#include <string_view>

using namespace std::literals;

static constexpr std::string_view sma_subscription_request_call =
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
              "entity": [
                0
              ]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_i:36013_3019197057"
            },
            {
              "entity": [
                0
              ]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 3
        },
        {
          "cmdClassifier": "call"
        },
        {
          "ackRequest": true
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "nodeManagementSubscriptionRequestCall": [
                  {
                    "subscriptionRequest": [
                      {
                        "clientAddress": [
                          {
                            "device": "d:_i:Demo_EVSE-234567890"
                          },
                          {
                            "entity": [
                              0
                            ]
                          },
                          {
                            "feature": 0
                          }
                        ]
                      },
                      {
                        "serverAddress": [
                          {
                            "device": "d:_i:36013_3019197057"
                          },
                          {
                            "entity": [
                              0
                            ]
                          },
                          {
                            "feature": 0
                          }
                        ]
                      },
                      {
                        "serverFeatureType": "NodeManagement"
                      }
                    ]
                  }
                ]
              }
            ]
          ]
        }
      ]
    }
  ]
})"sv;
