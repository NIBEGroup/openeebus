namespace eg_lpp_test {
namespace receive {

static constexpr char limits_reply_ref_25[] = R"({
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
          "msgCounter": 65
        },
        {
          "msgCounterReference": 25
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
                      ],
                      [
                        {
                          "limitId": 1
                        },
                        {
                          "isLimitChangeable": true
                        },
                        {
                          "isLimitActive": true
                        },
                        {
                          "value": [
                            {
                              "number": 8000
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
