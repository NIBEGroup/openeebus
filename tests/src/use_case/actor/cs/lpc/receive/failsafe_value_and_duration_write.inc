namespace cs_lpc_test {
namespace receive {

static constexpr char failsafe_value_and_duration_write[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:HEMS_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 3
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_n:HeatPump_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 3
            }
          ]
        },
        {
          "msgCounter": 100
        },
        {
          "cmdClassifier": "write"
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
                "function": "deviceConfigurationKeyValueListData"
              },
              {
                "filter": [
                  [
                    {
                      "cmdControl": [
                        {
                          "partial": []
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "deviceConfigurationKeyValueListData": [
                  {
                    "deviceConfigurationKeyValueData": [
                      [
                        {
                          "keyId": 0
                        },
                        {
                          "value": [
                            {
                              "scaledNumber": [
                                {
                                  "number": 20
                                },
                                {
                                  "scale": 1
                                }
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "keyId": 1
                        },
                        {
                          "value": [
                            {
                              "duration": "PT3H"
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
})";

}  // namespace receive

}  // namespace cs_lpc_test
