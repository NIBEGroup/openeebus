
/*
 *
 * Actors and Use Cases defined here:
 *
 * 1. ControllableSystem (Entity 3)
 *   - limitationOfPowerConsumption v1.0.0
 *
 * 2. Compressor (Entity 3,1)
 *   - optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility v1.0.0
 *
 * 3. DHWCircuit (Entity 4)
 *   - monitoringOfDhwTemperature v1.0.0
 *   - configurationOfDhwTemperature v1.0.0
 *   - monitoringOfDhwSystemFunction v1.0.0
 *
 * 4. HVACRoom (Entity 5,1,1)
 *   - configurationOfRoomHeatingTemperature v1.0.0
 *   - configurationOfRoomHeatingSystemFunction v1.0.0
 *   - monitoringOfRoomTemperature v1.0.0
 *
 * 5. HeatingCircuit (Entity 5,1,1)
 *   - visualizationOfHeatingAreaName v1.0.0 RC1
 *
 * 6. HeatingZone (Entity 5,1,1)
 *   - visualizationOfHeatingAreaName v1.0.0 RC1
 *
 * 7. OutdoorTemperatureSensor (Entity 6)
 *    - monitoringOfOutdoorTemperature v1.0.0
 *
 * 8. MonitoringAppliance (Entity 1)
 *    - monitoringOfGridConnectionPoint v1.0.0
 *
 * 9. VisualizationAppliance (Entity 2)
 *    - visualizationOfAggregatedBatteryData v1.0.0
 *    - visualizationOfAggregatedPhotovoltaicData v1.0.0
 */

static constexpr char use_case_data_less_cases[] = R"RAWFILE({
    "nodeManagementUseCaseData": [
      {
        "useCaseInformation": [
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    3
                  ]
                }
              ]
            },
            {
              "actor": "ControllableSystem"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "limitationOfPowerConsumption"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2,
                      3,
                      4
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "release"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    3,
                    1
                  ]
                }
              ]
            },
            {
              "actor": "Compressor"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC2"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    4
                  ]
                }
              ]
            },
            {
              "actor": "DHWCircuit"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "monitoringOfDhwTemperature"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ],
                [
                  {
                    "useCaseName": "configurationOfDhwTemperature"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1-public"
                  }
                ],
                [
                  {
                    "useCaseName": "monitoringOfDhwSystemFunction"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1-public"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    5,
                    1,
                    1
                  ]
                }
              ]
            },
            {
              "actor": "HVACRoom"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "configurationOfRoomHeatingTemperature"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ],
                [
                  {
                    "useCaseName": "configurationOfRoomHeatingSystemFunction"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ],)RAWFILE"
                                                   R"RAWFILE(
                [
                  {
                    "useCaseName": "monitoringOfRoomTemperature"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    5,
                    1,
                    1
                  ]
                }
              ]
            },
            {
              "actor": "HeatingCircuit"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "visualizationOfHeatingAreaName"
                  },
                  {
                    "useCaseVersion": "1.0.0 RC1"
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "release"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    5,
                    1,
                    1
                  ]
                }
              ]
            },
            {
              "actor": "HeatingZone"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "visualizationOfHeatingAreaName"
                  },
                  {
                    "useCaseVersion": "1.0.0 RC1"
                  },
                  {
                    "scenarioSupport": [
                      2
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "release"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    6
                  ]
                }
              ]
            },
            {
              "actor": "OutdoorTemperatureSensor"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "monitoringOfOutdoorTemperature"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    1
                  ]
                }
              ]
            },
            {
              "actor": "MonitoringAppliance"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "monitoringOfGridConnectionPoint"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2,
                      3,
                      4,
                      5,
                      6,
                      7
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "release"
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                },
                {
                  "entity": [
                    2
                  ]
                }
              ]
            },
            {
              "actor": "VisualizationAppliance"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "visualizationOfAggregatedBatteryData"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2,
                      3,
                      4
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ],
                [
                  {
                    "useCaseName": "visualizationOfAggregatedPhotovoltaicData"
                  },
                  {
                    "useCaseVersion": "1.0.0"
                  },
                  {
                    "useCaseAvailable": true
                  },
                  {
                    "scenarioSupport": [
                      1,
                      2,
                      3
                    ]
                  },
                  {
                    "useCaseDocumentSubRevision": "RC1"
                  }
                ]
              ]
            }
          ]
        ]
      }
    ]
  }
)RAWFILE";
