namespace eg_lpp_test {
namespace receive {

static constexpr char device_configuration_key_value_list_reply_ref_26[] = R"({
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
          "msgCounter": 66
        },
        {
          "msgCounterReference": 26
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
                                  "number": 600
                                },
                                {
                                  "scale": 0
                                }
                              ]
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

}  // namespace receive
}  // namespace eg_lpp_test
