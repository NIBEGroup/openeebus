namespace gcp_mgcp_test {
namespace send {

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
              "device": "d:_n:GridConnectionPoint_123456789"
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
                          "measurementType": "energy"
                        },
                        {
                          "commodityType": "electricity"
                        },
                        {
                          "unit": "Wh"
                        },
                        {
                          "scopeType": "gridFeedIn"
                        }
                      ],
                      [
                        {
                          "measurementId": 2
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
                          "scopeType": "gridConsumption"
                        }
                      ],
                      [
                        {
                          "measurementId": 3
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
                          "measurementId": 4
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
                          "measurementId": 5
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
                          "measurementId": 6
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
                          "measurementId": 7
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
                          "measurementId": 8
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

}  // namespace send
}  // namespace gcp_mgcp_test
