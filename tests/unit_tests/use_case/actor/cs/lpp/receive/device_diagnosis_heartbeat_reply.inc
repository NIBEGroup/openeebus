namespace cs_lpp_test {
namespace receive {

static constexpr char device_diagnosis_heartbeat_reply[] = R"({
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
                1
              ]
            },
            {
              "feature": 5
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
                1
              ]
            },
            {
              "feature": 1
            }
          ]
        },
        {
          "msgCounter": 17
        },
        {
          "msgCounterReference": 6
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
                "deviceDiagnosisHeartbeatData": [
                  {
                    "timestamp": "2025-02-07T14:55:02Z"
                  },
                  {
                    "heartbeatCounter": 1
                  },
                  {
                    "heartbeatTimeout": "PT1M"
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
