namespace cs_lpp_test {
namespace send {

static constexpr char load_control_description_reply[] = R"({
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
          "msgCounter": 11
        },
        {
          "msgCounterReference": 10
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

}  // namespace send

}  // namespace cs_lpp_test
