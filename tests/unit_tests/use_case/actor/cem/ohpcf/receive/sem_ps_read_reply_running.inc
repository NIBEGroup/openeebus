namespace cem_ohpcf_test {
namespace receive {

static constexpr char sem_ps_read_reply_running[] = R"({
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
          "msgCounter": 31
        },
        {
          "msgCounterReference": 7
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
                                    "state": "running"
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

}  // namespace receive
}  // namespace cem_ohpcf_test
