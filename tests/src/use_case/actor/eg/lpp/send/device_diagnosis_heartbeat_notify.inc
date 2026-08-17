namespace eg_lpp_test {
namespace send {

static constexpr char device_diagnosis_heartbeat_notify[] = R"({
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
          "msgCounter": 20
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
                "deviceDiagnosisHeartbeatData": [
                  {
                    "timestamp": "2026-04-07T15:37:54Z"
                  },
                  {
                    "heartbeatCounter": 1
                  },
                  {
                    "heartbeatTimeout": "PT60S"
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
}  // namespace eg_lpp_test
