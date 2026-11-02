namespace mu_mpc_test {
namespace send {

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
              "feature": 6
            }
          ]
        },
        {
          "msgCounter": 16
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
                              "number": 23500
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
                          "measurementId": 10
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 23600
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
                          "measurementId": 11
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 23400
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
                          "measurementId": 12
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 40500
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
                              "number": 40600
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
                          "measurementId": 14
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 40400
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

}  // namespace send
}  // namespace mu_mpc_test
