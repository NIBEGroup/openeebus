namespace compressor_ohpcf_test {
namespace send {

static constexpr char sem_ps_data_announce_2[] = R"({
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
                1,
                1
              ]
            },
            {
              "feature": 1
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
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 17
        },
        {
          "cmdClassifier": "notify"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "smartEnergyManagementPsData": [
                  {
                    "nodeScheduleInformation": [
                      {
                        "nodeRemoteControllable": true
                      },
                      {
                        "supportsSingleSlotSchedulingOnly": true
                      },
                      {
                        "alternativesCount": 1
                      },
                      {
                        "totalSequencesCountMax": 1
                      },
                      {
                        "supportsReselection": false
                      }
                    ]
                  },
                  {
                    "alternatives": [
                      [
                        {
                          "relation": [
                            {
                              "alternativesId": 0
                            }
                          ]
                        },
                        {
                          "powerSequence": [
                            [
                              {
                                "description": [
                                  {
                                    "sequenceId": 0
                                  },
                                  {
                                    "powerUnit": "W"
                                  },
                                  {
                                    "valueSource": "calculatedValue"
                                  }
                                ]
                              },
                              {
                                "state": [
                                  {
                                    "state": "inactive"
                                  },
                                  {
                                    "sequenceRemoteControllable": true
                                  }
                                ]
                              },
                              {
                                "scheduleConstraints": [
                                  {
                                    "earliestStartTime": "PT0S"
                                  },
                                  {
                                    "latestEndTime": "P1D"
                                  }
                                ]
                              },
                              {
                                "operatingConstraintsInterrupt": [
                                  {
                                    "isPausable": false
                                  },
                                  {
                                    "isStoppable": true
                                  }
                                ]
                              },
                              {
                                "operatingConstraintsDuration": [
                                  {
                                    "activeDurationMin": "PT3M"
                                  }
                                ]
                              },
                              {
                                "powerTimeSlot": [
                                  [
                                    {
                                      "schedule": [
                                        {
                                          "slotNumber": 0
                                        }
                                      ]
                                    },
                                    {
                                      "valueList": [
                                        {
                                          "value": [
                                            [
                                              {
                                                "valueType": "powerMax"
                                              },
                                              {
                                                "value": [
                                                  {
                                                    "number": 2000
                                                  },
                                                  {
                                                    "scale": 0
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
}  // namespace compressor_ohpcf_test
