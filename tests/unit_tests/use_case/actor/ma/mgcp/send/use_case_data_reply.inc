namespace ma_mgcp_test {
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
          "msgCounter": 6
        },
        {
          "msgCounterReference": 4
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
                              "device": "d:_n:HEMS_123456789"
                            },
                            {
                              "entity": [
                                1
                              ]
                            }
                          ]
                        },
                        {
                          "actor": "MonitoringAppliance"
                        },
                        {
                          "useCaseSupport": [
                            [
                              {
                                "useCaseName": "monitoringOfGridConnectionPoint"
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
                                  2,
                                  3,
                                  4,
                                  5,
                                  6,
                                  7
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
}  // namespace ma_mgcp_test
