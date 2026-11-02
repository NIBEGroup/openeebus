namespace cem_ohpcf_test {
namespace receive {

static constexpr char use_case_reply[] = R"RAWFILE({
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
                0
              ]
            },
            {
              "feature": 0
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
                0
              ]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 29
        },
        {
          "msgCounterReference": 4
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
                                3
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "IdentifiableNode"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "nodeIdentification"
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
                                3
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "MonitoredUnit"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "monitoringOfPowerConsumption"
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
                                  5
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
                                3,
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "MonitoredUnit"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "monitoringOfPowerConsumption"
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
                                "useCaseName": "configurationOfDhwSystemFunction"
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
                            ],
                            [
                              {
                                "useCaseName": "monitoringOfRoomHeatingSystemFunction"
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
                                "useCaseName": "visualizationOfHeatingAreaName"
                              },
                              {
                                "useCaseVersion": "1.0.0 RC1"
                              },
                              {
                                "scenarioSupport": [
                                  3
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
                              }
                            ],)RAWFILE" R"RAWFILE(
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
            ]
          ]
        }
      ]
    }
  ]
})RAWFILE";

}  // namespace receive
}  // namespace cem_ohpcf_test
