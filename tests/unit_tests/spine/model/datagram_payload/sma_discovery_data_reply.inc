#include <string_view>

using namespace std::literals;

static constexpr std::string_view sma_discovery_data_reply =
R"RAWFILE({
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
          "msgCounter": 3
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
                                "device": "d:_i:36013_3019197057"
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
                              "entityType": "GridConnectionPointOfPremises"
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
                                    4
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "ElectricityStorageSystem"
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
                                    4,1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "BatterySystem"
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
                              "entityType": "ElectricityGenerationSystem"
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
                                    5,1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "PVSystem"
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
                              "entityType": "CEM"
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
                                [)RAWFILE" R"RAWFILE(
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
                                    2
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
                                    2
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
                                    4,1
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
                                    4,1
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
                                    5,1
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
                                    5,1
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
                                  {)RAWFILE" R"RAWFILE(
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
                                    5,1
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
})RAWFILE"sv;
