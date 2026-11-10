namespace ma_mpc_test {
namespace receive {

static constexpr char measurement_notify_voltage[] = R"({
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
          "msgCounter": 17
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
                          "measurementId": 9
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 110
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
                          "measurementId": 10
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 1205
                            },
                            {
                              "scale": -1
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 11
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 130
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
                          "measurementId": 12
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 14037
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
                          "measurementId": 13
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 150
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
                          "measurementId": 14
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 16
                            },
                            {
                              "scale": 1
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
