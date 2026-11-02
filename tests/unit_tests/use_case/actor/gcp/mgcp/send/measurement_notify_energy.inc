namespace gcp_mgcp_test {
namespace send {

static constexpr char measurement_notify_energy[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
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
              "feature": 6
            }
          ]
        },
        {
          "msgCounter": 17
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
                "function": "measurementListData"
              },
              {
                "filter": [
                  [
                    {
                      "cmdControl": [
                        {
                          "partial": []
                        }
                      ]
                    }
                  ]
                ]
              },
              {
                "measurementListData": [
                  {
                    "measurementData": [
                      [
                        {
                          "measurementId": 1
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 5000
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
                        }
                      ],
                      [
                        {
                          "measurementId": 2
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 8000
                            },
                            {
                              "scale": 0
                            }
                          ]
                        },
                        {
                          "valueSource": "measuredValue"
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
}  // namespace gcp_mgcp_test
