namespace gcp_mgcp_test {
namespace receive {

static constexpr char measurement_subscription_request[] = R"({
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
          "msgCounter": 8
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
                            "feature": 6
                          }
                        ]
                      },
                      {
                        "serverAddress": [
                          {
                            "device": "d:_n:GridConnectionPoint_123456789"
                          },
                          {
                            "entity": [
                              1
                            ]
                          },
                          {
                            "feature": 2
                          }
                        ]
                      },
                      {
                        "serverFeatureType": "Measurement"
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
}  // namespace gcp_mgcp_test
