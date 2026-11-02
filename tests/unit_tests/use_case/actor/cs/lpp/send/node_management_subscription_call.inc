namespace cs_lpp_test {
namespace send {

static constexpr char node_management_subscription_call[] = R"({
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
          "msgCounter": 3
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
                        "serverAddress": [
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
                        "serverFeatureType": "NodeManagement"
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

}  // namespace cs_lpp_test
