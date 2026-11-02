namespace cem_ohpcf_test {
namespace receive {

static constexpr char sem_ps_report_state_completed_notify[] = R"({
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
          "msgCounter": 38
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
                "function": "smartEnergyManagementPsData"
              },
              {
                "filter": [
                  [
                    {
                      "cmdControl": [
                        {
                          "partial": []
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "smartEnergyManagementPsData": [
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
                                  }
                                ]
                              },
                              {
                                "state": [
                                  {
                                    "state": "completed"
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
