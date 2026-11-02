namespace cs_lpc_test {
namespace send {

static constexpr char limits_reply[] = R"({
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
          "msgCounter": 18
        },
        {
          "msgCounterReference": 18
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
                "loadControlLimitListData": [
                  {
                    "loadControlLimitData": [
                      [
                        {
                          "limitId": 0
                        },
                        {
                          "isLimitChangeable": true
                        },
                        {
                          "isLimitActive": false
                        },
                        {
                          "value": [
                            {
                              "number": 4200
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

}  // namespace send

}  // namespace cs_lpc_test
