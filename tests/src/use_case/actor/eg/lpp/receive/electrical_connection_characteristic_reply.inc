namespace eg_lpp_test {
namespace receive {

static constexpr char electrical_connection_characteristic_reply[] = R"({
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
          "msgCounter": 50
        },
        {
          "msgCounterReference": 16
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
}  // namespace eg_lpp_test
