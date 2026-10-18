namespace cem_ohpcf_test {
namespace receive {

static constexpr char sem_ps_announce_notify_2[] = R"({
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
              "feature": 1
            }
          ]
        },
        {
          "msgCounter": 40
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
                                    "earliestStartTime": "PT5M"
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
                                    "activeDurationMin": "PT10M"
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
                                                    "number": 3000
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

}  // namespace receive
}  // namespace cem_ohpcf_test
