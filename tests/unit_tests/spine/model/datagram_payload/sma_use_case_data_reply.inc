#include <string_view>

using namespace std::literals;

static constexpr std::string_view sma_use_case_data_reply =
R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_i:36013_3019197057"
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
              "device": "d:_i:Demo_EVSE-234567890"
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
          "msgCounter": 10
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
                              "device": "d:_i:36013_3019197057"
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
                                  1,2,3,4,5
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
                              "device": "d:_i:36013_3019197057"
                            },
                            {
                              "entity": [
                                2
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "GridConnectionPoint"
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
                                  1,2,3,4,5,6,7
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC7"
                              }
                            ]
                          ]
                        }
                      ],
                      [
                        {
                          "address": [
                            {
                              "device": "d:_i:36013_3019197057"
                            },
                            {
                              "entity": [
                                3
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "CEM"
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
                                  1,2
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
                              "device": "d:_i:36013_3019197057"
                            },
                            {
                              "entity": [
                                3
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
                                  1,2,3,4,5
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC2"
                              }
                            ],
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
                                  1,2,3,4,5
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC2"
                              }
                            ],
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
                                  1,2,3,4,5
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
                              "device": "d:_i:36013_3019197057"
                            },
                            {
                              "entity": [
                                6
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "CEM"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "flexibleStartForWhiteGoods"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC1"
                              }
                            ],
                            [
                              {
                                "useCaseName": "flexibleStartForWhiteGoods"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC1"
                              }
                            ],
                            [
                              {
                                "useCaseName": "flexibleStartForWhiteGoods"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,2,3,4
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
                              "device": "d:_i:36013_3019197057"
                            },
                            {
                              "entity": [
                                6
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
                                  1,2,3,4,5
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC2"
                              }
                            ],
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
                                  1,2,3,4,5
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC2"
                              }
                            ],
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
                                  1,2,3,4,5
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "RC2"
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
})"sv;
