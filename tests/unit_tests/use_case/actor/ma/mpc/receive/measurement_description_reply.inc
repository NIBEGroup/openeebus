namespace ma_mpc_test {
namespace receive {

static constexpr char measurement_description_reply[] = R"({
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
          "msgCounterReference": 9
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
                "measurementDescriptionListData": [
                  {
                    "measurementDescriptionData": [
                      [
                        {
                          "measurementId": 0
                        },
                        {
                          "measurementType": "power"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "acPowerTotal"
                        }
                      ],
                      [
                        {
                          "measurementId": 1
                        },
                        {
                          "measurementType": "power"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "acPower"
                        }
                      ],
                      [
                        {
                          "measurementId": 2
                        },
                        {
                          "measurementType": "power"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "acPower"
                        }
                      ],
                      [
                        {
                          "measurementId": 3
                        },
                        {
                          "measurementType": "power"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "W"
                        },
                        {
                          "scopeType": "acPower"
                        }
                      ],
                      [
                        {
                          "measurementId": 4
                        },
                        {
                          "measurementType": "energy"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "Wh"
                        },
                        {
                          "scopeType": "acEnergyConsumed"
                        }
                      ],
                      [
                        {
                          "measurementId": 5
                        },
                        {
                          "measurementType": "energy"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "Wh"
                        },
                        {
                          "scopeType": "acEnergyProduced"
                        }
                      ],
                      [
                        {
                          "measurementId": 6
                        },
                        {
                          "measurementType": "current"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "A"
                        },
                        {
                          "scopeType": "acCurrent"
                        }
                      ],
                      [
                        {
                          "measurementId": 7
                        },
                        {
                          "measurementType": "current"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "A"
                        },
                        {
                          "scopeType": "acCurrent"
                        }
                      ],
                      [
                        {
                          "measurementId": 8
                        },
                        {
                          "measurementType": "current"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "A"
                        },
                        {
                          "scopeType": "acCurrent"
                        }
                      ],
                      [
                        {
                          "measurementId": 9
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 10
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 11
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 12
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 13
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 14
                        },
                        {
                          "measurementType": "voltage"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "V"
                        },
                        {
                          "scopeType": "acVoltage"
                        }
                      ],
                      [
                        {
                          "measurementId": 15
                        },
                        {
                          "measurementType": "frequency"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "Hz"
                        },
                        {
                          "scopeType": "acFrequency"
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
