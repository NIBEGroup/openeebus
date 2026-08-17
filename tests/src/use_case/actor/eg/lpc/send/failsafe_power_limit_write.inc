namespace eg_lpc_test {
namespace send {

static constexpr char failsafe_power_limit_write[] = R"({
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
          "msgCounter": 24
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
                                  "number": 600
                                },
                                {
                                  "scale": 0
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
          ]
        }
      ]
    }
  ]
})";

}  // namespace send
}  // namespace eg_lpc_test
