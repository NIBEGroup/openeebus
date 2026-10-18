namespace cem_ohpcf_test {
namespace send {

static constexpr char sem_ps_subscription_call[] = R"({
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
          "msgCounter": 6
        },
        {
          "cmdClassifier": "call"
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
                "nodeManagementSubscriptionRequestCall": [
                  {
                    "subscriptionRequest": [
                      {
                        "clientAddress": [
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
                        "serverAddress": [
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
                        "serverFeatureType": "SmartEnergyManagementPs"
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
}  // namespace cem_ohpcf_test
