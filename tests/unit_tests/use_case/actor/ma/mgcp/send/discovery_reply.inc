namespace ma_mgcp_test {
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
          "addressDestination": [
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
                                "device": "d:_n:HEMS_123456789"
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
                              "featureType": "ElectricalConnection"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "ElectricalConnection client"
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
                              "featureType": "Measurement"
                            },
                            {
                              "role": "client"
                            },
                            {
                              "description": "Measurement client"
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
                              "role": "client"
                            },
                            {
                              "description": "DeviceConfiguration client"
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
}  // namespace ma_mgcp_test
