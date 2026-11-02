namespace eg_lpp_test {
namespace receive {

static constexpr char electrical_connection_characteristic_reply_ref_28[] = R"({
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
          "msgCounter": 68
        },
        {
          "msgCounterReference": 28
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
                          "characteristicType": "powerProductionNominalMax"
                        },
                        {
                          "value": [
                            {
                              "number": 14000
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
}  // namespace eg_lpp_test
