namespace cs_lpc_test {
namespace send {

static constexpr char limits_notify_no_duration[] = R"({
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
          "msgCounter": 32
        },
        {
          "cmdClassifier": "notify"
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
                              "number": 200
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
