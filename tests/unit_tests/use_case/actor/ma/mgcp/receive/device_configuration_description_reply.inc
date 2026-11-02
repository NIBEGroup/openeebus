namespace ma_mgcp_test {
namespace receive {

static constexpr char device_configuration_description_reply[] = R"({
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
          "msgCounter": 14
        },
        {
          "msgCounterReference": 12
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
                "deviceConfigurationKeyValueDescriptionListData": [
                  {
                    "deviceConfigurationKeyValueDescriptionData": [
                      [
                        {
                          "keyId": 0
                        },
                        {
                          "keyName": "pvCurtailmentLimitFactor"
                        },
                        {
                          "valueType": "scaledNumber"
                        },
                        {
                          "unit": "pct"
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
}  // namespace ma_mgcp_test
