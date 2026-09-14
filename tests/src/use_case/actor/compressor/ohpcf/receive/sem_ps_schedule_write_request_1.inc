namespace compressor_ohpcf_test {
namespace receive {

static constexpr char sem_ps_schedule_write_request_1[] = R"({
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
              "feature": 0
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
          "msgCounter": 16
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
                                "schedule": [
                                  {
                                    "startTime": "PT10S"
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
}  // namespace compressor_ohpcf_test
