namespace ma_mpc_test {
namespace receive {

static constexpr char electrical_connection_description_reply[] = R"({
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
              "feature": 1
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
              "feature": 1
            }
          ]
        },
        {
          "msgCounter": 8
        },
        {
          "msgCounterReference": 6
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
                "electricalConnectionDescriptionListData": [
                  {
                    "electricalConnectionDescriptionData": [
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "powerSupplyType": "ac"
                        },
                        {
                          "acConnectedPhases": 3
                        },
                        {
                          "positiveEnergyDirection": "consume"
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
}  // namespace ma_mpc_test
