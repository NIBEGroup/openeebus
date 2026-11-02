namespace cs_lpc_test {
namespace send {

static constexpr char failsafe_duration_local_notify[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
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
          "addressDestination": [
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
          "msgCounter": 36
        },
        {
          "cmdClassifier": "notify"
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
                          "keyId": 1
                        },
                        {
                          "value": [
                            {
                              "duration": "PT3H"
                            }
                          ]
                        },
                        {
                          "isValueChangeable": true
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

}  // namespace cs_lpc_test
