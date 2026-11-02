namespace ma_mpc_test {
namespace receive {

// Cross-scenario partial notify: power total (0), energy produced (5),
// voltage phase AB (12), frequency (15) — tests that a single notify can
// carry measurements from multiple scenarios and overwrite stored values.
static constexpr char measurement_notify_mixed[] = R"({
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
          "msgCounter": 20
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
                "function": "measurementListData"
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
                "measurementListData": [
                  {
                    "measurementData": [
                      [
                        {
                          "measurementId": 0
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 5500
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 5
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 999999
                            },
                            {
                              "scale": -3
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 12
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 39600
                            },
                            {
                              "scale": -2
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 15
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 4999
                            },
                            {
                              "scale": -2
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
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
