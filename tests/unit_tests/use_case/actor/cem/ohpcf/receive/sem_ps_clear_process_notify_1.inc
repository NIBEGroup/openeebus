namespace cem_ohpcf_test {
namespace receive {

static constexpr char sem_ps_clear_process_notify_1[] = R"({
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
          "msgCounter": 39
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
                        "alternativesCount": 0
                      },
                      {
                        "totalSequencesCountMax": 1
                      },
                      {
                        "supportsReselection": false
                      }
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
