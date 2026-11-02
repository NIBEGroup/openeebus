namespace ma_mgcp_test {
namespace send {

static constexpr char measurement_subscription_call[] = R"({
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
          "msgCounter": 10
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
                            "feature": 2
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

}  // namespace send
}  // namespace ma_mgcp_test
