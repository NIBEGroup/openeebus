namespace cs_lpc_test {
namespace receive {

static constexpr char limits_write_multi_entry[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
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
          "addressDestination": [
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
          "msgCounter": 101
        },
        {
          "cmdClassifier": "write"
        },
        {
          "ackRequest": true
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "function": "loadControlLimitListData"
              },
              {
                "filter": [
                  [
                    {
                      "cmdControl": [
                        {
                          "partial": []
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "loadControlLimitListData": [
                  {
                    "loadControlLimitData": [
                      [
                        {
                          "limitId": 0
                        },
                        {
                          "isLimitActive": true
                        },
                        {
                          "value": [
                            {
                              "number": 100
                            },
                            {
                              "scale": 0
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "limitId": 99
                        },
                        {
                          "isLimitActive": true
                        },
                        {
                          "value": [
                            {
                              "number": 999999
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

}  // namespace cs_lpc_test
