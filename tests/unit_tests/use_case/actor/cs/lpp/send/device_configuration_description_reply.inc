namespace cs_lpp_test {
namespace send {

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
          "msgCounter": 14
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
                "deviceConfigurationKeyValueDescriptionListData": [
                  {
                    "deviceConfigurationKeyValueDescriptionData": [
                      [
                        {
                          "keyId": 0
                        },
                        {
                          "keyName": "failsafeProductionActivePowerLimit"
                        },
                        {
                          "valueType": "scaledNumber"
                        },
                        {
                          "unit": "W"
                        }
                      ],
                      [
                        {
                          "keyId": 1
                        },
                        {
                          "keyName": "failsafeDurationMinimum"
                        },
                        {
                          "valueType": "duration"
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

}  // namespace cs_lpp_test
