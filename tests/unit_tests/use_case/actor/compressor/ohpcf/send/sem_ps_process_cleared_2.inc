namespace compressor_ohpcf_test {
namespace send {

static constexpr char sem_ps_process_cleared_2[] = R"({
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
          "msgCounter": 25
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

}  // namespace send
}  // namespace compressor_ohpcf_test
