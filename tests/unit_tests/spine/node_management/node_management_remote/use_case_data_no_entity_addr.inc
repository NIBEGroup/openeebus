
/*
 * Actors and Use Cases defined here (address contains device only, no entity):
 *
 * 1. ControllableSystem (no entity in address)
 *   - limitationOfPowerConsumption v1.0.0
 *
 * 2. MonitoredUnit (no entity in address)
 *   - monitoringOfPowerConsumption v1.0.0
 */

static constexpr char use_case_data_no_entity_addr[] = R"RAWFILE({
    "nodeManagementUseCaseData": [
      {
        "useCaseInformation": [
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                }
              ]
            },
            {
              "actor": "ControllableSystem"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "limitationOfPowerConsumption"
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
                      4
                    ]
                  }
                ]
              ]
            }
          ],
          [
            {
              "address": [
                {
                  "device": "d:_n:HeatPump_123456789"
                }
              ]
            },
            {
              "actor": "MonitoredUnit"
            },
            {
              "useCaseSupport": [
                [
                  {
                    "useCaseName": "monitoringOfPowerConsumption"
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
                      5
                    ]
                  }
                ]
              ]
            }
          ]
        ]
      }
    ]
})RAWFILE";
