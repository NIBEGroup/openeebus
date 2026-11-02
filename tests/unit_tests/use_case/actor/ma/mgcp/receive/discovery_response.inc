namespace ma_mgcp_test {
namespace receive {

static constexpr char discovery_response[] = R"({
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
                                "device": "d:_n:GridConnectionPoint_123456789"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
                                },
                                {
                                  "entity": [
                                    1
                                  ]
                                }
                              ]
                            },
                            {
                              "entityType": "GridConnectionPointOfPremises"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
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
                                        "read": []
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
                      ],
                      [
                        {
                          "description": [
                            {
                              "featureAddress": [
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
                              "featureType": "Measurement"
                            },
                            {
                              "role": "server"
                            },
                            {
                              "supportedFunction": [
                                [
                                  {
                                    "function": "measurementListData"
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
                                    "function": "measurementDescriptionListData"
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
                                    "function": "measurementConstraintsListData"
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
                              "description": "Measurement server"
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
                                  "device": "d:_n:GridConnectionPoint_123456789"
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
}  // namespace ma_mgcp_test
