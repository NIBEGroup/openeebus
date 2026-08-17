namespace eg_lpc_test {
namespace receive {

static constexpr char electrical_connection_characteristic_reply_ref_29[] = R"({
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
              "feature": 5
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
              "feature": 4
            }
          ]
        },
        {
          "msgCounter": 67
        },
        {
          "msgCounterReference": 29
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
                "electricalConnectionCharacteristicListData": [
                  {
                    "electricalConnectionCharacteristicData": [
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "characteristicId": 0
                        },
                        {
                          "characteristicContext": "entity"
                        },
                        {
                          "characteristicType": "powerConsumptionNominalMax"
                        },
                        {
                          "value": [
                            {
                              "number": 12000
                            },
                            {
                              "scale": 0
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
}  // namespace eg_lpc_test
