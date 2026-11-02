namespace cs_lpp_test {
namespace send {

static constexpr char discovery_reply[] = R"({
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
          "msgCounter": 2
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
                                    "function": "nodeManagementDestinationListData"
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
                              "featureType": "DeviceDiagnosis"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "DeviceDiagnosis client"
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
                              "description": "LoadControl server"
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
                                    "function": "deviceConfigurationKeyValueDescriptionListData"
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
                                ]
                              ]
                            },
                            {
                              "description": "DeviceConfiguration server"
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
                              "description": "DeviceDiagnosis server"
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
                              "description": "ElectricalConnection server"
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
})";

}  // namespace send

}  // namespace cs_lpp_test
