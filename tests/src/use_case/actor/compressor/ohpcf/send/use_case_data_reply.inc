namespace compressor_ohpcf_test {
namespace send {

static constexpr char use_case_data_reply[] = R"({
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
          "msgCounter": 6
        },
        {
          "msgCounterReference": 10
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
                "nodeManagementUseCaseData": [
                  {
                    "useCaseInformation": [
                      [
                        {
                          "address": [
                            {
                              "device": "d:_n:HeatPump_123456789"
                            },
                            {
                              "entity": [
                                1,
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "Compressor"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility"
                              },
                              {
                                "useCaseVersion": "1.0.0"
                              },
                              {
                                "useCaseAvailable": true
                              },
                              {
                                "scenarioSupport": [
                                  1,
                                  2
                                ]
                              },
                              {
                                "useCaseDocumentSubRevision": "release"
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
