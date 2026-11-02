namespace cs_lpp_test {
namespace receive {

static constexpr char device_configuration_binding_request[] = R"({
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
          "msgCounter": 12
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
                "nodeManagementBindingRequestCall": [
                  {
                    "bindingRequest": [
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
                            "feature": 3
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
                              1
                            ]
                          },
                          {
                            "feature": 3
                          }
                        ]
                      },
                      {
                        "serverFeatureType": "DeviceConfiguration"
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

}  // namespace cs_lpp_test
