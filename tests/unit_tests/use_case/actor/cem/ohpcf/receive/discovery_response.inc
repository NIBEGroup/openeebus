namespace cem_ohpcf_test {
namespace receive {

static constexpr char discovery_response[] = R"RAWFILE({
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
          "msgCounter": 26
        },
        {
          "msgCounterReference": 1
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
                "nodeManagementDetailedDiscoveryData": [
                  {
                    "specificationVersionList": [
                      {
                        "specificationVersion": [
                          "1.3.0"
                        ]
                      }
                    ]
                  },
                  {
                    "deviceInformation": [
                      {
                        "description": [
                          {
                            "deviceAddress": [
                              {
                                "device": "d:_n:HeatPump_123456789"
                              }
                            ]
                          },
                          {
                            "deviceType": "Generic"
                          },
                          {
                            "networkFeatureSet": "smart"
                          },
                          {
                            "lastStateChange": "added"
                          }
                        ]
                      }
                    ]
                  },
                  {
                    "entityInformation": [
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    0
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "DeviceInformation"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "CEM"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    2
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "CEM"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "HeatPumpAppliance"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    3,
                                    1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "Compressor"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    4
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "DHWCircuit"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    5
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "HeatingCircuit"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    5,
                                    1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "HeatingZone"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
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
                              "entityType": "HVACRoom"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "entityAddress": [
                                {
                                  "entity": [
                                    6
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "TemperatureSensor"
                            }
                          ]
                        }
                      ]
                    ]
                  },
                  {
                    "featureInformation": [
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
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
                              "featureType": "NodeManagement"
                            },
                            {
                              "role": "special"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "nodeManagementBindingData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementBindingDeleteCall"
                                  },
                                  {
                                    "possibleOperations": []
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementBindingRequestCall"
                                  },
                                  {
                                    "possibleOperations": []
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementDetailedDiscoveryData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementSubscriptionData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementSubscriptionDeleteCall"
                                  },
                                  {
                                    "possibleOperations": []
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementSubscriptionRequestCall"
                                  },
                                  {
                                    "possibleOperations": []
                                  }
                                ],
                                [
                                  {
                                    "function": "nodeManagementUseCaseData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    0
                                  ]
                                },
                                {
                                  "feature": 1
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceClassification"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceClassificationManufacturerData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
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
                              "featureType": "Generic"
                            },
                            {
                              "role": "client"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    2
                                  ]
                                },
                                {
                                  "feature": 1
                                }
                              ]
                            },
                            {
                              "featureType": "Generic"
                            },
                            {
                              "role": "client"
                            }
                          ]
                        }
                      ],
                      [)RAWFILE" R"RAWFILE(
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 4
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceClassification"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceClassificationManufacturerData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 7
                                }
                              ]
                            },
                            {
                              "featureType": "ElectricalConnection"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "electricalConnectionCharacteristicListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "electricalConnectionDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "electricalConnectionParameterDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 10
                                }
                              ]
                            },
                            {
                              "featureType": "LoadControl"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "loadControlLimitDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "loadControlLimitListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 11
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 24
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceConfiguration"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceConfigurationKeyValueDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "deviceConfigurationKeyValueListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 1000
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceDiagnosis"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceDiagnosisHeartbeatData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "deviceDiagnosisStateData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            },
                            {
                              "description": "Device diagnosis server feature"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3
                                  ]
                                },
                                {
                                  "feature": 1001
                                }
                              ]
                            },
                            {
                              "featureType": "Generic"
                            },
                            {)RAWFILE" R"RAWFILE(
                              "role": "client"
                            },
                            {
                              "description": "Heartbeat device diagnosis client feature"
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3,
                                    1
                                  ]
                                },
                                {
                                  "feature": 7
                                }
                              ]
                            },
                            {
                              "featureType": "ElectricalConnection"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "electricalConnectionDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "electricalConnectionParameterDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3,
                                    1
                                  ]
                                },
                                {
                                  "feature": 11
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    3,
                                    1
                                  ]
                                },
                                {
                                  "feature": 19
                                }
                              ]
                            },
                            {
                              "featureType": "SmartEnergyManagementPs"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "smartEnergyManagementPsData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    4
                                  ]
                                },
                                {
                                  "feature": 9
                                }
                              ]
                            },
                            {
                              "featureType": "HVAC"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "hvacOperationModeDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionSetpointRelationListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionOperationModeRelationListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacOverrunDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacOverrunListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    4
                                  ]
                                },
                                {
                                  "feature": 11
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {)RAWFILE" R"RAWFILE(
                                    "function": "measurementDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    4
                                  ]
                                },
                                {
                                  "feature": 18
                                }
                              ]
                            },
                            {
                              "featureType": "Setpoint"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "setpointConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "setpointDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "setpointListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5
                                  ]
                                },
                                {
                                  "feature": 4
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceClassification"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceClassificationUserData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5,
                                    1
                                  ]
                                },
                                {
                                  "feature": 4
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceClassification"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceClassificationUserData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5,
                                    1,
                                    1
                                  ]
                                },
                                {
                                  "feature": 4
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceClassification"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "deviceClassificationUserData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5,
                                    1,
                                    1
                                  ]
                                },
                                {
                                  "feature": 9
                                }
                              ]
                            },
                            {
                              "featureType": "HVAC"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "hvacOperationModeDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionSetpointRelationListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "hvacSystemFunctionOperationModeRelationListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5,
                                    1,
                                    1
                                  ]
                                },
                                {
                                  "feature": 11
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },)RAWFILE" R"RAWFILE(
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    5,
                                    1,
                                    1
                                  ]
                                },
                                {
                                  "feature": 18
                                }
                              ]
                            },
                            {
                              "featureType": "Setpoint"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "setpointConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "setpointDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "setpointListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      },
                                      {
                                        "write": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ]
                              ]
                            }
                          ]
                        }
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
                                {
                                  "entity": [
                                    6
                                  ]
                                },
                                {
                                  "feature": 11
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementConstraintsListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementDescriptionListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
                                        ]
                                      }
                                    ]
                                  }
                                ],
                                [
                                  {
                                    "function": "measurementListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": [
                                          {
                                            "partial": []
                                          }
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
