namespace ma_mpc_test {
namespace receive {

static constexpr char measurement_reply_ref_14[] = R"({
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
              "feature": 2
            }
          ]
        },
        {
          "msgCounter": 19
        },
        {
          "msgCounterReference": 14
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
                "measurementListData": [
                  {
                    "measurementData": [
                      [
                        {
                          "measurementId": 0
                        },
                        {
                          "valueType": "value"
                        },
                        {
                          "value": [
                            {
                              "number": 35000
                            },
                            {
                              "scale": -1
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

}  // namespace receive
}  // namespace ma_mpc_test
