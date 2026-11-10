namespace eg_lpc_test {
namespace send {

static constexpr char limits_write_delete_duration[] = R"({
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
          "msgCounter": 21
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
                  ],
                  [
                    {
                      "cmdControl": [
                        {
                          "delete": []
                        }
                      ]
                    },
                    {
                      "loadControlLimitListDataSelectors": [
                        {
                          "limitId": 0
                        }
                      ]
                    },
                    {
                      "loadControlLimitDataElements": [
                        {
                          "timePeriod": []
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
                              "number": 3000
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
}  // namespace eg_lpc_test
