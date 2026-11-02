namespace ma_mpc_test {
namespace receive {

static constexpr char electrical_connection_parameter_description_reply[] = R"({
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
              "feature": 1
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
              "feature": 1
            }
          ]
        },
        {
          "msgCounter": 9
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
                "electricalConnectionParameterDescriptionListData": [
                  {
                    "electricalConnectionParameterDescriptionData": [
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 0
                        },
                        {
                          "measurementId": 0
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "abc"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 1
                        },
                        {
                          "measurementId": 1
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "a"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 2
                        },
                        {
                          "measurementId": 2
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "b"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 3
                        },
                        {
                          "measurementId": 3
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "c"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 4
                        },
                        {
                          "measurementId": 4
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasurementType": "real"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 5
                        },
                        {
                          "measurementId": 5
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasurementType": "real"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 6
                        },
                        {
                          "measurementId": 6
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "a"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 7
                        },
                        {
                          "measurementId": 7
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "b"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 8
                        },
                        {
                          "measurementId": 8
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "c"
                        },
                        {
                          "acMeasurementType": "real"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 9
                        },
                        {
                          "measurementId": 9
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "a"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 10
                        },
                        {
                          "measurementId": 10
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "b"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 11
                        },
                        {
                          "measurementId": 11
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "c"
                        },
                        {
                          "acMeasuredInReferenceTo": "neutral"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 12
                        },
                        {
                          "measurementId": 12
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "a"
                        },
                        {
                          "acMeasuredInReferenceTo": "b"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 13
                        },
                        {
                          "measurementId": 13
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "b"
                        },
                        {
                          "acMeasuredInReferenceTo": "c"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 14
                        },
                        {
                          "measurementId": 14
                        },
                        {
                          "voltageType": "ac"
                        },
                        {
                          "acMeasuredPhases": "c"
                        },
                        {
                          "acMeasuredInReferenceTo": "a"
                        },
                        {
                          "acMeasurementType": "apparent"
                        },
                        {
                          "acMeasurementVariant": "rms"
                        }
                      ],
                      [
                        {
                          "electricalConnectionId": 0
                        },
                        {
                          "parameterId": 15
                        },
                        {
                          "measurementId": 15
                        },
                        {
                          "voltageType": "ac"
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
