namespace eg_lpp_test {
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
          "msgCounter": 22
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
                            "deviceType": "HeatGenerationSystem"
                          },
                          {
                            "networkFeatureSet": "smart"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
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
                              "entityType": "Inverter"
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
                              "featureType": "NodeManagement"
                            },
                            {
                              "role": "special"
                            },
                            {
                              "supportedFunction": [
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
                                    "function": "nodeManagementBindingRequestCall"
                                  },
                                  {
                                    "possibleOperations": []
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
                                    "function": "nodeManagementSubscriptionRequestCall"
                                  },
                                  {
                                    "possibleOperations": []
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
                                    "function": "nodeManagementDestinationListData"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
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
                              "featureType": "DeviceDiagnosis"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "DeviceDiagnosis Client"
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
                                        "read": []
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
                                        "read": []
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
                            },
                            {
                              "description": "LoadControl Server"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 3
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
                                    "function": "deviceConfigurationKeyValueListData"
                                  },
                                  {
                                    "possibleOperations": [
                                      {
                                        "read": []
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
                                    "function": "deviceConfigurationKeyValueDescriptionListData"
                                  },
                                  {)RAWFILE" R"RAWFILE(
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
                              "description": "DeviceConfiguration Server"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 4
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
                                ]
                              ]
                            },
                            {
                              "description": "DeviceDiagnosis Server"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 5
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
                                        "read": []
                                      }
                                    ]
                                  }
                                ]
                              ]
                            },
                            {
                              "description": "ElectricalConnection Server"
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
                                  "device": "d:_n:HeatPump_123456789"
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
                              "featureType": "LoadControl"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "LoadControl Client"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 7
                                }
                              ]
                            },
                            {
                              "featureType": "DeviceConfiguration"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "DeviceConfiguration Client"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 8
                                }
                              ]
                            },
                            {
                              "featureType": "ElectricalConnection"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "ElectricalConnection Client"
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
                                  "device": "d:_n:HeatPump_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                },
                                {
                                  "feature": 9
                                }
                              ]
                            },
                            {
                              "featureType": "Measurement"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "Measurement Client"
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
}  // namespace eg_lpp_test
