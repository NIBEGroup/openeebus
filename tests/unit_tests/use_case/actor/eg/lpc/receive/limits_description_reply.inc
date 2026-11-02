namespace eg_lpc_test {
namespace receive {

static constexpr char limits_description_reply[] = R"({
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
              "feature": 2
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
              "feature": 2
            }
          ]
        },
        {
          "msgCounter": 34
        },
        {
          "msgCounterReference": 7
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
                "loadControlLimitDescriptionListData": [
                  {
                    "loadControlLimitDescriptionData": [
                      [
                        {
                          "limitId": 0
                        },
                        {
                          "limitType": "signDependentAbsValueLimit"
                        },
                        {
                          "limitCategory": "obligation"
                        },
                        {
                          "limitDirection": "consume"
                        },
                        {
                          "measurementId": 0
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "activePowerLimit"
                        }
                      ],
                      [
                        {
                          "limitId": 1
                        },
                        {
                          "limitType": "signDependentAbsValueLimit"
                        },
                        {
                          "limitCategory": "obligation"
                        },
                        {
                          "limitDirection": "produce"
                        },
                        {
                          "measurementId": 0
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "activePowerLimit"
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
