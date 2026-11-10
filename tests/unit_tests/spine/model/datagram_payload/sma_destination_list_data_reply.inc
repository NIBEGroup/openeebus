#include <string_view>

using namespace std::literals;

static constexpr std::string_view sma_destination_list_data_reply =
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
          "msgCounter": 7
        },
        {
          "msgCounterReference": 13
        },
        {
          "cmdClassifier": "reply"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "nodeManagementDestinationListData": [
                  {
                    "nodeManagementDestinationData": [
                      [
                        {
                          "deviceDescription": [
                            {
                              "deviceAddress": [
                                {
                                  "device": "d:_i:Demo_EVSE-234567890"
                                }
                              ]
                            },
                            {
                              "deviceType": "ChargingStation"
                            },
                            {
                              "networkFeatureSet": "smart"
                            }
                          ]
                        }
                      ]
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
