namespace cem_ohpcf_test {
namespace send {

static constexpr char sem_ps_resume_write[] = R"({
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
          "msgCounter": 13
        },
        {
          "cmdClassifier": "write"
        },
        {
          "ackRequest": true
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
                                    "state": "running"
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

}  // namespace send
}  // namespace cem_ohpcf_test
