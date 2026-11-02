namespace gcp_mgcp_test {
namespace send {

static constexpr char device_configuration_key_value_reply[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:GridConnectionPoint_123456789"
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
          "msgCounter": 15
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
                                  "number": 75
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
}  // namespace gcp_mgcp_test
